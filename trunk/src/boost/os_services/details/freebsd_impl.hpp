//TODO: ver: http://www.boost.org/doc/libs/1_42_0/libs/ptr_container/doc/examples.html
//TODO: ver Guidelines: http://www.boost.org/doc/libs/1_42_0/libs/ptr_container/doc/guidelines.html
//TODO: ver, leer: http://www.gotw.ca/publications/mill18.htm
//TODO: ver: http://www.boost.org/doc/libs/1_42_0/libs/smart_ptr/make_shared.html
//TODO: weak_ptr
//TODO: class invariants:  http://www.informit.com/articles/article.aspx?p=21084&seqNum=4
//                         http://en.wikipedia.org/wiki/Class_invariant

//TODO: ver shared_ptr: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2351.htm

//TODO: http://www.boost.org/doc/libs/1_40_0/libs/smart_ptr/sp_techniques.html

//TODO: linked_ptr nuevo nombre para master_ptr y slave_ptr
//TODO: owner_ptr: nuevo SmartPtr en el cual se registre solo un unico owner_ptr por memoria... O sea, no podria haber dos owner_ptr apuntando a la misma posicion de memoria...
//TODO: analizar estos dos ultimos a ver si son viables con algun smart pointer actual.

//TODO: comentarios
//TODO: ver si hay "limit of file descriptors per process" Puede llegar a complicar el monitoreo usando kqueue
//TODO: ver que pasa con NetBSD, OpenBSD, Darwin, MacOSX, etc... aparentemente lo soportan
//TODO: ver de usar boost::noncopyable cuando sea aplicable

//TODO: ver que pasa cuando se crea, renombra o elimina un hardlink o softlink
//TODO: ver que pasa cuando se crea, renombra o elimina un archivo existiendo hardlink (o softlink) a este archivo. El sistema puede confundirse.

//TODO: problema en runtime: Parece que si los eventos ocurren muy rapido no son capturados, se pierden eventos. No se como funciona el cache del kevent... investigar eso...
//      sino implementar una cola de mensajes sincronizada...



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
#include <vector>

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
#include <boost/function.hpp>
//#include <boost/integer.hpp>
//#include <boost/ptr_container/ptr_vector.hpp>
//#include <boost/smart_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>


#include <boost/os_services/change_types.hpp>
#include <boost/os_services/details/base_impl.hpp>
#include <boost/os_services/details/file_inode_info.hpp>
#include <boost/os_services/details/filesystem_item.hpp>
#include <boost/os_services/details/user_entry.hpp>
#include <boost/os_services/notify_filters.hpp>


//TODO: ver como arreglamos esto...
//#define EVENT_SIZE  ( sizeof (struct inotify_event) )
//#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )


//TODO: sacar, es solo para debug
//#include <boost/date_time/posix_time/posix_time.hpp>
//using namespace boost::posix_time;




//TODO: sacar
/* kqueue(4) in MacOS/X does not support NOTE_TRUNCATE */
#ifndef NOTE_TRUNCATE
# define NOTE_TRUNCATE 0
#endif




namespace boost {
namespace os_services {
namespace detail {

//TODO: si es necesario para todas las implementaciones, pasar a base_impl
typedef boost::shared_ptr<boost::thread> thread_type;

	
//struct filesystem_item;		//forward-declaration
//struct user_entry;			//forward-declaration
//
//
////TODO: no me gusta, ver si se puede agregar al forward declaration
////typedef boost::shared_ptr<user_entry> user_entry_pointer_type;
//typedef user_entry* user_entry_pointer_type;
////typedef user_entry* user_entry::pointer_type; //TODO: ver si se puede hacer algo asi...



struct null_deleter
{
	void operator()(void const *) const
	{
	}
};


class freebsd_impl : public base_impl<freebsd_impl>
{
public:

	freebsd_impl()
		: is_initialized_(false), closing_(false), kqueue_file_descriptor_(0)
	{}

	~freebsd_impl()
	{
		std::cout << "debug ~freebsd_impl() - 1" << std::endl;
		closing_ = true;

		//TODO: cerrar los archivos /watches

		if ( kqueue_file_descriptor_ != 0 )
		{
			std::cout << "debug ~freebsd_impl() - 2" << std::endl;
			int ret_value = ::close( kqueue_file_descriptor_ );
			if ( ret_value == -1 )
			{
				//Destructor -> no-throw
				std::cerr << "Failed to close kqueue file descriptor - File Descriptor: '" << kqueue_file_descriptor_ << "' - Reason: " << std::strerror(errno) << std::endl; 
			}
			std::cout << "debug ~freebsd_impl() - 3" << std::endl;
			kqueue_file_descriptor_ = 0;
		}


		std::cout << "debug ~freebsd_impl() - 4" << std::endl;
		if ( thread_ )
		{
			std::cout << "debug ~freebsd_impl() - 5" << std::endl;
			thread_->join();
			std::cout << "debug ~freebsd_impl() - 6" << std::endl;
		}
		std::cout << "debug ~freebsd_impl() - 7" << std::endl;

		std::cout << "debug ~freebsd_impl() - 8" << std::endl;
	}


	//TODO: agregar
	//void add_directory_impl ( const boost::filesystem::path& dir ) //throw (std::invalid_argument, std::runtime_error)

	void add_directory_impl ( const std::string& dir_name )
	{
		//TODO: asignar mask
		//TODO: crear un factory o un allocator para poder crear un objeto del tipo user_entry usando shared_ptr o un raw-C-pointer y crearlo de cualquier forma.
		//user_entry::pointer_type item = new user_entry( dir_name );
		user_entry::pointer_type item ( new user_entry( dir_name ) );
		user_watches_.push_back(item);
	}
	//void remove_directory_impl(const std::string& dir_name) // throw (std::invalid_argument);

	//TODO: ver si hace falta hacer lo mismo para Windows
	void initialize() //TODO: protected
	{
		if ( ! is_initialized_ )
		{
			kqueue_file_descriptor_ = kqueue(); //::kqueue();

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
		this->initialize();

		//TODO: BOOST_FOREACH
		//TODO: STL transform
		for (user_entry::collection_type::iterator it = user_watches_.begin(); it != user_watches_.end(); ++it )
		{
			//it->initialize();
			filesystem_item::pointer_type watch = create_filesystem_item ( (*it)->path(), *it );
			begin_watch( watch, false );
		}

		thread_.reset( new boost::thread( boost::bind(&freebsd_impl::handle_directory_changes, this) ) );
	}

public: //private:  //TODO:


	//filesystem_item::pointer_type create_filesystem_item ( const boost::filesystem::path& path, user_entry& entry )
	filesystem_item::pointer_type create_filesystem_item ( const boost::filesystem::path& path, user_entry::pointer_type& entry )
	{
		filesystem_item::pointer_type watch = new filesystem_item ( path, entry ); 
		entry->set_root ( watch );
		entry->add_watch ( watch );

		//TODO: ver de esto que va...
		//watch->open(); //TODO: catch errors
		//begin_watch( watch, false );
		//begin_watch( watch, launch_events );
		//item->inode_info_ = inode_info;
		//item->mask_ = PN_CREATE;

		return watch;
	}

	filesystem_item::pointer_type create_filesystem_item ( const boost::filesystem::path& path, user_entry::pointer_type& entry, filesystem_item::pointer_type parent )
	{
		filesystem_item::pointer_type watch = create_filesystem_item( path, entry );

		if ( parent )
		{
			//TODO: agregar metodo add_subitem a filesystem_item
			parent->subitems_.push_back(watch);
		}

		return watch;
	}


	filesystem_item::pointer_type create_filesystem_item ( void* raw_pointer )
	{
		filesystem_item::pointer_type px( reinterpret_cast<filesystem_item*>( raw_pointer ), null_deleter() );
		return px;
	}



	void begin_watch( filesystem_item::pointer_type watch, bool launch_events = false )
	{
		//std::cout << "void begin_watch( filesystem_item::pointer_type watch )" << std::endl;
		//std::cout << "watch->path.native_file_string(): " << watch->path.native_file_string() << std::endl;

		struct kevent event;

		//int mask = watch->mask_;
		watch->open(); //TODO: catch errors

		if ( watch->is_directory() )
		{
			scan_directory( watch, launch_events );
		}

		//TODO: traducir de watch->mask_ a fflags
		//TODO: ver estos flags, deberia monitoriarse solo lo que el usuairo quiera monitorear...
		unsigned int fflags = NOTE_DELETE |  NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_REVOKE | NOTE_RENAME;

		//EV_SET( &event, watch->file_descriptor_, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, fflags, 0, watch ); // EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR
		EV_SET( &event, watch->file_descriptor_, EVFILT_VNODE, EV_ADD | EV_ENABLE | EV_CLEAR, fflags, 0, watch.get() ); // EV_ADD | EV_ENABLE | EV_ONESHOT | EV_CLEAR

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

		int return_code = kevent( kqueue_file_descriptor_, &event, 1, NULL, 0, NULL );
		if ( return_code == -1 ) //< 0)
		{
			std::ostringstream oss;
			oss << "kevent error: - Reason: " << std::strerror(errno);
			throw (std::runtime_error(oss.str()));
		}
	}


	//TODO: contemplar la opcion include_sub_directories_
	void scan_directory( filesystem_item::pointer_type root_dir, bool launch_events = false )
	{
		//std::cout << "void scan_directory( fsitem* root_dir )" << std::endl;
		//std::cout << "root_dir->path.native_file_string(): " << root_dir->get_path().native_file_string() << std::endl;

		boost::filesystem::directory_iterator end_iter;
		for ( boost::filesystem::directory_iterator dir_itr( root_dir->path() ); dir_itr != end_iter; ++dir_itr )
		{
			try
			{
				file_inode_info inode_info( dir_itr->path() );

				//TODO: reemplazar por std::find o algo similar...
				//Linear-search
				//TODO: all_watches_ ?????

				bool found = false;
				for (filesystem_item::collection_type::iterator it = root_dir->subitems_.begin(); it != root_dir->subitems_.end(); ++it )
				{
					if (  (*it)->is_equal ( inode_info, dir_itr->path() ) )
					{
						found = true;
						break;
					}
				}

				if ( !found ) //new file
				{
					//std::cout << "launch_events: " << launch_events << std::endl;

					if ( launch_events )
					{
						notify_file_system_event_args( change_types::created, dir_itr->path() );
					}

					filesystem_item::pointer_type watch = create_filesystem_item ( dir_itr->path(), root_dir->root_user_entry_, root_dir );
					begin_watch( watch, launch_events );
				}
			}
			catch ( const std::exception & ex )
			{
				//TODO: manejar esta excepcion
				std::cout << dir_itr->path().native_file_string() << " " << ex.what() << std::endl;
			}
		}
	}



	//TODO: evaluar si rename_watch y remove_watch tienen que ir acá o en sus respectivas clases
	//void rename_watch ( filesystem_item* watch, const boost::filesystem::path& new_path ) 
	void rename_watch ( filesystem_item::pointer_type watch, const boost::filesystem::path& new_path ) 
	{
		watch->set_path( new_path );
	}

	
	void remove_watch ( filesystem_item::pointer_type watch ) 
	{
		watch->parent_->remove_subitem( watch );
		watch->root_user_entry_->remove_watch( watch );
		//TODO: llamar a metodo que lanza el evento...
	}

	//void handle_rename( filesystem_item* watch )
	void handle_rename( filesystem_item::pointer_type watch )
	{
		boost::filesystem::path parent_path;

		parent_path = watch->root_user_entry_->path();

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
				notify_rename_event_args ( change_types::renamed, dir_itr->path(), watch->path() );
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
		notify_file_system_event_args( change_types::deleted, watch->path() );
		remove_watch ( watch );
	}

	
	//void handle_write( filesystem_item* watch )
	void handle_write( filesystem_item::pointer_type watch )
	{
		if ( watch->is_directory() )
		{
			//TODO: no está buena esta llamada... no me convence...
			//watch->root_user_entry_->scan_directory( watch, true ); //Detectamos si es un Add o un Rename o Delete que ya fue procesado.
			scan_directory( watch, true ); //Detectamos si es un Add o un Rename o Delete que ya fue procesado.
		}
		else
		{
			notify_file_system_event_args( change_types::changed, watch->path() );
		}
	}

	void handle_directory_changes()
	{

		//filesystem_item::pointer_type queued_write_watch = 0;
		filesystem_item::pointer_type queued_write_watch;

		while ( ! closing_ )
		{
			struct kevent event;

			struct timespec timeout;
			timeout.tv_sec = 0;
			timeout.tv_nsec = 300000; //300 milliseconds //TODO: sacar el hardcode, hacer configurable...

			//TODO: pasar toda esta logica a un metodo o clase...
			int return_code = kevent ( kqueue_file_descriptor_, NULL, 0, &event, 1, &timeout );



			if ( return_code == -1 || event.flags & EV_ERROR) //< 0
			{
				//TODO: evaluar si este throw está relacionado con el destructor de fsm ya que está ejecutado en otro thread, no deberia... pero...
				std::ostringstream oss;
				oss << "kevent error - Reason: " << std::strerror(errno);
				throw (std::runtime_error(oss.str()));
			}


			if ( ! closing_ )
			{
				if ( return_code == 0 ) //timeout
				{
					if ( queued_write_watch ) // != 0
					{
						handle_write( queued_write_watch );
						queued_write_watch.reset(); // = 0;
					}
				}
				else
				{
					//filesystem_item::pointer_type watch = reinterpret_cast<filesystem_item::pointer_type>( event.udata );
					filesystem_item::pointer_type watch = create_filesystem_item( event.udata );
					//TODO: watch se puede convertir en un shared_ptr usando un null_deleter: http://www.boost.org/doc/libs/1_40_0/libs/smart_ptr/sp_techniques.html#static


					if ( event.fflags & NOTE_DELETE )
					{
						handle_remove( watch );
						queued_write_watch = 0;
					}

					if ( event.fflags & NOTE_RENAME )
					{
						handle_rename( watch );
						queued_write_watch = 0;
					}

					if ( event.fflags & NOTE_WRITE )
					{
						if ( queued_write_watch ) //!= 0
						{
							handle_write( queued_write_watch );
							queued_write_watch.reset(); // = 0;
						}

						//Encolamos un solo evento WRITE ya que siempre viene WRITE+RENAME... hacemos que primero se procese el evento rename y luego el write
						queued_write_watch = watch;
					}

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
	int kqueue_file_descriptor_; // file descriptor
	bool closing_;
	user_entry::collection_type user_watches_;
	//filesystem_item::collection_type all_watches_; //TODO: quizas haga falta contabilizar todos los watches en un solo lugar... VER

};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_FREEBSD_IMPL_HPP
