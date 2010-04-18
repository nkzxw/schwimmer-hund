//TODO: comentarios
//TODO: ver si hay "limit of file descriptors per process" Puede llegar a complicar el monitoreo usando kqueue
//TODO: ver que pasa con NetBSD, OpenBSD, Darwin, MacOSX, etc... aparentemente lo soportan


// http://en.wikipedia.org/wiki/Kqueue
// http://mark.heily.com/pnotify/
/*
pnotify has been ported to the following platforms:

* Linux 2.6
* OpenBSD 4.0
* NetBSD 3.1
* FreeBSD 6.2
* MacOS/X 10.4 

There are platforms that are not supported due to lack of developer resources. Interested parties with access to these platforms are encouraged to port the library to their platform. For example:

* Sun OpenSolaris will soon get a similar API called FEM that could be supported.
* Microsoft Windows has a filesystem event notification mechanism 
*/


#ifndef BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP

#include <string>
#include <vector>

// C-Std Headers
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>	// for strerror

#include <fcntl.h>
#include <sys/types.h>
#include <sys/event.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <unistd.h>

#include <boost/bind.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/foreach.hpp>
#include <boost/integer.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/os_services/change_types.hpp>
#include <boost/os_services/details/base_impl.hpp>
#include <boost/os_services/notify_filters.hpp>

//TODO: ver como arreglamos esto...
//#define EVENT_SIZE  ( sizeof (struct inotify_event) )
//#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )


//TODO: sacar, es solo para debug
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;


//TODO: sacar
static const int WATCH_MAX = 20000;


//TODO: sacar
enum {

	/** The atime of a file has been modified */
	PN_ACCESS 		= 0x1 << 0,
	/** A file was created in a watched directory */
	PN_CREATE		= 0x1 << 1,
	/** A file was deleted from a watched directory */
	PN_DELETE		= 0x1 << 2,
	/** The modification time of a file has changed */
	PN_MODIFY		= 0x1 << 3,



	/** Automatically delete the watch after a matching event occurs */
	PN_ONESHOT		= 0x1 << 4,
	/** An error condition in the underlying kernel event queue */
	PN_ERROR		= 0x1 << 5,

	PN_RENAME		= 0x1 << 6


} __PN_BITMASK;

#define PN_ALL_EVENTS	(PN_ACCESS | PN_CREATE | PN_DELETE | PN_MODIFY | PN_RENAME)

//TODO: sacar
/* kqueue(4) in MacOS/X does not support NOTE_TRUNCATE */
#ifndef NOTE_TRUNCATE
# define NOTE_TRUNCATE 0
#endif




namespace boost {
namespace os_services {
namespace detail {



struct fsitem; //forward-declaration

//TODO: ver boost::ptr_vector
typedef boost::shared_ptr<fsitem> watch_type;
typedef std::vector<watch_type> watch_collection_type;

struct fsitem
{

	~fsitem()
	{
//		std::cout << "--------------------- ~fsitem() ------------------------------" << std::endl;
//		std::cout << "this->path.native_file_string(): " << this->path.native_file_string() << std::endl;
	}

	boost::filesystem::path path;
	bool is_directory;

	//TODO: uno de estos dos (file_descriptor_ y watch_descriptor_) no es necesario
	int file_descriptor_;	/**< kqueue(4) watched file descriptor */
	int watch_descriptor_;	/**< Unique 'watch descriptor' */

	//char            path[255 + 1];	//path[PATH_MAX + 1];	/**< Path associated with fd */
	struct kevent event;
	boost::uint32_t mask;	/**< Mask of monitored events */

	int parent_watch_descriptor_;
	watch_type parent;

	//TODO: renombrar
    dev_t st_dev; /* ID of device containing file */
    ino_t st_ino; /* inode number */

	watch_collection_type subitems;

	

	//TODO: PONER PUNTERO A parent
};

//TODO: si es necesario para todas las implementaciones, pasar a base_impl
typedef boost::shared_ptr<boost::thread> thread_type;

#ifndef O_EVTONLY
#define O_EVTONLY O_RDONLY
#endif


class freebsd_impl : public base_impl<freebsd_impl>
{
public:

	freebsd_impl()
		: is_initialized_(false), closing_(false), kqueue_file_descriptor_(0), next_watch_(0)
	{}

//	~freebsd_impl()
//	{
//		//TODO: rehacer completamente el destructor...
//
//		closing_ = true;
//
//		if ( thread_ )
//		{
//			thread_->join();
//		}
//
//		if ( kqueue_file_descriptor_ != 0 )
//		{
//			//TODO:
////			BOOST_FOREACH(pair_type p, watch_descriptors_)
////			{
////				if ( p.second != 0 )
////				{
////					//int ret_value = ::inotify_rm_watch( kqueue_file_descriptor_, p.second );
////					int ret_value = 0;
////
////					if ( ret_value < 0 )
////					{
////						//TODO: analizar si esta es la forma optima de manejar errores.
////						std::ostringstream oss;
////						oss << "Failed to remove watch - Reason: "; //TODO: ver que usar en Linux/BSD << GetLastError();
////						throw (std::runtime_error(oss.str()));
////					}
////				}
////			}
//
//			// TODO: parece que close(0) cierra el standard input (CIN)
//			//int ret_value = ::close( kqueue_file_descriptor_ );
//			int ret_value = 0;
//
//			if ( ret_value < 0 )
//			{
//				std::ostringstream oss;
//				oss << "Failed to close file descriptor - Reason: "; //TODO: ver que usar en Linux/BSD << GetLastError();
//				throw (std::runtime_error(oss.str()));
//			}
//		}
//	}


	//TODO: agregar
	//void add_directory_impl ( const boost::filesystem::path& dir ) //throw (std::invalid_argument, std::runtime_error)

	void add_directory_impl ( const std::string& dir_name ) //throw (std::invalid_argument, std::runtime_error)
	{
		watch_type item(new fsitem);
		item->path = dir_name;		//boost::filesystem::path full_path( str_path, boost::filesystem::native );
		//TODO: asignar mask

		user_watches_.push_back(item);
		all_watches_.push_back(item);
	}
	//void remove_directory_impl(const std::string& dir_name) // throw (std::invalid_argument);

	//TODO: ver si hace falta hacer lo mismo para Windows
	void initialize() //private
	{
		//std::cout << "void initialize()" << std::endl;
		if (!is_initialized_)
		{
			kqueue_file_descriptor_ = kqueue(); //::kqueue();
			//std::cout << "kqueue_file_descriptor_" << kqueue_file_descriptor_ << std::endl;

			if ( kqueue_file_descriptor_ == -1 )   //< 0)
			{
				std::ostringstream oss;
				oss << "Failed to initialize monitor - Reason: " << std::strerror(errno);
				throw (std::runtime_error(oss.str()));
			}
			is_initialized_ = true;
		}
	}

	void create_watch ( watch_type watch )
	{
//		std::cout << "void create_watch( watch_type watch )" << std::endl;
//		std::cout << "watch->path.native_file_string(): " << watch->path.native_file_string() << std::endl;

		//TODO: ver esto...
		if ( watch->mask == 0 )
		{
			watch->mask = PN_ALL_EVENTS; //TODO: asignar lo que el usuario quiere monitorear...
		}

		//TODO: en el constructor
		watch->watch_descriptor_ = 0;
		watch->is_directory = false;

		struct kevent *event = &watch->event;
		int mask = watch->mask;

		//TODO: y esto?????? nunca se ejecuta, cual es la razon ????
		if (watch->watch_descriptor_ < 0)
		{
			std::ostringstream oss;
			oss << "Failed to monitor directory - Directory: " << watch->path.native_file_string() << " - Reason: " << std::strerror(errno);
			throw (std::invalid_argument(oss.str()));
		}

		//if ( (watch->fd = open( watch->path.native_file_string().c_str(), O_RDONLY )) < 0)
		watch->file_descriptor_ = open( watch->path.native_file_string().c_str(), O_EVTONLY );
		if ( watch->file_descriptor_ == -1 ) //< 0
		{
			//warn("opening path `%s' failed", watch->path);
			//return -1;

			std::ostringstream oss;
			//TODO:
			oss << "opening path failed: - Reason: " << std::strerror(errno);
			throw (std::invalid_argument(oss.str()));
		}


		//TODO: que hace el lstat ??? Que pasa si da error ??????
		struct stat st;
		if ( lstat( watch->path.native_file_string().c_str(), &st) < 0)
		{
			//TODO: error
			std::cout << "STAT ERROR -- 1 -- - Reason: " << std::strerror(errno) << std::endl;
			std::cout << "watch->path.native_file_string(): " << watch->path.native_file_string() << std::endl;

			//ptime now = second_clock::local_time();
			//std::cout << now << std::endl;
			ptime now = microsec_clock::local_time();
			std::cout << to_iso_string(now) << std::endl;

			return;
		}

		watch->st_dev = st.st_dev;
		watch->st_ino = st.st_ino;

		

		if ( boost::filesystem::is_directory( watch->path ) )
		{
			watch->is_directory = true;
			//scan_directory( watch );
			scan_directory( watch.get() );
		}

		/* Generate a new watch ID */
		/* FIXME - this never decreases and might fail */
		//if ((watch->wd = ++ctl->next_wd) > WATCH_MAX)

		if ( (watch->watch_descriptor_ = ++next_watch_) > WATCH_MAX )
		{
			//warn("watch_max exceeded");
			//return -1;
			std::ostringstream oss;
			//TODO:
			oss << "watch_max exceeded";
			throw (std::invalid_argument(oss.str()));
		}

		if ( watch->file_descriptor_ == 0 )
		{
			std::cout << "-------------------------------------------------------------------------------" << std::endl;
			std::cout << "watch->fd: " << watch->file_descriptor_ << std::endl;
			std::cout << "watch->wd: " << watch->watch_descriptor_ << std::endl;
			std::cout << "watch->path.native_file_string(): " << watch->path.native_file_string() << std::endl;
			std::cout << "-------------------------------------------------------------------------------" << std::endl;
		}


		//TODO: ver estos flags, deberia monitoriarse solo lo que el usuairo quiera monitorear...
		//unsigned int fflags = NOTE_DELETE |  NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE; //| NOTE_RENAME;
		unsigned int fflags = NOTE_DELETE |  NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE | NOTE_RENAME;
		//event->fflags = NOTE_DELETE |  NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE; //| NOTE_RENAME
		//| UKKQueueNotifyAboutAttributeChange];


		//EV_SET( event, watch->file_descriptor_, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR, fflags, 0, watch.get() );
		EV_SET( event, watch->file_descriptor_, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, fflags, 0, watch.get() );

		//TODO: ver si Windows y Linux saltan cuando se mofica el nombre del directorio raiz monitoreado.
		         // sino saltan, evisar que se use NOTE_RENAME con cualquier directorio raiz

				// -> Windows no salta...
				// -> Linux tampoco...
				// -> FreeBSD: ????????????

//		kev->fflags |= NOTE_RENAME; //Eliminado porque no sirve monitorear el dir raiz.
//		kev->fflags |= NOTE_TRUNCATE; //TODO: ver
//		kev->fflags |= NOTE_WRITE; //TODO: ver
//		kev->fflags |= NOTE_EXTEND; //TODO: ver
//		kev->fflags |= NOTE_ATTRIB; //TODO: ver
//		kev->fflags |= NOTE_DELETE; //TODO: ver
//		kev->fflags |= NOTE_RENAME; //TODO: ver
//		kev->fflags |= NOTE_REVOKE; //TODO: ver
//		kev->fflags |= NOTE_LINK; //TODO: ver

		//TODO: no está incluido en PN_ALL_EVENTS por eso lo agrego de una
		//TODO: ver como mapear las constantes de BSD con las de la interfaz que vamos a exponer al usuario...
		//TODO: vamos a tener que hacer algo similar a lo que estamos haciendo a continuacion...

		//if (mask & PN_ACCESS || mask & PN_MODIFY)
		//{
		//	event->fflags |= NOTE_ATTRIB;
		//}

		//if (mask & PN_CREATE)
		//{
		//	event->fflags |= NOTE_WRITE;
		//}

		//if (mask & PN_DELETE)
		//{
		//	event->fflags |= NOTE_DELETE | NOTE_WRITE;
		//}

		//if (mask & PN_MODIFY)
		//{
		//	event->fflags |= NOTE_WRITE | NOTE_TRUNCATE | NOTE_EXTEND;
		//}

		//if (mask & PN_ONESHOT)
		//{
		//	event->flags |= EV_ONESHOT;
		//}

		//std::cout << "kev->flags: " << kev->flags << std::endl;

		/* Add the kevent to the kernel event queue */
		int return_code = kevent(kqueue_file_descriptor_, event, 1, NULL, 0, NULL);
		if ( return_code == -1 ) //< 0)
		{
			//perror("kevent(2)");
			//return -1;

			std::ostringstream oss;
			//TODO:
			oss << "kevent(2): - Reason: " << std::strerror(errno);
			throw (std::invalid_argument(oss.str()));
		}
	}


//	//TODO: contemplar la opcion include_sub_directories_
//	void scan_directory( watch_type head_dir )
//	{
//		//std::cout << "void scan_directory( watch_type head_dir )" << std::endl;
//		//std::cout << "head_dir->path.native_file_string(): " << head_dir->path.native_file_string() << std::endl;
//
//		//TODO: STL --> std::transform o std::for_each o boost::lambda o BOOST_FOREACH
//		//TODO: watch_collection_type o all_watches_type ?????? GUARDA!!!!
//
//		//std::cout << "head_dir->subitems.size(): " << head_dir->subitems.size() << std::endl;
//		for (watch_collection_type::iterator it =  head_dir->subitems.begin(); it != head_dir->subitems.end(); ++it )
//		{
//			(*it)->mask = PN_DELETE;
//		}
//
//		//std::cout << "-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.---..--..-.-.-.-...-.-.-.-.-.-.-." << std::endl;
//
//		boost::filesystem::directory_iterator end_iter;
//		for ( boost::filesystem::directory_iterator dir_itr( head_dir->path ); dir_itr != end_iter; ++dir_itr )
//		{
//			try
//			{
//				bool found = false;
//
//				struct stat dir_st;
//				if ( lstat( dir_itr->path().native_file_string().c_str(), &dir_st) < 0)
//				{
//					//TODO: manejo de errores
//					std::cout << "STAT ERROR -- 2 -- - Reason: " << std::strerror(errno) << std::endl;
//					std::cout << "dir_itr->path().native_file_string(): " << dir_itr->path().native_file_string() << std::endl;
//
//					ptime now = microsec_clock::local_time();
//					std::cout << to_iso_string(now) << std::endl;
//
//				}
//
//				//TODO: no me gusta esta busqueda lineal...
//				//TODO: reemplazar por std::find o algo similar...
//				//TODO: user_watchs o all_watchs ?????? GUARDA!!!!
//				//Linear-search
//				for (watch_collection_type::iterator it =  head_dir->subitems.begin(); it != head_dir->subitems.end(); ++it )
//				{
//
//					//TODO: para que hago esto ? porque no comparar los path directamente? Funcionara ??
//					if (  (*it)->path.native_file_string() == dir_itr->path().native_file_string() )
//					{
//						//std::cout << "found filename: " << (*it)->path.native_file_string() << std::endl;
//						//std::cout << "(*it)->path.native_file_string(): " << (*it)->path.native_file_string() << std::endl;
//						found = true;
//					}
//
//					if ((dir_st.st_dev == (*it)->st_dev) &&  (dir_st.st_ino == (*it)->st_ino))
//					{
//						//std::cout << "found inode: " << (*it)->path.native_file_string() << " - " << dir_itr->path().native_file_string() << std::endl;
//					}
//
//
//				}
//
//				if ( !found )
//				{
//					//std::cout << "if ( !found )" << std::endl;
//
//					watch_type item(new fsitem);
//					item->path = dir_itr->path();
//
//					//TODO: ver en el codigo de pnotify: /* Add a watch if it is a regular file */
//					create_watch( item );
//					item->mask = PN_CREATE;
//					item->parent_watch_descriptor_ = head_dir->watch_descriptor_;
//					item->parent = head_dir;
//
////					std::cout << "PN_CREATE: " << PN_CREATE << std::endl;
////					std::cout << "item->path.native_file_string(): " << item->path.native_file_string() << std::endl;
////					std::cout << "item->mask: " << item->mask << std::endl;
////					std::cout << "item->parent_watch_descriptor_: " << item->parent_watch_descriptor_ << std::endl;
//
//
//					head_dir->subitems.push_back(item);
//				}
//			}
//			catch ( const std::exception & ex )
//			{
//				std::cout << dir_itr->path().native_file_string() << " " << ex.what() << std::endl;
//			}
//		}
//
//	}


	void scan_directory( fsitem* head_dir )
	{
//		std::cout << "void scan_directory( fsitem* head_dir )" << std::endl;
//		std::cout << "head_dir->path.native_file_string(): " << head_dir->path.native_file_string() << std::endl;

		watch_collection_type temp_file_list;

//		std::cout << "PN_CREATE: " << PN_CREATE << std::endl;
//		std::cout << "PN_ACCESS: " << PN_ACCESS << std::endl;
//		std::cout << "PN_DELETE: " << PN_DELETE << std::endl;
//		std::cout << "PN_MODIFY: " << PN_MODIFY << std::endl;
//		std::cout << "PN_ONESHOT: " << PN_ONESHOT << std::endl;
//		std::cout << "PN_ERROR: " << PN_ERROR << std::endl;

		//TODO: STL --> std::transform o std::for_each o boost::lambda o BOOST_FOREACH
		//TODO: watch_collection_type o all_watches_type ?????? GUARDA!!!!
		for (watch_collection_type::iterator it =  head_dir->subitems.begin(); it != head_dir->subitems.end(); ++it )
		{
			(*it)->mask = PN_DELETE;  //TODO: recursivo
		}

//		std::cout << "PN_DELETE: " << PN_DELETE << std::endl;
//		std::cout << "PN_CREATE: " << PN_CREATE << std::endl;

		boost::filesystem::directory_iterator end_iter;
		for ( boost::filesystem::directory_iterator dir_itr( head_dir->path ); dir_itr != end_iter; ++dir_itr )
		{
			try
			{
				//std::cout << "--- Finding File Name: " << dir_itr->path().native_file_string() << std::endl;
				bool found_filename = false;
				bool found_inode = false;

				struct stat dir_st;
				if ( lstat( dir_itr->path().native_file_string().c_str(), &dir_st) < 0)
				{
					//TODO: manejo de errores
					std::cout << "STAT ERROR -- 3 -- - Reason: " << std::strerror(errno) << std::endl;
					std::cout << "dir_itr->path().native_file_string(): " << dir_itr->path().native_file_string() << std::endl;

					ptime now = microsec_clock::local_time();
					std::cout << to_iso_string(now) << std::endl;

				}

				//TODO: reemplazar por std::find o algo similar...
				//TODO: user_watchs o all_watchs ?????? GUARDA!!!!

				//Linear-search
				for (watch_collection_type::iterator it =  head_dir->subitems.begin(); it != head_dir->subitems.end(); ++it )
				{
					if (  dir_st.st_dev == (*it)->st_dev && dir_st.st_ino == (*it)->st_ino && (*it)->path.native_file_string() == dir_itr->path().native_file_string() )
					{
						//std::cout << "found inode & filename: " << (*it)->path.native_file_string() << std::endl;
						(*it)->mask = 0; //-999;
						//std::cout << "(*it)->path.native_file_string(): " << (*it)->path.native_file_string() << std::endl;
						found_filename = true;
						found_inode = true;
					}
					else
					{

						if (  dir_st.st_dev == (*it)->st_dev && dir_st.st_ino == (*it)->st_ino )
						{
							//std::cout << "found inode: " << (*it)->path.native_file_string() << std::endl;
							found_inode = true;
						}

						if ( (*it)->path.native_file_string() == dir_itr->path().native_file_string() )
						{
							//std::cout << "found filename: " << (*it)->path.native_file_string() << std::endl;
							found_filename = true;
						}
					}
				}


				if ( !found_filename && !found_inode )	//Archivo nuevo
				{
					std::cout << "if ( !found_filename && !found_inode )" << std::endl;


//					std::cout << "if (!found_filename && found_inode)" << std::endl;
//					std::cout << "dir_itr->path().native_file_string(): " << dir_itr->path().native_file_string() << std::endl;
//					std::cout << "dir_st.st_dev: " << dir_st.st_dev << std::endl;
//					std::cout << "dir_st.st_ino: " << dir_st.st_ino << std::endl;

					watch_type item(new fsitem);
					item->path = dir_itr->path();

					//TODO: volver a habilitar hasta el fin de las pruebas
					//std::cout << "File created: " << item->path.native_file_string() << std::endl;


					//std::cout << "DEBUG 1" << std::endl;

					//TODO: ver en el codigo de pnotify: /* Add a watch if it is a regular file */
					create_watch( item );
					item->mask = PN_CREATE;
					item->parent_watch_descriptor_ = head_dir->watch_descriptor_;
					//item->parent = head_dir;
					item->st_dev = dir_st.st_dev;
					item->st_ino = dir_st.st_ino;

					head_dir->subitems.push_back(item);

					//std::cout << "DEBUG 2" << std::endl;
				}

				if ( !found_filename && found_inode )
				{
					std::cout << "if ( !found_filename && found_inode )" << std::endl;
////					std::cout << "if ( !found_filename && found_inode )" << std::endl;
////					std::cout << "dir_itr->path().native_file_string(): " << dir_itr->path().native_file_string() << std::endl;
////					std::cout << "dir_st.st_dev: " << dir_st.st_dev << std::endl;
////					std::cout << "dir_st.st_ino: " << dir_st.st_ino << std::endl;
//
//					watch_type item(new fsitem);
//					item->path = dir_itr->path();
//					item->mask = PN_CREATE;
//					item->parent_watch_descriptor_ = head_dir->watch_descriptor_;
//					item->st_dev = dir_st.st_dev;
//					item->st_ino = dir_st.st_ino;
//
//					temp_file_list.push_back(item);
				}


			}
			catch ( const std::exception & ex )
			{
				std::cout << dir_itr->path().native_file_string() << " " << ex.what() << std::endl;
			}
		}

		//std::cout << "DEBUG 3" << std::endl;

		for (watch_collection_type::iterator it =  head_dir->subitems.begin(); it != head_dir->subitems.end(); ++it )
		{
			if ( (*it)->mask != 0 ) //-999 )
			{
				bool found = false;
				for (watch_collection_type::iterator it2 =  temp_file_list.begin(); it2 != temp_file_list.end(); ++it2 )
				{
					if ( (*it2)->mask != 0 ) //-999 )
					{
						if (  (*it)->st_dev == (*it2)->st_dev && (*it)->st_ino == (*it2)->st_ino )
						{
							//std::cout << "found inode: " << (*it)->path.native_file_string() << " - " << (*it2)->path.native_file_string() << std::endl;
							found = true;

							//TODO: volver a habilitar hasta el fin de las pruebas
							//std::cout << "File: " << (*it)->path.native_file_string() << " renamed to: " << (*it2)->path.native_file_string() << std::endl;

							(*it)->mask = PN_RENAME; //  -998;
							(*it2)->mask = 0; //-999;	//NO PROCESAR
							break;
						}

						if ( (*it)->path.native_file_string() == (*it2)->path.native_file_string() )
						{
							//TODO: que hacemos acá, ver cuando podría a generarse este caso... Me parece que nunca.
							//TODO: volver a habilitar hasta el fin de las pruebas
							//std::cout << "found filename: " << (*it)->path.native_file_string() << " - " << (*it2)->path.native_file_string() << std::endl;
							found = true;
						}
					}
				}
			}
		}


		//std::cout << "DEBUG 4" << std::endl;

//		for (watch_collection_type::iterator it =  temp_file_list.begin(); it != temp_file_list.end(); ++it )
//		{
//			if ( (*it)->mask != 0 ) //-999 )
//			{
////				std::cout << "--- NEW FILE ---" << std::endl;
////				std::cout << "(*it)->path.native_file_string(): " << (*it)->path.native_file_string() << std::endl;
//
//				watch_type item(new fsitem);
//				item->path = (*it)->path;
//
//				//TODO: volver a habilitar hasta el fin de las pruebas
//				//std::cout << "File created: " << item->path.native_file_string() << std::endl;
//
//
//				//TODO: ver en el codigo de pnotify: /* Add a watch if it is a regular file */
//				create_watch( item );
//				item->mask = PN_CREATE;
//				item->parent_watch_descriptor_ = head_dir->watch_descriptor_;
//				item->st_dev = (*it)->st_dev;
//				item->st_ino = (*it)->st_ino;
//
//				head_dir->subitems.push_back(item);
//			}
//		}

		//std::cout << "DEBUG 5" << std::endl;


	}

	void start()
	{
		//std::cout << "void start()" << std::endl;
		initialize();

		//TODO: BOOST_FOREACH
		for (watch_collection_type::iterator it =  user_watches_.begin(); it != user_watches_.end(); ++it )
		{
			create_watch( *it );
		}

		thread_.reset( new boost::thread( boost::bind(&freebsd_impl::handle_directory_changes, this) ) );
	}

public: //private:  //TODO:

	void handle_directory_changes()
	{
		while ( ! closing_ )
		{
//			struct pnotify_event *evp;
			struct kevent event;
			int return_code;

			//std::cout << "waiting for kernel event.." << std::endl;

			//TODO: ver timeout
			return_code = kevent ( kqueue_file_descriptor_, NULL, 0, &event, 1, NULL );
			if ( return_code == -1 || event.flags & EV_ERROR) //< 0
			{
				//TODO:
				//warn("kevent(2) failed");
				std::cout << "ERRRRRRRRRRRRRORRR ON kevent Wait" << std::endl;
				return;
			}




			if ( ! closing_ )
			{

				//TODO: esto puede ser un tema, porque el shared_ptr (watch_type) va a tener el contador en 1 y cuando salga de scope va a hacer delete de la memoria...
				//watch_type watch( (fsitem*) event.udata );
				fsitem* watch =  (fsitem*) event.udata;

				/* Workaround:

				   Deleting a file in a watched directory causes two events:
			     		NOTE_MODIFY on the directory
						NOTE_DELETE on the file
				   We ignore the NOTE_DELETE on the file.
				*/
				//if ( watch->parent_watch_descriptor_ && event.fflags & NOTE_DELETE )
				//{
				//	std::cout << "-*-*-*-*-*--*-*-*-*-** IGNORE NOTE_DELETE" << std::endl;
				//	//goto retry;
				//	continue;
				//}

	//			std::cout << "----------------------------------------------------------------------------" << std::endl;
	//			std::cout << "watch->fd: " << watch->fd << std::endl;
	//			std::cout << "watch->wd: " << watch->wd << std::endl;
	//			std::cout << "watch->mask: " << watch->mask << std::endl;
	//			std::cout << "watch->path.native_file_string(): " << watch->path.native_file_string() << std::endl;
	//			std::cout << "----------------------------------------------------------------------------" << std::endl;



				if ( event.fflags & NOTE_DELETE )
				{
					std::cout << "---------------------------------- NOTE_DELETE ---------------------------------------" << std::endl;

					std::cout << "watch: " << watch << std::endl;
					std::cout << "watch->fd: " << watch->file_descriptor_ << std::endl;
					std::cout << "watch->wd: " << watch->watch_descriptor_ << std::endl;
					std::cout << "watch->parent_watch_descriptor_: " << watch->parent_watch_descriptor_ << std::endl;
					std::cout << "watch->path: " << watch->path.native_file_string() << std::endl;
					std::cout << "watch->is_directory: " << watch->is_directory << std::endl;
					std::cout << "watch->mask: " << watch->mask << std::endl;
					std::cout << "watch->st_dev: " << watch->st_dev << std::endl;
					std::cout << "watch->st_ino: " << watch->st_ino << std::endl;


					//std::cout << "event.ident: " << event.ident << std::endl;
					//std::cout << "event.filter: " << event.filter << std::endl;
					//std::cout << "event.flags: " << event.flags << std::endl;
					//std::cout << "event.fflags: " << event.fflags << std::endl;
					//std::cout << "event.data: " << event.data << std::endl;
					//std::cout << "event.udata: " << event.udata << std::endl;

					std::cout << "--------------------------------------------------------------------------------------" << std::endl;
				}


				if ( event.fflags & NOTE_RENAME )
				{
					std::cout << "---------------------------------- NOTE_RENAME ---------------------------------------" << std::endl;

					std::cout << "watch: " << watch << std::endl;
					std::cout << "watch->fd: " << watch->file_descriptor_ << std::endl;
					std::cout << "watch->wd: " << watch->watch_descriptor_ << std::endl;
					std::cout << "watch->parent_watch_descriptor_: " << watch->parent_watch_descriptor_ << std::endl;
					std::cout << "watch->path: " << watch->path.native_file_string() << std::endl;
					std::cout << "watch->is_directory: " << watch->is_directory << std::endl;
					std::cout << "watch->mask: " << watch->mask << std::endl;
					std::cout << "watch->st_dev: " << watch->st_dev << std::endl;
					std::cout << "watch->st_ino: " << watch->st_ino << std::endl;


					//std::cout << "event.ident: " << event.ident << std::endl;
					//std::cout << "event.filter: " << event.filter << std::endl;
					//std::cout << "event.flags: " << event.flags << std::endl;
					//std::cout << "event.fflags: " << event.fflags << std::endl;
					//std::cout << "event.data: " << event.data << std::endl;
					//std::cout << "event.udata: " << event.udata << std::endl;

					boost::filesystem::path parent;

					//TODO: find
					for (watch_collection_type::iterator it =  all_watches_.begin(); it != all_watches_.end(); ++it )
					{
						if ( watch->parent_watch_descriptor_ == (*it)->watch_descriptor_ )
						{
							parent = (*it)->path;
						}
					}

					if ( ! parent.empty() )
					{
						boost::filesystem::directory_iterator end_iter;

						//TODO: pasar a metodo estatico
						for ( boost::filesystem::directory_iterator dir_itr( parent ); dir_itr != end_iter; ++dir_itr )
						{
							struct stat dir_st;
							
							int return_code = lstat( dir_itr->path().native_file_string().c_str(), &dir_st);
							if ( return_code == -1)
							{
								//TODO: manejo de errores
								std::cout << "STAT ERROR -- 4 -- - Reason: " << std::strerror(errno) << std::endl;
								std::cout << "dir_itr->path().native_file_string(): " << dir_itr->path().native_file_string() << std::endl;

								ptime now = microsec_clock::local_time();
								std::cout << to_iso_string(now) << std::endl;
							}
							else
							{
								if (  dir_st.st_dev == watch->st_dev && dir_st.st_ino == watch->st_ino )
								{
									std::cout << "Nuevo Nombre de Archivo:  " << dir_itr->path().native_file_string() << std::endl;
								}
							}
						}
					}














					std::cout << "--------------------------------------------------------------------------------------" << std::endl;

				}

				if ( event.fflags & NOTE_WRITE )
				{
					std::cout << "---------------------------------- NOTE_WRITE ---------------------------------------" << std::endl;
					
					std::cout << "watch: " << watch << std::endl;
					std::cout << "watch->fd: " << watch->file_descriptor_ << std::endl;
					std::cout << "watch->wd: " << watch->watch_descriptor_ << std::endl;
					std::cout << "watch->parent_watch_descriptor_: " << watch->parent_watch_descriptor_ << std::endl;
					std::cout << "watch->path: " << watch->path.native_file_string() << std::endl;
					std::cout << "watch->is_directory: " << watch->is_directory << std::endl;
					std::cout << "watch->mask: " << watch->mask << std::endl;
					std::cout << "watch->st_dev: " << watch->st_dev << std::endl;
					std::cout << "watch->st_ino: " << watch->st_ino << std::endl;


					//std::cout << "event.ident: " << event.ident << std::endl;
					//std::cout << "event.filter: " << event.filter << std::endl;
					//std::cout << "event.flags: " << event.flags << std::endl;
					//std::cout << "event.fflags: " << event.fflags << std::endl;
					//std::cout << "event.data: " << event.data << std::endl;
					//std::cout << "event.udata: " << event.udata << std::endl;
					std::cout << "--------------------------------------------------------------------------------------" << std::endl;
				}


				//if (event.fflags & NOTE_WRITE)
				//{
				//	std::cout << "NOTE_WRITE -> PN_MODIFY" << std::endl;
				//}
				//if (event.fflags & NOTE_TRUNCATE)
				//{
				//	std::cout << "NOTE_TRUNCATE -> PN_MODIFY" << std::endl;
				//}
				//if (event.fflags & NOTE_EXTEND)
				//{
				//	std::cout << "NOTE_EXTEND -> PN_MODIFY" << std::endl;
				//}
				//if (event.fflags & NOTE_ATTRIB)
				//{
				//	std::cout << "NOTE_ATTRIB -> PN_ATTRIB" << std::endl;
				//}

				if ( event.fflags & NOTE_RENAME )
				{
					//std::cout << "NOTE_RENAME -> XXXXXXXXX" << std::endl;
					//std::cout << "watch->fd: " << watch->fd << std::endl;
					//std::cout << "OLDNAME: " << watch->path.native_file_string() << std::endl;


					//
//					//TODO: buscar el directorio padre de la forma más eficiente posible a traves del parent_
//					std::cout << "Parent: " << user_watches_[0]->path.native_file_string() << std::endl;
//
//
//					struct stat old_st;
//					if (fstat(watch->fd, &old_st) < 0)
//					{
//						//warn("fstat(2) failed");
//						std::cout << "ERROR STAT" << std::endl;
//						return;
//					}
//
//////					std::cout << "old_st.st_dev: " << old_st.st_dev << std::endl;
////					std::cout << "old_st.st_ino: " << old_st.st_ino << std::endl;
////					std::cout << "old_st.st_mode: " << old_st.st_mode << std::endl;
////					std::cout << "old_st.st_nlink: " << old_st.st_nlink << std::endl;
////					std::cout << "old_st.st_rdev: " << old_st.st_rdev << std::endl;
////					std::cout << "old_st.st_atime: " << old_st.st_atime << std::endl;
////					std::cout << "old_st.st_mtime: " << old_st.st_mtime << std::endl;
////					std::cout << "old_st.st_ctime: " << old_st.st_ctime << std::endl;
////					std::cout << "S_ISDIR(old_st.st_mode): " << S_ISDIR(old_st.st_mode) << std::endl;
////					std::cout << "S_ISREG(old_st.st_mode): " << S_ISREG(old_st.st_mode) << std::endl;
////					std::cout << "S_ISLNK(old_st.st_mode): " << S_ISLNK(old_st.st_mode) << std::endl;
//
//
//
//
//
//					boost::filesystem::directory_iterator end_iter;
//					//TODO: hardcode
//					for ( boost::filesystem::directory_iterator dir_itr( user_watches_[0]->path ); dir_itr != end_iter; ++dir_itr )
//					{
//						try
//						{
//							struct stat it_st;
//
//							//if ( stat( dir_itr->path().native_file_string().c_str(), &it_st) == 0)
//							if ( lstat( dir_itr->path().native_file_string().c_str(), &it_st) == 0)
//							{
////								std::cout << "dir_itr->path().native_file_string(): " << dir_itr->path().native_file_string() << std::endl;
//////								std::cout << "it_st.st_dev: " << it_st.st_dev << std::endl;
////								std::cout << "it_st.st_ino: " << it_st.st_ino << std::endl;
////								std::cout << "it_st.st_mode: " << it_st.st_mode << std::endl;
////								std::cout << "it_st.st_nlink: " << it_st.st_nlink << std::endl;
////								std::cout << "it_st.st_rdev: " << it_st.st_rdev << std::endl;
////								std::cout << "it_st.st_atime: " << it_st.st_atime << std::endl;
////								std::cout << "it_st.st_mtime: " << it_st.st_mtime << std::endl;
////								std::cout << "it_st.st_ctime: " << it_st.st_ctime << std::endl;
////								std::cout << "S_ISDIR(it_st.st_mode): " << S_ISDIR(it_st.st_mode) << std::endl;
////								std::cout << "S_ISREG(it_st.st_mode): " << S_ISREG(it_st.st_mode) << std::endl;
////								std::cout << "S_ISLNK(it_st.st_mode): " << S_ISLNK(it_st.st_mode) << std::endl;
//
//								if ((it_st.st_dev == old_st.st_dev) &&  (it_st.st_ino == old_st.st_ino))
//								{
//									std::cout << "NEW NAME " << dir_itr->path().native_file_string() << std::endl;
//								}
//							}
//							else
//							{
//								std::cout << "ERROR EN STAT" << std::endl;
//								std::cout << "dir_itr->path().native_file_string(): " << dir_itr->path().native_file_string() << std::endl;
//							}
//						}
//						catch ( const std::exception & ex )
//						{
//							std::cout << dir_itr->path().native_file_string() << " " << ex.what() << std::endl;
//						}
//					}



				}


				//if (event.fflags & NOTE_REVOKE)
				//{
				//	std::cout << "NOTE_REVOKE -> XXXXXXXXX" << std::endl;
				//}
				//if (event.fflags & NOTE_LINK)
				//{
				//	std::cout << "NOTE_LINK -> XXXXXXXXX" << std::endl;
				//}


				//if ( boost::filesystem::is_directory( watch->path ) )
				if ( watch->is_directory )
				{
					if (event.fflags & NOTE_WRITE)
					{
						directory_event_handler( watch );
					}
					else if (event.fflags & NOTE_DELETE)	//TODO: Handle the deletion of a watched directory
					{
						//warn("unimplemented - TODO");
						//return -1;

						//TODO: cerrar el monitoreo del directorio e hijos...y el file_descriptor_
					}
					else
					{
						//						warn("unknown event recieved");
						//						return -1;
					}
				}
				else
				{
//					if (event.fflags & NOTE_WRITE)
//						evt->mask |= PN_MODIFY;
//					if (event.fflags & NOTE_TRUNCATE)
//						evt->mask |= PN_MODIFY;
//					if (event.fflags & NOTE_EXTEND)
//						evt->mask |= PN_MODIFY;
//			#if TODO
//					// not implemented yet
//					if (event.fflags & NOTE_ATTRIB)
//						evt->mask |= PN_ATTRIB;
//			#endif
//					if (event.fflags & NOTE_DELETE)
//						evt->mask |= PN_DELETE;

//					/* Construct a pnotify_event structure */
//					if ((evp = calloc(1, sizeof(*evp))) == NULL)
//					{
//						warn("malloc failed");
//						return -1;
//					}
//
//					/* If the event happened within a watched directory,
//					   add the filename and the parent watch descriptor.
//					*/
//					if (watch->parent_watch_descriptor_)
//					{
//
//						/* KLUDGE: remove the leading basename */
//						char *fn = strrchr(watch->path, '/') ;
//						if (!fn)
//						{
//							fn = watch->path;
//						}
//						else
//						{
//							fn++;
//						}
//
//						evt->wd = watch->parent_watch_descriptor_;
//						/* FIXME: more error handling */
//						(void) strncpy(evt->name, fn, strlen(fn));
//					}
//
//					/* Add the event to the list of pending events */
//					memcpy(evp, evt, sizeof(*evp));
//					STAILQ_INSERT_TAIL(&ctl->event, evp, entries);
//
//					dprint_event(evt);

				}







				//watch = (struct pnotify_watch *) event.udata;
//				watch = (watch_data*) event.udata;

//				std::cout << "watch->fd: " << watch->fd << std::endl;
//				std::cout << "watch->wd: " << watch->wd << std::endl;
//				std::cout << "watch->mask: " << watch->mask << std::endl;
//				//std::cout << "watch.fd: " << watch.fd << std::endl;

				//TODO: ver este tema, porque se puede estar disparando contra el directorui
//				if (watch->parent_watch_descriptor_ && event.fflags & NOTE_DELETE)
//				{
//					dprintf("ignoring NOTE_DELETE on a watched file\n");
//					goto retry;
//				}
			}
		}
	}

	//TODO: cambiarle el nombre porque es parecido al event handler general y no se indica bien que hace cada uno...
	//void directory_event_handler(struct kevent kev, struct pnotify_cb * ctl, struct pnotify_watch * watch)
//	void directory_event_handler( watch_type head_dir )
//	{
//		struct pnotify_event *ev;
//		struct dentry  *dptr, *dtmp;
//
//		//TODO: ????
//		//assert(ctl && watch);
//
//		scan_directory( head_dir );
//
//
//		for (watch_collection_type::iterator it =  head_dir->subitems.begin(); it != head_dir->subitems.end(); ++it )
//		{
//			if ((*it)->mask == 0) /* Skip files that have not changed */
//			{
//				continue;
//			}
//
//			//TODO: manejar el evento...
////			/* Construct a pnotify_event structure */
////			if ((ev = calloc(1, sizeof(*ev))) == NULL)
////			{
////				warn("malloc failed");
////				return -1;
////			}
////			ev->wd = watch->wd;
////			ev->mask = dptr->mask;
////			(void) strlcpy(ev->name, dptr->ent.d_name, sizeof(ev->name));
////			dprint_event(ev);
////
////			/* Add the event to the list of pending events */
////			STAILQ_INSERT_TAIL(&ctl->event, ev, entries);
//
//			/* Remove the directory entry for a deleted file */
//			if ( (*it)->mask & PN_DELETE )
//			{
//				std::cout << "ELIMINANDO ITEM DE LA LISTA" << std::endl;
//				std::cout << "(*it)->path.native_file_string(): " << (*it)->path.native_file_string() << std::endl;
//
////				LIST_REMOVE(dptr, entries);
////				free(dptr);
//			}
//
//		}
//
//	}


	void directory_event_handler( fsitem* head_dir ) //TODO: cambiarle el nombre porque un handler se confunde con un handler de un evento...
	{
//		std::cout << "void directory_event_handler( fsitem* head_dir )" << std::endl;
//		std::cout << "head_dir->path.native_file_string(): " << head_dir->path.native_file_string() << std::endl;

		struct pnotify_event *ev;
		struct dentry *dptr, *dtmp;

		//TODO: ????
		//assert(ctl && watch);

		scan_directory( head_dir );

//		//std::cout << "DEBUG 6" << std::endl;
//
//		watch_collection_type::iterator it = head_dir->subitems.begin();
//		while ( it != head_dir->subitems.end() )
//		{
//
//			//std::cout << "DEBUG 7" << std::endl;
//			//std::cout << "File removed: " << (*it)->path.native_file_string() << std::endl;
//
//			if ((*it)->mask == 0) /* Skip files that have not changed */
//			{
//				++it;
//				continue;
//			}
//
//			//std::cout << "DEBUG 8" << std::endl;
//
//
//			/* Remove the directory entry for a deleted file */
//			if ( (*it)->mask & PN_DELETE )
//			{
//				//std::cout << "DEBUG 9" << std::endl;
//
//
//				//std::cout << "File removed: " << (*it)->path.native_file_string() << std::endl;
//
////				std::cout << "ELIMINANDO ITEM DE LA LISTA" << std::endl;
////				std::cout << "(*it)->path.native_file_string(): " << (*it)->path.native_file_string() << std::endl;
////				//std::cout << "(*it)->mask: " << (*it)->mask << std::endl;
//
//
//				it = head_dir->subitems.erase(it);
//			}
//			else
//			{
//				//std::cout << "DEBUG 10" << std::endl;
//
//				++it;
//			}
//
//			//std::cout << "DEBUG 11" << std::endl;
//
//
//		}
//		//std::cout << "DEBUG 12" << std::endl;

	}

protected:

	//TODO: las tres funciones siguientes estÃ¡n duplicadas en windows_impl y freebsd_impl -> RESOLVER
	// inline void notify_file_system_event_args( int action, const std::string& directory, const std::string& name )

	thread_type thread_;

	bool is_initialized_;

	//TODO: cambiarle el nombre
	int kqueue_file_descriptor_; // file descriptor
	int next_watch_; //TODO: analizar si es necesario
	bool closing_;

	watch_collection_type user_watches_;
	watch_collection_type all_watches_;
	
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP
