//TODO: ver: http://www.boost.org/doc/libs/1_42_0/libs/ptr_container/doc/examples.html
//TODO: ver Guidelines: http://www.boost.org/doc/libs/1_42_0/libs/ptr_container/doc/guidelines.html
//TODO: ver, leer: http://www.gotw.ca/publications/mill18.htm
//TODO: ver: http://www.boost.org/doc/libs/1_42_0/libs/smart_ptr/make_shared.html
//TODO: weak_ptr
//TODO: class invariants:  http://www.informit.com/articles/article.aspx?p=21084&seqNum=4
//                         http://en.wikipedia.org/wiki/Class_invariant

//TODO: ver shared_ptr: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2351.htm


//TODO: linked_ptr nuevo nombre para master_ptr y slave_ptr
//TODO: owner_ptr: nuevo SmartPtr en el cual se registre solo un unico owner_ptr por memoria... O sea, no podria haber dos owner_ptr apuntando a la misma posicion de memoria...
//TODO: analizar estos dos ultimos a ver si son viables con algun smart pointer actual.

//TODO: comentarios
//TODO: ver si hay "limit of file descriptors per process" Puede llegar a complicar el monitoreo usando kqueue
//TODO: ver que pasa con NetBSD, OpenBSD, Darwin, MacOSX, etc... aparentemente lo soportan
//TODO: ver de usar boost::noncopyable cuando sea aplicable



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

//TODO: agregar a todos los archivos fuente... quizas este freebsd y etc no, es al pedo
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

//TODO: ver cuales headers son innecesarios
#include <string>
//#include <vector>

// C-Std Headers
#include <cerrno>	//TODO: probar si es necesario
#include <cstdio>	//TODO: probar si es necesario
#include <cstdlib>	//TODO: probar si es necesario
#include <cstring>	// for strerror

#include <sys/event.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/filesystem/path.hpp>
//#include <boost/foreach.hpp>
//#include <boost/integer.hpp>
//#include <boost/smart_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <boost/thread.hpp>

#include <boost/os_services/change_types.hpp>
#include <boost/os_services/details/base_impl.hpp>
#include <boost/os_services/details/file_inode_info.hpp>
#include <boost/os_services/notify_filters.hpp>


//TODO: ver como arreglamos esto...
//#define EVENT_SIZE  ( sizeof (struct inotify_event) )
//#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )


//TODO: sacar, es solo para debug
#include <boost/date_time/posix_time/posix_time.hpp>
using namespace boost::posix_time;


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

//TODO: si es necesario para todas las implementaciones, pasar a base_impl
typedef boost::shared_ptr<boost::thread> thread_type;

//TODO: pasar a clase freebsd_impl
static int kqueue_file_descriptor_ = 0;
	
struct filesystem_item;		//forward-declaration
struct user_entry;			//forward-declaration


//TODO: no me gusta, ver si se puede agregar al forward declaration
//typedef boost::shared_ptr<user_entry> user_entry_pointer_type;
typedef user_entry* user_entry_pointer_type;
//typedef user_entry* user_entry::pointer_type; //TODO: ver si se puede hacer algo asi...

//TODO: renombrar
class filesystem_item
{
public:
	
	//typedef boost::shared_ptr<filesystem_item> pointer_type;
	//typedef std::vector<pointer_type> collection_type;

	typedef filesystem_item* pointer_type;
	typedef boost::ptr_vector<filesystem_item> collection_type;

	//filesystem_item( const boost::filesystem::path& path, const user_entry_pointer_type& root_user_entry )
	filesystem_item( const boost::filesystem::path& path, user_entry_pointer_type root_user_entry )
		: root_user_entry_(root_user_entry), parent_(0), is_directory_(false), file_descriptor_(0), mask_(PN_ALL_EVENTS) //TODO: asignar lo que el usuario quiere monitorear...
	{
		set_path( path );
	}

	//filesystem_item ( const boost::filesystem::path& path, const user_entry_pointer_type& root_user_entry, filesystem_item::pointer_type parent )
	//	: root_user_entry_(root_user_entry), parent_(parent), is_directory_(false), file_descriptor_(0), mask_(PN_ALL_EVENTS) //TODO: asignar lo que el usuario quiere monitorear...
	//filesystem_item ( const boost::filesystem::path& path, const user_entry_pointer_type& root_user_entry, filesystem_item* parent )


	filesystem_item ( const boost::filesystem::path& path, user_entry_pointer_type root_user_entry, filesystem_item::pointer_type parent )
		: root_user_entry_(root_user_entry), parent_(parent), is_directory_(false), file_descriptor_(0), mask_(PN_ALL_EVENTS) //TODO: asignar lo que el usuario quiere monitorear...
	{
		set_path( path );
	}

	~filesystem_item()
	{
		this->close( true, true ); //no-throw
	}

	//bool operator==(const fs_item& other) const
	//{
	//	return ( this->path_ == other.path_ && this->inode_info_ == other.inode_info_ );
	//}


	bool is_equal(const filesystem_item& other) const
	{
		return ( this->path_ == other.path_ && this->inode_info_ == other.inode_info_ );
	}

	//bool is_equal(const filesystem_item::pointer_type& other) const
	//bool is_equal(filesystem_item* other) const
	bool is_equal(filesystem_item::pointer_type other) const
	{
		return ( this->path_ == other->path_ && this->inode_info_ == other->inode_info_ );
	}

	bool is_equal(const file_inode_info& inode_info, const boost::filesystem::path& path) const
	{
		return (  this->inode_info_ == inode_info && this->path_ == path );
	}

	void open()
	{
		this->file_descriptor_ = ::open( path_.native_file_string().c_str(), O_EVTONLY );

		//std::cout << "this->file_descriptor_: " << this->file_descriptor_ << std::endl;
		//std::cout << "path_.native_file_string(): " << path_.native_file_string() << std::endl;

		if ( this->file_descriptor_ == -1 )
		{
			std::ostringstream oss;
			oss << "open failed - File: " << path_.native_file_string() << " - Reason: " << std::strerror(errno);
			throw (std::runtime_error(oss.str()));
			//throw (std::invalid_argument(oss.str()));
		}
		this->inode_info_.set( this->path_ );
	}

	void close( bool no_throw = true, bool close_subitems = true )
	{
		if ( this->file_descriptor_ != 0 )
		{
			if ( close_subitems )
			{
				for (filesystem_item::collection_type::iterator it =  this->subitems_.begin(); it != this->subitems_.end(); ++it )
				{
					it->close( no_throw, close_subitems );
				}
			}

			int ret_value = ::close( this->file_descriptor_ ); //close
			if ( ret_value == -1 )
			{
				if ( no_throw )
				{
					//Destructor -> no-throw
					std::cerr << "Failed to close file descriptor - File descriptor: '" << this->file_descriptor_ << "' - File path: '" << this->path_.native_file_string() << "' - Reason: " << std::strerror(errno) << std::endl;
				}
				else
				{
					std::ostringstream oss;
					oss << "Failed to close file descriptor - File descriptor: '" << this->file_descriptor_ << "' - File path: '" << this->path_.native_file_string() << "' - Reason: " << std::strerror(errno);
					throw (std::runtime_error(oss.str()));					
				}
			}
			this->file_descriptor_ = 0;
		}
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

	bool is_directory() const
	{
		return this->is_directory_;
	}

protected:
public: //TODO: sacar
	boost::filesystem::path path_;
	bool is_directory_;

public: //private:

	int file_descriptor_;
	//TODO: ver si es necesario
	boost::uint32_t mask_;

	//filesystem_item* parent_;
	filesystem_item::pointer_type parent_;

	file_inode_info inode_info_;
	//TODO: ver boost::ptr_vector
	collection_type subitems_;


	//TODO: ver que pasa si agregamos el mismo directorio como dos user_entry distintos... el open da el mismo file descriptor?
	//user_entry* root_user_entry_; 
	//user_entry::pointer_type root_user_entry_;
	user_entry_pointer_type root_user_entry_; 
};


struct user_entry : public enable_shared_from_this<user_entry>
{
	//typedef boost::shared_ptr<user_entry> pointer_type;
	//typedef std::vector<pointer_type> collection_type;

	typedef user_entry* pointer_type;
	typedef boost::ptr_vector<user_entry> collection_type;


	//user_entry()
	//{
	//}

	//~user_entry()
	//{
	//	//std::cout << "--------------------- ~fsitem() ------------------------------" << std::endl;
	//	//std::cout << "this->path.native_file_string(): " << this->path.native_file_string() << std::endl;
	//}

	//TODO: ver que sentido tiene este metodo...
	//void add_watch( filesystem_item* item )
	void add_watch( filesystem_item::pointer_type item )
	{
		//std::cout << "debug 1" << std::endl;
		all_watches_.push_back(item);
		//std::cout << "debug 2" << std::endl;
		//all_watches_.rele
	}

	void initialize()
	{
		//TODO: estas dos instrucciones ponerlas en un factory

		//filesystem_item::pointer_type item ( new filesystem_item (path_, this ) );
		//filesystem_item::pointer_type item ( new filesystem_item (path_, shared_from_this() ) );
		//filesystem_item::pointer_type item = new filesystem_item (path_, shared_from_this() );
		filesystem_item::pointer_type item = new filesystem_item (path_, this );

		//std::cout << "debug 3" << std::endl;
		all_watches_.push_back(item);
		root_ = item;
		//std::cout << "debug 4" << std::endl;

		create_watch( item, false );
	}

	
	//void create_watch( filesystem_item* watch )
	void create_watch( filesystem_item::pointer_type watch, bool launch_events = false )
	{
		//std::cout << "void create_watch( filesystem_item::pointer_type watch )" << std::endl;
		//std::cout << "watch->path.native_file_string(): " << watch->path.native_file_string() << std::endl;

		struct kevent event;

		//int mask = watch->mask_;

		watch->open(); //TODO: catch errors

		if ( watch->is_directory() )
		{
			//scan_directory( watch.get() );
			scan_directory( watch, launch_events );
		}

		//TODO: traducir de watch->mask_ a fflags
		//TODO: ver estos flags, deberia monitoriarse solo lo que el usuairo quiera monitorear...
		unsigned int fflags = NOTE_DELETE |  NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE | NOTE_RENAME;

		//std::cout << "0-------------------------------------------------------------" << std::endl;
		//std::cout << "&watch: " << &watch << std::endl;
		//std::cout << "watch.get(): " << watch.get() << std::endl;
		//std::cout << "watch->get_path().native_file_string(): " << watch->get_path().native_file_string() << std::endl;
		//std::cout << "watch->get_path().native_file_string(): " << watch->get_path().native_file_string() << std::endl;
		//std::cout << "0-------------------------------------------------------------" << std::endl;


		//EV_SET( &event, watch->file_descriptor_, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, fflags, 0, watch.get() ); // EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR
		EV_SET( &event, watch->file_descriptor_, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, fflags, 0, watch ); // EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR
		//EV_SET( &event, watch->file_descriptor_, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, fflags, 0, &watch ); // EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR

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

		int return_code = kevent(kqueue_file_descriptor_, &event, 1, NULL, 0, NULL);
		if ( return_code == -1 ) //< 0)
		{
			std::ostringstream oss;
			oss << "kevent error: - Reason: " << std::strerror(errno);
			throw (std::runtime_error(oss.str()));
		}
	}

	
	//TODO: contemplar la opcion include_sub_directories_
	//void scan_directory( filesystem_item* root_dir )
	void scan_directory( filesystem_item::pointer_type root_dir, bool launch_events = false )
	{
		//std::cout << "void scan_directory( fsitem* root_dir )" << std::endl;
		//std::cout << "root_dir->path.native_file_string(): " << root_dir->get_path().native_file_string() << std::endl;

		boost::filesystem::directory_iterator end_iter;
		for ( boost::filesystem::directory_iterator dir_itr( root_dir->get_path() ); dir_itr != end_iter; ++dir_itr )
		{
			try
			{
				bool found = false;

				file_inode_info inode_info( dir_itr->path() );

				//TODO: reemplazar por std::find o algo similar...
				//Linear-search
				//TODO: all_watches_ ?????
				//std::cout << "debug 11" << std::endl;
				for (filesystem_item::collection_type::iterator it =  root_dir->subitems_.begin(); it != root_dir->subitems_.end(); ++it )
				{
					//if (  (*it)->is_equal ( inode_info, dir_itr->path() ) )
					if (  it->is_equal ( inode_info, dir_itr->path() ) )
					{
						found = true;
						break;
					}
				}
				//std::cout << "debug 12" << std::endl;

				if ( !found )	//Archivo nuevo
				{
					if ( launch_events )
					{
						notify_file_system_event_args( change_types::created, dir_itr->path() );
					}

					//TODO: usar algun metodo que lo haga facil.. add_subitem o algo asi, quizas desde una factory
					//filesystem_item::pointer_type item ( new filesystem_item( dir_itr->path(), root_dir->root_user_entry_, root_dir) );
					filesystem_item::pointer_type item = new filesystem_item( dir_itr->path(), root_dir->root_user_entry_, root_dir );
					
					//std::cout << "debug 5" << std::endl;
					this->all_watches_.push_back(item);
					//std::cout << "debug 6" << std::endl;

					create_watch( item, launch_events );
					item->mask_ = PN_CREATE;
					item->inode_info_ = inode_info;
					//std::cout << "debug 7" << std::endl;
					root_dir->subitems_.push_back(item);
					//std::cout << "debug 8" << std::endl;
				}
			}
			catch ( const std::exception & ex )
			{
				//TODO: manejar esta excepcion
				std::cout << dir_itr->path().native_file_string() << " " << ex.what() << std::endl;
			}
		}
	}

	boost::filesystem::path path_;

	//TODO: estos deberian ser weak_ptr's quizas...
	filesystem_item::pointer_type root_;			//este tiene la estructura de arbol
	filesystem_item::collection_type all_watches_;
};






class freebsd_impl : public base_impl<freebsd_impl>
{
public:

	freebsd_impl()
		: is_initialized_(false), closing_(false) //, kqueue_file_descriptor_(0)
	{
		//kqueue_file_descriptor_ = 0;
	}

	~freebsd_impl()
	{
		//std::cout << "debug 200" << std::endl;
		closing_ = true;
		//std::cout << "debug 201" << std::endl;
		
		if ( thread_ )
		{
			//std::cout << "debug 202" << std::endl;
			thread_->join();
			//std::cout << "debug 203" << std::endl;
		}

		//std::cout << "debug 204" << std::endl;


		if ( kqueue_file_descriptor_ != 0 )
		{

			//std::cout << "debug 205" << std::endl;

			//TODO:
			//BOOST_FOREACH(pair_type p, watch_descriptors_)
			//{
			//	if ( p.second != 0 )
			//	{
			//		//int ret_value = ::inotify_rm_watch( kqueue_file_descriptor_, p.second );
			//		int ret_value = 0;

			//		if ( ret_value < 0 )
			//		{
			//			//TODO: analizar si esta es la forma optima de manejar errores.
			//			std::ostringstream oss;
			//			oss << "Failed to remove watch - Reason: "; //TODO: ver que usar en Linux/BSD << GetLastError();
			//			throw (std::runtime_error(oss.str()));
			//		}
			//	}
			//}

			//std::cout << "debug 206" << std::endl;

			int ret_value = ::close( kqueue_file_descriptor_ );
			//std::cout << "debug 207" << std::endl;
			if ( ret_value == -1 )
			{
				//Destructor -> no-throw
				std::cerr << "Failed to close kqueue file descriptor - File Descriptor: '" << kqueue_file_descriptor_ << "' - Reason: " << std::strerror(errno) << std::endl; 
			}
			kqueue_file_descriptor_ = 0;
		}

		//std::cout << "debug 210" << std::endl;


	}


	//TODO: agregar
	//void add_directory_impl ( const boost::filesystem::path& dir ) //throw (std::invalid_argument, std::runtime_error)

	void add_directory_impl ( const std::string& dir_name )
	{
		//TODO: asignar mask
		//user_entry::pointer_type item( new user_entry );
		user_entry::pointer_type item = new user_entry;

		item->path_ = dir_name; //TODO: revisar
		//std::cout << "debug 9" << std::endl;
		user_watches_.push_back(item);
		//std::cout << "debug 10" << std::endl;
	}
	//void remove_directory_impl(const std::string& dir_name) // throw (std::invalid_argument);

	//TODO: ver si hace falta hacer lo mismo para Windows
	void initialize() //TODO: private
	{
		if (!is_initialized_)
		{
			kqueue_file_descriptor_ = kqueue(); //::kqueue();
			//std::cout << "kqueue_file_descriptor_" << kqueue_file_descriptor_ << std::endl;

			if ( kqueue_file_descriptor_ == -1 )   //< 0)
			{
				std::ostringstream oss;
				oss << "Failed to initialize kqueue - Reason: " << std::strerror(errno);
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
		//TODO: STL transform
		//std::cout << "debug 30" << std::endl;
		for (user_entry::collection_type::iterator it = user_watches_.begin(); it != user_watches_.end(); ++it )
		{
			//std::cout << "debug 30.A" << std::endl;
			//(*it)->initialize();
			it->initialize();
			//std::cout << "debug 30.B" << std::endl;
		}
		//std::cout << "debug 31" << std::endl;

		thread_.reset( new boost::thread( boost::bind(&freebsd_impl::handle_directory_changes, this) ) );
	}

public: //private:  //TODO:

	//TODO: evaluar si rename_watch y remove_watch tienen que ir acá o en sus respectivas clases
	//void rename_watch ( filesystem_item* watch, const boost::filesystem::path& new_path ) 
	void rename_watch ( filesystem_item::pointer_type watch, const boost::filesystem::path& new_path ) 
	{
		watch->set_path( new_path );
	}

	
	//void remove_watch ( filesystem_item* watch ) 
	void remove_watch ( filesystem_item::pointer_type watch ) 
	{
		//std::cout << "debug 40" << std::endl;
		filesystem_item::collection_type::iterator it = watch->parent_->subitems_.begin();
		while ( it != watch->parent_->subitems_.end() )
		{
			//if ( watch == (*it) )
			if ( watch->is_equal( *it ) )
			{
				it = watch->parent_->subitems_.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}
		
		//std::cout << "debug 20" << std::endl;
		it = watch->root_user_entry_->all_watches_.begin();
		while ( it != watch->parent_->subitems_.end() )
		{
			if ( watch->is_equal( *it ) )
			{
				it = watch->root_user_entry_->all_watches_.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}
		//std::cout << "debug 21" << std::endl;
		//TODO: llamar a metodo que lanza el evento...
	}

	//void handle_rename( filesystem_item* watch )
	void handle_rename( filesystem_item::pointer_type watch )
	{
		boost::filesystem::path parent_path;

		parent_path = watch->root_user_entry_->path_;

		if ( ! parent_path.empty() )
		{
			//TODO: pasar a metodo estatico
			//TODO: STL find o similar...
			boost::filesystem::directory_iterator end_iter;
			boost::filesystem::directory_iterator dir_itr( parent_path );
			for ( ; dir_itr != end_iter; ++dir_itr )
			{
				file_inode_info inode_info ( dir_itr->path() ); //TODO: puede arrojar una excepcion
				if ( watch->inode_info_ == inode_info )
				{
					break;
				}
				//TODO: que pasa si se encuentran más de un archivo con el mismo inodo ?????
				//      quiere decir que hay un symlink o un hardlink... ver como manejar eso...
			}

			if ( dir_itr != end_iter )
			{
				notify_rename_event_args ( change_types::renamed, dir_itr->path(), watch->get_path() );
				rename_watch(watch, dir_itr->path());
			}
			else	
			{
				//TODO: esto es un remove o un rename ?????
				//notify_rename_event_args ( change_types::renamed, new_path, watch->get_path() );
				handle_remove( watch );
			}
		}
	}

	//void handle_remove( filesystem_item* watch )
	void handle_remove( filesystem_item::pointer_type watch )
	{
		notify_file_system_event_args( change_types::deleted, watch->get_path() );
		remove_watch ( watch );
	}

	
	//void handle_write( filesystem_item* watch )
	void handle_write( filesystem_item::pointer_type watch )
	{
		if ( watch->is_directory() )
		{
			//TODO: no está buena esta llamada... no me convence...
			watch->root_user_entry_->scan_directory( watch, true ); //Detectamos si es un Add o un Rename o Delete que ya fue procesado.
		}
		else
		{
			notify_file_system_event_args( change_types::changed, watch->get_path() );
		}
	}

	void handle_directory_changes()
	{
		//std::cout << "debug 100" << std::endl;

		filesystem_item::pointer_type queued_write_watch = 0;

		//std::cout << "debug 101" << std::endl;

		while ( ! closing_ )
		{
			struct kevent event;

			struct timespec timeout;
			timeout.tv_sec = 0;
			timeout.tv_nsec = 300000; //300 milliseconds //TODO: sacar el hardcode, hacer configurable...

			//std::cout << "debug 102" << std::endl;

			//TODO: pasar toda esta logica a un metodo o clase...
			int return_code = kevent ( kqueue_file_descriptor_, NULL, 0, &event, 1, &timeout );

			//std::cout << "debug 103" << std::endl;


			if ( return_code == -1 || event.flags & EV_ERROR) //< 0
			{
				//TODO: evaluar si este throw está relacionado con el destructor de fsm ya que está ejecutado en otro thread, no deberia... pero...
				std::ostringstream oss;
				oss << "kevent error - Reason: " << std::strerror(errno);
				throw (std::runtime_error(oss.str()));
			}

			//std::cout << "debug 104" << std::endl;
			//std::cout << "closing_: " << closing_ << std::endl;
			

			if ( ! closing_ )
			{
				//std::cout << "debug 105" << std::endl;

				if ( return_code == 0 ) //timeout
				{
					//std::cout << "debug 106" << std::endl;

					//if ( queued_write_watch  )
					if ( queued_write_watch != 0 )
					{
						//std::cout << "debug 107" << std::endl;

						handle_write( queued_write_watch );
						//std::cout << "debug 108" << std::endl;

						queued_write_watch = 0;
						//queued_write_watch.reset();
					}
				}
				else
				{
					//std::cout << "debug 109" << std::endl;

					//filesystem_item* watch = (filesystem_item*) event.udata; //TODO: reinterpret_cast<>
					//filesystem_item* watch = reinterpret_cast<filesystem_item*>( event.udata );
					filesystem_item::pointer_type watch = reinterpret_cast<filesystem_item::pointer_type>( event.udata );

					//std::cout << "debug 110" << std::endl;


					//filesystem_item::pointer_type watch = *(reinterpret_cast<filesystem_item::pointer_type*>( event.udata ));
					//filesystem_item::pointer_type* watch_temp_1 = reinterpret_cast<filesystem_item::pointer_type*>( event.udata );
					//void* watch_temp_2 =  event.udata ;


					if ( event.fflags & NOTE_DELETE )
					{
						//std::cout << "debug 111" << std::endl;
						handle_remove( watch );
					}

					if ( event.fflags & NOTE_RENAME )
					{
						//std::cout << "debug 112" << std::endl;
						handle_rename( watch );
					}

					if ( event.fflags & NOTE_WRITE )
					{
						//std::cout << "debug 113" << std::endl;
						//if ( queued_write_watch  )
						if ( queued_write_watch != 0 )
						{
							//std::cout << "debug 114" << std::endl;
							handle_write( queued_write_watch );
							//std::cout << "debug 115" << std::endl;
							queued_write_watch = 0;
							//queued_write_watch.reset();
						}

						//Encolamos un solo evento WRITE ya que siempre viene WRITE+RENAME... hacemos que primero se procese el evento rename y luego el write
						queued_write_watch = watch;
					}

					//std::cout << "debug 116" << std::endl;




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
					//if (event.fflags & NOTE_REVOKE)
					//{
					//	std::cout << "NOTE_REVOKE -> XXXXXXXXX" << std::endl;
					//}
					//if (event.fflags & NOTE_LINK)
					//{
					//	std::cout << "NOTE_LINK -> XXXXXXXXX" << std::endl;
					//}
				}
			}
		}
		//std::cout << "debug 117" << std::endl;
	}

protected:

	//inline void notify_file_system_event_args( int action, const std::string& directory, const std::string& name )

	inline void notify_file_system_event_args( int action, const boost::filesystem::path& path )
	{
		//TODO: ver en .Net
		//if (!MatchPattern(name))
		//{
		//	return;
		//}

		if ( action == change_types::created )
		{
			do_callback(created_handler_, filesystem_event_args(change_types::created, path));
		}
		else if ( action == change_types::deleted )
		{
			do_callback(deleted_handler_, filesystem_event_args(change_types::deleted, path));
		}
		else if ( action == change_types::changed )
		{
			do_callback(changed_handler_, filesystem_event_args(change_types::changed, path));
		}
		else
		{
			//TODO:
			//Debug.Fail("Unknown FileSystemEvent action type!  Value: " + action);
		}
	}

	inline void notify_rename_event_args(int action, const boost::filesystem::path& path, const boost::filesystem::path& old_path)
	{
		//filter if neither new name or old name are a match a specified pattern

		//TODO:
		//if (!MatchPattern(name) && !MatchPattern(oldName))
		//{
		//	return;
		//}

		do_callback(renamed_handler_, renamed_event_args(action, path, old_path));
	}




protected:

	//TODO: las tres funciones siguientes estan duplicadas en windows_impl y freebsd_impl -> RESOLVER

	thread_type thread_;
	bool is_initialized_;
	//int kqueue_file_descriptor_; // file descriptor
	bool closing_;
	user_entry::collection_type user_watches_;
	//filesystem_item::collection_type all_watches_; //TODO: quizas haga falta contabilizar todos los watches en un solo lugar... VER

};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP
