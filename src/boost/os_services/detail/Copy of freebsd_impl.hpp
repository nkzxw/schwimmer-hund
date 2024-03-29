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
#include <boost/os_services/detail/base_impl.hpp>
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


//O_EVTONLY solo existe en MacOSX, no en FreeBSD
#ifndef O_EVTONLY
#define O_EVTONLY O_RDONLY
#endif


namespace boost {
namespace os_services {
namespace detail {

//TODO: que onda????? esto esta en freebsd_impl
static int kqueue_file_descriptor_ = 0; //TODO: que onda????? esto esta en freebsd_impl

	
struct filesystem_item;		//forward-declaration
struct user_entry;	//forward-declaration

//TODO: ver boost::ptr_vector
typedef boost::shared_ptr<filesystem_item> watch_type;			//TODO: renombrar
typedef std::vector<watch_type> watch_collection_type;	//TODO: renombrar

//TODO: pasar a otro archivo...
struct file_inode_info
{
	//TODO: public, protected, private ?????
	file_inode_info ( )
		: device_id_(0), inode_number_(0)
	{}

	file_inode_info ( dev_t device_id, ino_t inode_number )
		: device_id_(device_id), inode_number_(inode_number)
	{}

	file_inode_info ( const boost::filesystem::path& path )
	{
		set ( path );
	}

	file_inode_info ( const file_inode_info& other )
	{
		this->device_id_ = other.device_id_;
		this->inode_number_ = other.inode_number_;
	}
	
	file_inode_info& operator=(const file_inode_info& other)
	{
		if ( this != &other )
		{
			this->device_id_ = other.device_id_;
			this->inode_number_ = other.inode_number_;
		}
		return *this;
	}

	void set ( dev_t device_id, ino_t inode_number )
	{
		this->device_id_ = device_id;
		this->inode_number_ = inode_number;
	}

	void set ( const struct stat& st )
	{
		this->device_id_ = st.st_dev;
		this->inode_number_ = st.st_ino;
	}

	void set ( const boost::filesystem::path& path )
	{
		struct stat st;

		int return_code = lstat( path.native_file_string().c_str(), &st);
		if ( return_code < 0)
		{
			//TODO: error
			std::cout << "STAT ERROR -- on file_inode_info -- - Reason: " << std::strerror(errno) << std::endl;
			std::cout << "watch->path.native_file_string(): " << path.native_file_string() << std::endl;

			//ptime now = second_clock::local_time();
			//std::cout << now << std::endl;
			ptime now = microsec_clock::local_time();
			std::cout << to_iso_string(now) << std::endl;

			return;
		}
		else
		{
			set ( st );
		}
	}

	bool operator==(const file_inode_info& other) const
	{
		return ( this->device_id_ == other.device_id_ && this->inode_number_ == other.inode_number_ );
	}

	bool operator==(const struct stat& other) const
	{
		return ( this->device_id_ == other.st_dev && this->inode_number_ == other.st_ino );
	}


	dev_t device_id_;
	ino_t inode_number_;
};

//TODO: renombrar
class filesystem_item
{
public:
	
	typedef boost::shared_ptr<filesystem_item> pointer_type; 

	//TODO: agegar metodo add_subitem

	filesystem_item ( const boost::filesystem::path& path, user_entry* root_user_entry )
		: root_user_entry_(root_user_entry), parent_(0), is_directory_(false), file_descriptor_(0) //, watch_descriptor_(0)
	{
		std::cout << "--------------------- fs_item ( const boost::filesystem::path& path, const user_entry* const root_user_entry ) ------------------------------" << std::endl;
		//std::cout << "this->path.native_file_string(): " << this->path.native_file_string() << std::endl;

		set_path( path );
	}

	filesystem_item ( const boost::filesystem::path& path, user_entry* root_user_entry, filesystem_item* parent )
		: root_user_entry_(root_user_entry), parent_(parent), is_directory_(false), file_descriptor_(0) //, watch_descriptor_(0)
	{
		std::cout << "--------------------- fs_item ( const boost::filesystem::path& path, const user_entry* const root_user_entry, const fs_item* const parent ) ------------------------------" << std::endl;
		//std::cout << "this->path.native_file_string(): " << this->path.native_file_string() << std::endl;

		set_path( path );
	}

	~filesystem_item()
	{
		//TODO: Eliminar los subitems 

		std::cout << "--------------------- ~fsitem() ------------------------------" << std::endl;
		//std::cout << "this->path.native_file_string(): " << this->path.native_file_string() << std::endl;

		if ( this->file_descriptor_ != 0 )
		{
			int ret_value = close( this->file_descriptor_ ); //::close

			if ( ret_value < 0 )
			{
				std::ostringstream oss;
				oss << "Failed to close file descriptor - Reason: "; //TODO: ver que usar en Linux/BSD << GetLastError();
				throw (std::runtime_error(oss.str()));
			}
		}

	}

	//bool operator==(const fs_item& other) const
	//{
	//	return ( this->path_ == other.path_ && this->inode_info_ == other.inode_info_ );
	//}


	bool is_equal(const filesystem_item& other) const
	{
		return ( this->path_ == other.path_ && this->inode_info_ == other.inode_info_ );
	}

	bool is_equal(const watch_type& other) const
	{
		return ( this->path_ == other->path_ && this->inode_info_ == other->inode_info_ );
	}

	bool is_equal(filesystem_item* other) const
	{
		return ( this->path_ == other->path_ && this->inode_info_ == other->inode_info_ );
	}

	bool is_equal(const file_inode_info& inode_info, const boost::filesystem::path& path) const
	{
		return (  this->inode_info_ == inode_info && this->path_ == path );
	}

	void open ()
	{
		this->file_descriptor_ = open( path_.native_file_string().c_str(), O_EVTONLY );
		if ( this->file_descriptor_ == -1 ) //< 0
		{
			//warn("opening path `%s' failed", watch->path);
			//return -1;

			std::ostringstream oss;
			//TODO:
			oss << "opening path failed: - Reason: " << std::strerror(errno);
			throw (std::invalid_argument(oss.str()));
		}

		this->inode_info_.set( this->path_ );
	}

	void add_subitem ( const filesystem_item& subitem )
	{
		//TODO: completar
	}

	void set_path ( const boost::filesystem::path& path )
	{
		this->path_ = path;

		if ( boost::filesystem::is_directory( this->path_ ) )
		{
			this->is_directory_ = true;
		}
	}

	boost::filesystem::path get_path ( ) const
	{
		return this->path_;
	}

public: //private:
	boost::filesystem::path path_;
	bool is_directory_;

	int file_descriptor_;
	//int watch_descriptor_; //TODO: analizar si es necesario

	struct kevent event_;		//TODO: creo que no es necesario

	boost::uint32_t mask_;

	//watch_type parent;
	filesystem_item* parent_; //TODO: cambiar a watch_type

	file_inode_info inode_info_;

	watch_collection_type subitems_;

	user_entry* root_user_entry_; //TODO: ver que pasa si agregamos el mismo directorio como dos user_entry distintos... el open da el mismo file descriptor?
};


struct user_entry
{
	//user_entry()
	//{
	//}

	~user_entry()
	{
		//std::cout << "--------------------- ~fsitem() ------------------------------" << std::endl;
		//std::cout << "this->path.native_file_string(): " << this->path.native_file_string() << std::endl;
	}

	//TODO: ver que sentido tiene este metodo...
	void add_watch( watch_type item )
	{
		all_watches_.push_back(item);
	}

	void initialize()
	{
		//TODO: estas dos instrucciones ponerlas en un factory
		watch_type item ( new filesystem_item(path_, this) );
		all_watches_.push_back(item);

		head_ = item;

		create_watch( item );

	}

	void create_watch ( watch_type watch )
	{
		//std::cout << "void create_watch( watch_type watch )" << std::endl;
		//std::cout << "watch->path.native_file_string(): " << watch->path.native_file_string() << std::endl;

		//TODO: ver esto...
		if ( watch->mask_ == 0 )
		{
			watch->mask_ = PN_ALL_EVENTS; //TODO: asignar lo que el usuario quiere monitorear...
		}

		struct kevent *event = &watch->event_;
		int mask = watch->mask_;

		////TODO: y esto?????? nunca se ejecuta, cual es la razon ????
		//if (watch->watch_descriptor_ < 0)
		//{
		//	std::ostringstream oss;
		//	oss << "Failed to monitor directory - Directory: " << watch->get_path().native_file_string() << " - Reason: " << std::strerror(errno);
		//	throw (std::invalid_argument(oss.str()));
		//}

		////if ( (watch->fd = open( watch->path.native_file_string().c_str(), O_RDONLY )) < 0)
		//watch->file_descriptor_ = open( watch->get_path().native_file_string().c_str(), O_EVTONLY );
		//if ( watch->file_descriptor_ == -1 ) //< 0
		//{
		//	//warn("opening path `%s' failed", watch->path);
		//	//return -1;

		//	std::ostringstream oss;
		//	//TODO:
		//	oss << "opening path failed: - Reason: " << std::strerror(errno);
		//	throw (std::invalid_argument(oss.str()));
		//}

		//watch->inode_info_.set( watch->get_path() );
		watch->open(); //TODO: catch errors

		if ( watch->is_directory_ )
		{
			scan_directory( watch.get() );
		}

		if ( watch->file_descriptor_ == 0 )
		{
			std::cout << "-------------------------------------------------------------------------------" << std::endl;
			std::cout << "watch->fd: " << watch->file_descriptor_ << std::endl;
			//std::cout << "watch->watch_descriptor_: " << watch->watch_descriptor_ << std::endl;
			std::cout << "watch->path.native_file_string(): " << watch->get_path().native_file_string() << std::endl;
			std::cout << "-------------------------------------------------------------------------------" << std::endl;
		}


		//TODO: ver estos flags, deberia monitoriarse solo lo que el usuairo quiera monitorear...
		//unsigned int fflags = NOTE_DELETE |  NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE; //| NOTE_RENAME;
		unsigned int fflags = NOTE_DELETE |  NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE | NOTE_RENAME;

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

		//TODO: no est� incluido en PN_ALL_EVENTS por eso lo agrego de una
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

	//TODO: contemplar la opcion include_sub_directories_
	void scan_directory( filesystem_item* head_dir )
	{
		std::cout << "void scan_directory( fsitem* head_dir )" << std::endl;
		std::cout << "head_dir->path.native_file_string(): " << head_dir->get_path().native_file_string() << std::endl;

		watch_collection_type temp_file_list;

		//TODO: STL --> std::transform o std::for_each o boost::lambda o BOOST_FOREACH
		//TODO: watch_collection_type o all_watches_type ?????? GUARDA!!!!
		for (watch_collection_type::iterator it =  head_dir->subitems_.begin(); it != head_dir->subitems_.end(); ++it )
		{
			(*it)->mask_ = PN_DELETE;  //TODO: recursivo
		}

		//		std::cout << "PN_DELETE: " << PN_DELETE << std::endl;
		//		std::cout << "PN_CREATE: " << PN_CREATE << std::endl;

		boost::filesystem::directory_iterator end_iter;
		for ( boost::filesystem::directory_iterator dir_itr( head_dir->get_path() ); dir_itr != end_iter; ++dir_itr )
		{
			try
			{
				//std::cout << "--- Finding File Name: " << dir_itr->path().native_file_string() << std::endl;
				bool found_filename = false;
				bool found_inode = false;

				file_inode_info inode_info( dir_itr->path() );

				//std::cout << "-----------------------------------------------------------------------" << std::endl;
				//std::cout << "dir_itr->path().native_file_string(): " << dir_itr->path().native_file_string() << std::endl;
				//std::cout << "dir_st.st_dev: " << dir_st.st_dev << std::endl;
				//std::cout << "dir_st.st_ino: " << dir_st.st_ino << std::endl;
				//std::cout << "-----------------------------------------------------------------------" << std::endl;


				//TODO: reemplazar por std::find o algo similar...
				//TODO: user_watchs o all_watchs ?????? GUARDA!!!!

				//Linear-search
				//TODO: all_watches_ ?????
				//for (watch_collection_type::iterator it =  head_dir->subitems.begin(); it != head_dir->subitems.end(); ++it )
				for (watch_collection_type::iterator it =  head_dir->subitems_.begin(); it != head_dir->subitems_.end(); ++it )
				{

					//std::cout << "-----------------------------------------------------------------------" << std::endl;
					//std::cout << "(*it)->path.native_file_string(): " << (*it)->path.native_file_string() << std::endl;
					//std::cout << "(*it)->inode_info_.device_id_: " << (*it)->inode_info_.device_id_ << std::endl;
					//std::cout << "(*it)->inode_info_.inode_number_: " << (*it)->inode_info_.inode_number_ << std::endl;
					//std::cout << "-----------------------------------------------------------------------" << std::endl;

					//if (  (*it)->inode_info_ == inode_info && (*it)->get_path().native_file_string() == dir_itr->path().native_file_string() )
					if (  (*it)->is_equal ( inode_info, dir_itr->path() ) )
					{
						//std::cout << "found inode & filename: " << (*it)->path.native_file_string() << std::endl;
						(*it)->mask_ = 0; //-999;
						//std::cout << "(*it)->path.native_file_string(): " << (*it)->path.native_file_string() << std::endl;
						found_filename = true;
						found_inode = true;
					}
					else
					{

						if (  (*it)->inode_info_ == inode_info  )
						{
							//std::cout << "found inode: " << (*it)->path.native_file_string() << std::endl;
							found_inode = true;
						}

						if ( (*it)->get_path() == dir_itr->path() )
						{
							//std::cout << "found filename: " << (*it)->path.native_file_string() << std::endl;
							found_filename = true;
						}
					}
				}


				if ( !found_filename && !found_inode )	//Archivo nuevo
				{
					std::cout << "if ( !found_filename && !found_inode )" << std::endl;


					//std::cout << "if (!found_filename && found_inode)" << std::endl;
					//std::cout << "dir_itr->path().native_file_string(): " << dir_itr->path().native_file_string() << std::endl;
					//std::cout << "dir_st.st_dev: " << dir_st.st_dev << std::endl;
					//std::cout << "dir_st.st_ino: " << dir_st.st_ino << std::endl;

					//TODO: usar algun metodo que lo haga facil.. add_subitem o algo asi, quizas desde una factory
					watch_type item ( new filesystem_item( dir_itr->path(), head_dir->root_user_entry_, head_dir) );
					this->all_watches_.push_back(item);

					create_watch( item );
					item->mask_ = PN_CREATE;
					item->inode_info_ = inode_info;

					head_dir->subitems_.push_back(item);

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
					//                  this->all_watches_.push_back(item);
					//					item->mask = PN_CREATE;
					//					item->parent_watch_descriptor_ = head_dir->watch_descriptor_;
					//					item->inode_info_.device_id_ = dir_st.st_dev;
					//					item->inode_info_.inode_number_ = dir_st.st_ino;
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

		for (watch_collection_type::iterator it =  head_dir->subitems_.begin(); it != head_dir->subitems_.end(); ++it )
		{
			if ( (*it)->mask_ != 0 ) //-999 )
			{
				bool found = false;
				for (watch_collection_type::iterator it2 =  temp_file_list.begin(); it2 != temp_file_list.end(); ++it2 )
				{
					if ( (*it2)->mask_ != 0 ) //-999 )
					{
						if (  (*it)->inode_info_ == (*it2)->inode_info_ )
						{
							//std::cout << "found inode: " << (*it)->path.native_file_string() << " - " << (*it2)->path.native_file_string() << std::endl;
							found = true;

							//TODO: volver a habilitar hasta el fin de las pruebas
							//std::cout << "File: " << (*it)->path.native_file_string() << " renamed to: " << (*it2)->path.native_file_string() << std::endl;

							(*it)->mask_ = PN_RENAME; //  -998;
							(*it2)->mask_ = 0; //-999;	//NO PROCESAR
							break;
						}

						if ( (*it)->get_path() == (*it2)->get_path() )
						{
							//TODO: que hacemos ac�, ver cuando podr�a a generarse este caso... Me parece que nunca.
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
		//			if ( (*it)->mask_ != 0 ) //-999 )
		//			{
		////				std::cout << "--- NEW FILE ---" << std::endl;
		////				std::cout << "(*it)->path.native_file_string(): " << (*it)->path.native_file_string() << std::endl;
		//
		//				watch_type item(new fsitem);
		//				item->path = (*it)->path;
		//
		//              this->all_watches_.push_back(item);
		//
		//				//TODO: volver a habilitar hasta el fin de las pruebas
		//				//std::cout << "File created: " << item->path.native_file_string() << std::endl;
		//
		//
		//				//TODO: ver en el codigo de pnotify: /* Add a watch if it is a regular file */
		//				create_watch( item );
		//				item->mask_ = PN_CREATE;
		//				item->parent_watch_descriptor_ = head_dir->watch_descriptor_;
		//				item->inode_info_.device_id_ = (*it)->inode_info_.device_id_;
		//				item->inode_info_.inode_number_ = (*it)->inode_info_.inode_number_;
		//
		//				head_dir->subitems.push_back(item);
		//			}
		//		}

		//std::cout << "DEBUG 5" << std::endl;


	}

	boost::filesystem::path path_;
	watch_type head_;						//este tiene la estructura de arbol
	watch_collection_type all_watches_;
};

typedef boost::shared_ptr<user_entry> user_item_pointer;
typedef std::vector<user_item_pointer> user_item_collection;



//TODO: si es necesario para todas las implementaciones, pasar a base_impl
typedef boost::shared_ptr<boost::thread> thread_type;



class freebsd_impl : public base_impl<freebsd_impl>
{
public:

	freebsd_impl()
		: is_initialized_(false), closing_(false) //, kqueue_file_descriptor_(0)
	{
		//kqueue_file_descriptor_ = 0;
	}

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
		//TODO: probar agregar el constructor a fsitem
		//watch_type item(new fs_item);
		//item->path = dir_name;		//boost::filesystem::path full_path( str_path, boost::filesystem::native );
		//TODO: asignar mask

		user_item_pointer item(new user_entry);
		item->path_ = dir_name; //TODO: revisar
		user_watches_.push_back(item);
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

	void start()
	{
		//std::cout << "void start()" << std::endl;
		initialize();

		//TODO: BOOST_FOREACH
		
		for (user_item_collection::iterator it = user_watches_.begin(); it != user_watches_.end(); ++it )
		{
			(*it)->initialize();
		}


		//for (watch_collection_type::iterator it =  user_watches_.begin(); it != user_watches_.end(); ++it )
		//{
		//	create_watch( *it );
		//}

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
				filesystem_item* watch = (filesystem_item*) event.udata; //TODO: reinterpret_cast<>

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
	//			std::cout << "watch->watch_descriptor_: " << watch->watch_descriptor_ << std::endl;
	//			std::cout << "watch->mask_: " << watch->mask_ << std::endl;
	//			std::cout << "watch->path.native_file_string(): " << watch->path.native_file_string() << std::endl;
	//			std::cout << "----------------------------------------------------------------------------" << std::endl;



				if ( event.fflags & NOTE_DELETE )
				{
					std::cout << "---------------------------------- NOTE_DELETE ---------------------------------------" << std::endl;

					std::cout << "watch: " << watch << std::endl;
					std::cout << "watch->fd: " << watch->file_descriptor_ << std::endl;
					//std::cout << "watch->watch_descriptor_: " << watch->watch_descriptor_ << std::endl;
					std::cout << "watch->parent: " << watch->parent_ << std::endl;
					std::cout << "watch->path: " << watch->get_path().native_file_string() << std::endl;
					std::cout << "watch->is_directory: " << watch->is_directory_ << std::endl;
					std::cout << "watch->mask_: " << watch->mask_ << std::endl;
					std::cout << "watch->inode_info_.device_id_: " << watch->inode_info_.device_id_ << std::endl;
					std::cout << "watch->inode_info_.inode_number_: " << watch->inode_info_.inode_number_ << std::endl;

					//std::cout << "event.ident: " << event.ident << std::endl;
					//std::cout << "event.filter: " << event.filter << std::endl;
					//std::cout << "event.flags: " << event.flags << std::endl;
					//std::cout << "event.fflags: " << event.fflags << std::endl;
					//std::cout << "event.data: " << event.data << std::endl;
					//std::cout << "event.udata: " << event.udata << std::endl;

				
					watch_collection_type::iterator it = watch->parent_->subitems_.begin();
					while ( it != watch->parent_->subitems_.end() )
					{

						//if ( watch == (*it) )
						if ( watch->is_equal( *it ) )
						{
							std::cout << "-----------------------------------------------------------------------" << std::endl;
							std::cout << "File removed: " << std::endl;
							std::cout << "(*it)->get_path().native_file_string(): " << (*it)->get_path().native_file_string() << std::endl;
							std::cout << "(*it)->inode_info_.device_id_: " << (*it)->inode_info_.device_id_ << std::endl;
							std::cout << "(*it)->inode_info_.inode_number_: " << (*it)->inode_info_.inode_number_ << std::endl;
							std::cout << "-----------------------------------------------------------------------" << std::endl;

							//it = head_dir->subitems.erase(it);
							break;
						}
						else
						{
							++it;
						}
					}

					std::cout << "--------------------------------------------------------------------------------------" << std::endl;
				}


				if ( event.fflags & NOTE_RENAME )
				{
					std::cout << "---------------------------------- NOTE_RENAME ---------------------------------------" << std::endl;

					std::cout << "watch: " << watch << std::endl;
					std::cout << "watch->fd: " << watch->file_descriptor_ << std::endl;
					std::cout << "watch->parent: " << watch->parent_ << std::endl;
					std::cout << "watch->path: " << watch->get_path().native_file_string() << std::endl;
					std::cout << "watch->is_directory: " << watch->is_directory_ << std::endl;
					std::cout << "watch->mask_: " << watch->mask_ << std::endl;
					std::cout << "watch->inode_info_.device_id_: " << watch->inode_info_.device_id_ << std::endl;
					std::cout << "watch->inode_info_.inode_number_: " << watch->inode_info_.inode_number_ << std::endl;

					//std::cout << "event.ident: " << event.ident << std::endl;
					//std::cout << "event.filter: " << event.filter << std::endl;
					//std::cout << "event.flags: " << event.flags << std::endl;
					//std::cout << "event.fflags: " << event.fflags << std::endl;
					//std::cout << "event.data: " << event.data << std::endl;
					//std::cout << "event.udata: " << event.udata << std::endl;

					boost::filesystem::path parent_path;

				
					////TODO: find
					//for (watch_collection_type::iterator it =  watch->root_user_entry->all_watches_.begin(); it != watch->root_user_entry->all_watches_.end(); ++it )
					//{
					//	if ( watch->parent_watch_descriptor_ == (*it)->watch_descriptor_ )
					//	{
					//		parent_path = (*it)->path;
					//	}
					//}

					//if ( watch->parent != 0 ) //TODO: cambiar cuando parent sea un shared_ptr
					//{
					//	parent_path = watch->parent->path;
					//}

					parent_path = watch->root_user_entry_->path_;

					if ( ! parent_path.empty() )
					{
						boost::filesystem::directory_iterator end_iter;

						//TODO: pasar a metodo estatico
						boost::filesystem::directory_iterator dir_itr( parent_path );
						for ( ; dir_itr != end_iter; ++dir_itr )
						{
							file_inode_info inode_info ( dir_itr->path() );
							if ( watch->inode_info_ == inode_info )
							{
								break;
							}


							//struct stat dir_st;
							//
							//int return_code = lstat( dir_itr->path().native_file_string().c_str(), &dir_st);
							//if ( return_code == -1)
							//{
							//	//TODO: manejo de errores
							//	std::cout << "STAT ERROR -- 4 -- - Reason: " << std::strerror(errno) << std::endl;
							//	std::cout << "dir_itr->path().native_file_string(): " << dir_itr->path().native_file_string() << std::endl;

							//	ptime now = microsec_clock::local_time();
							//	std::cout << to_iso_string(now) << std::endl;
							//}
							//else
							//{
							//	//if (  dir_st.st_dev == watch->inode_info_.device_id_ && dir_st.st_ino == watch->inode_info_.inode_number_ )
							//	if ( watch->inode_info_ == dir_st )
							//	{
							//		break;
							//	}
							//}
						}

						if ( dir_itr != end_iter )
						{
							std::cout << "Nuevo Nombre de Archivo:  " << dir_itr->path().native_file_string() << std::endl;
							//watch->path_ = dir_itr->path();
							watch->set_path( dir_itr->path() );

							//TODO: llamar a metodo para renombrar
						}
						else	
						{
							std::cout << "-----------------------------------------------------------------------" << std::endl;
							std::cout << "File removed: " << std::endl;
							std::cout << "watch->path: " << watch->get_path().native_file_string() << std::endl;
 							std::cout << "watch->inode_info_.device_id_: " << watch->inode_info_.device_id_ << std::endl;
							std::cout << "watch->inode_info_.inode_number_: " << watch->inode_info_.inode_number_ << std::endl;
							std::cout << "-----------------------------------------------------------------------" << std::endl;

							//TODO: llamar a metodo para eliminar

							//it = head_dir->subitems.erase(it);
							break;						
						}
					}

					std::cout << "--------------------------------------------------------------------------------------" << std::endl;

				}

				if ( event.fflags & NOTE_WRITE )
				{
					std::cout << "---------------------------------- NOTE_WRITE ---------------------------------------" << std::endl;
					
					std::cout << "watch: " << watch << std::endl;
					std::cout << "watch->fd: " << watch->file_descriptor_ << std::endl;
					std::cout << "watch->parent: " << watch->parent_ << std::endl;
					std::cout << "watch->path: " << watch->get_path().native_file_string() << std::endl;
					std::cout << "watch->is_directory: " << watch->is_directory_ << std::endl;
					std::cout << "watch->mask_: " << watch->mask_ << std::endl;
					std::cout << "watch->inode_info_.device_id_: " << watch->inode_info_.device_id_ << std::endl;
					std::cout << "watch->inode_info_.inode_number_: " << watch->inode_info_.inode_number_ << std::endl;


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
//					//TODO: buscar el directorio padre de la forma m�s eficiente posible a traves del parent_
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
				if ( watch->is_directory_ )
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
//				std::cout << "watch->watch_descriptor_: " << watch->watch_descriptor_ << std::endl;
//				std::cout << "watch->mask_: " << watch->mask_ << std::endl;
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
	//TODO: cambiarle el nombre porque un handler se confunde con un handler de un evento...
	void directory_event_handler( filesystem_item* head_dir ) 
	{
//		std::cout << "void directory_event_handler( fsitem* head_dir )" << std::endl;
//		std::cout << "head_dir->path.native_file_string(): " << head_dir->path.native_file_string() << std::endl;

		struct pnotify_event *ev;
		struct dentry *dptr, *dtmp;

		//TODO: ????
		//assert(ctl && watch);

		head_dir->root_user_entry_->scan_directory( head_dir );
		//scan_directory( head_dir );

//		//std::cout << "DEBUG 6" << std::endl;
//
//		watch_collection_type::iterator it = head_dir->subitems.begin();
//		while ( it != head_dir->subitems.end() )
//		{
//
//			//std::cout << "DEBUG 7" << std::endl;
//			//std::cout << "File removed: " << (*it)->path.native_file_string() << std::endl;
//
//			if ((*it)->mask_ == 0) /* Skip files that have not changed */
//			{
//				++it;
//				continue;
//			}
//
//			//std::cout << "DEBUG 8" << std::endl;
//
//
//			/* Remove the directory entry for a deleted file */
//			if ( (*it)->mask_ & PN_DELETE )
//			{
//				//std::cout << "DEBUG 9" << std::endl;
//
//
//				//std::cout << "File removed: " << (*it)->path.native_file_string() << std::endl;
//
////				std::cout << "ELIMINANDO ITEM DE LA LISTA" << std::endl;
////				std::cout << "(*it)->path.native_file_string(): " << (*it)->path.native_file_string() << std::endl;
////				//std::cout << "(*it)->mask_: " << (*it)->mask_ << std::endl;
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

	//TODO: las tres funciones siguientes estan duplicadas en windows_impl y freebsd_impl -> RESOLVER
	// inline void notify_file_system_event_args( int action, const std::string& directory, const std::string& name )

	thread_type thread_;

	bool is_initialized_;

	//int kqueue_file_descriptor_; // file descriptor

	bool closing_;
	user_item_collection user_watches_;
	//watch_collection_type all_watches_; //TODO: quizas haga falta contabilizar todos los watches en un solo lugar... VER
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP
