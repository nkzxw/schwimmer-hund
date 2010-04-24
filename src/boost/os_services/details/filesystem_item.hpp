#ifndef BOOST_OS_SERVICES_DETAIL_FILESYSTEM_ITEM_HPP
#define BOOST_OS_SERVICES_DETAIL_FILESYSTEM_ITEM_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

//#include <iostream>

//TODO: revisar los headers

//// C-Std headers
//#include <cerrno>	//TODO: probar si es necesario
//#include <cstdio>	//TODO: probar si es necesario
//#include <cstdlib>	//TODO: probar si es necesario
//#include <cstring>	// for strerror

// POSIX headers
//#include <sys/event.h>
//#include <sys/fcntl.h>
//#include <sys/time.h>
//#include <sys/types.h>
//#include <unistd.h>

#include <boost/filesystem/path.hpp>

#include <boost/os_services/details/file_inode_info.hpp>
#include <boost/os_services/posix_syscall_wrapper.hpp>


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




namespace boost {
namespace os_services {
namespace detail {

struct user_entry;			//forward-declaration

//TODO: no me gusta, ver si se puede agregar al forward declaration
typedef boost::shared_ptr<user_entry> user_entry_pointer_type;

//TODO: renombrar
class filesystem_item
{
public:

	typedef boost::shared_ptr<filesystem_item> pointer_type;
	typedef std::vector<pointer_type> collection_type;


	static filesystem_item::pointer_type create( const boost::filesystem::path& path, const user_entry_pointer_type& root_user_entry )
	{
		filesystem_item::pointer_type watch ( new filesystem_item ( path, root_user_entry ) );
		return watch;
	}

	static filesystem_item::pointer_type create( const boost::filesystem::path& path, const user_entry_pointer_type& root_user_entry, const filesystem_item::pointer_type& parent )
	{
		filesystem_item::pointer_type watch ( new filesystem_item ( path, root_user_entry, parent ) );
		return watch;
	}

	~filesystem_item()
	{
		std::cout << "~filesystem_item()" << std::endl;
		std::cout << "~filesystem_item() - " << path_.native_file_string() << std::endl;
		this->close( true, true ); //no-throw
	}

	//TODO: habilitar
	//bool operator==(const fs_item& other) const
	//{
	//	return ( this->path_ == other.path_ && this->inode_info_ == other.inode_info_ );
	//}

	bool is_equal( const filesystem_item& other ) const
	{
		return ( this->path_ == other.path_ && this->inode_info_ == other.inode_info_ );
	}

	bool is_equal( const filesystem_item::pointer_type& other) const
	{
		return ( this->path_ == other->path_ && this->inode_info_ == other->inode_info_ );
	}

	bool is_equal(const file_inode_info& inode_info, const boost::filesystem::path& path) const
	{
		return (  this->inode_info_ == inode_info && this->path_ == path );
	}

	void open()
	{
		//this->file_descriptor_ = ::open( path_.native_file_string().c_str(), O_EVTONLY );

		////std::cout << "this->file_descriptor_: " << this->file_descriptor_ << std::endl;
		////std::cout << "path_.native_file_string(): " << path_.native_file_string() << std::endl;

		//if ( this->file_descriptor_ == -1 )
		//{
		//	std::ostringstream oss;
		//	oss << "open failed - File: " << path_.native_file_string() << " - Reason: " << std::strerror(errno);
		//	throw (std::runtime_error(oss.str()));
		//	//throw (std::invalid_argument(oss.str()));
		//}

		this->file_descriptor_ = boost::os_services::posix_syscall_wrapper::open_file( path_ );
		this->inode_info_.set( this->path_ );
	}

	//TODO: protected o no.... ????? ver!
	void close( bool no_throw = false, bool close_subitems = true )
	{
		//std::cout << "void close( bool no_throw = false, bool close_subitems = true )" << std::endl;

		//if ( this->file_descriptor_ != 0 )
		//{
		//	if ( close_subitems )
		//	{
		//		for (filesystem_item::collection_type::iterator it =  this->subitems_.begin(); it != this->subitems_.end(); ++it )
		//		{
		//			(*it)->close( no_throw, close_subitems );
		//		}
		//	}

		//	int ret_value = ::close( this->file_descriptor_ ); //close
		//	if ( ret_value == -1 )
		//	{
		//		if ( no_throw )
		//		{
		//			//Destructor -> no-throw
		//			std::cerr << "Failed to close file descriptor - File descriptor: '" << this->file_descriptor_ << "' - File path: '" << this->path_.native_file_string() << "' - Reason: " << std::strerror(errno) << std::endl;
		//		}
		//		else
		//		{
		//			std::ostringstream oss;
		//			oss << "Failed to close file descriptor - File descriptor: '" << this->file_descriptor_ << "' - File path: '" << this->path_.native_file_string() << "' - Reason: " << std::strerror(errno);
		//			throw (std::runtime_error(oss.str()));					
		//		}
		//	}
		//	this->file_descriptor_ = 0;
		//}


		if ( this->file_descriptor_ != 0 )
		{
			if ( close_subitems )
			{
				for (filesystem_item::collection_type::iterator it =  this->subitems_.begin(); it != this->subitems_.end(); ++it )
				{
					(*it)->close( no_throw, close_subitems );
				}
			}

			boost::os_services::posix_syscall_wrapper::close_file( this->file_descriptor_ );

			this->file_descriptor_ = 0;
		}
	}

	void add_subitem ( const filesystem_item::pointer_type& subitem )
	{
		//TODO: ver si se necesita completar algo...
		subitems_.push_back( subitem );
	}

	void remove_subitem( const filesystem_item::pointer_type& watch )
	{
		filesystem_item::collection_type::iterator it = subitems_.begin();
		while ( it != subitems_.end() )
		{
			if ( watch->is_equal( *it ) )
			{
				it = subitems_.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}
	}

	void set_path ( const boost::filesystem::path& path )
	{
		this->path_ = path;

		if ( boost::filesystem::is_directory( this->path_ ) )
		{
			this->is_directory_ = true;
		}
	}

	void set_parent ( const filesystem_item::pointer_type& new_parent )
	{
		this->parent_ = new_parent;
	}

	filesystem_item::pointer_type parent() const
	{
		//TODO: ver si usamos lock o si construimos un shared_ptr a partir del weak_ptr
		return this->parent_.lock();
	}

	user_entry_pointer_type root_user_entry() const		//user_entry::pointer_type
	{
		//TODO: ver si usamos lock o si construimos un shared_ptr a partir del weak_ptr
		return this->root_user_entry_.lock();
	}

	const boost::filesystem::path& path() const
	{
		return this->path_;
	}

	bool is_directory() const
	{
		return this->is_directory_;
	}

	boost::uint32_t mask() const
	{
		return this->mask_;
	}


protected:

	//TODO: asignar lo que el usuario quiere monitorear...
	filesystem_item( const boost::filesystem::path& path, const user_entry_pointer_type& root_user_entry )
		: root_user_entry_(root_user_entry), is_directory_(false), file_descriptor_(0), mask_(PN_ALL_EVENTS) //parent_(0),
	{
		set_path( path );

		std::cout << "filesystem_item(...)" << std::endl;
		std::cout << "filesystem_item(...) - " << path_.native_file_string() << std::endl;
	}

	//TODO: asignar lo que el usuario quiere monitorear...
	filesystem_item ( const boost::filesystem::path& path, const user_entry_pointer_type& root_user_entry, const filesystem_item::pointer_type& parent )
		: root_user_entry_(root_user_entry), parent_(parent), is_directory_(false), file_descriptor_(0), mask_(PN_ALL_EVENTS)
	{
		set_path( path );

		std::cout << "filesystem_item(...)" << std::endl;
		std::cout << "filesystem_item(...) - " << path_.native_file_string() << std::endl;
	}

protected:
public: //TODO: sacar
	boost::filesystem::path path_;
	bool is_directory_;

public: //private:

	int file_descriptor_;
	//TODO: ver si es necesario
	boost::uint32_t mask_;
	
	file_inode_info inode_info_;

	boost::weak_ptr<filesystem_item> parent_; //avoid circular references
	//TODO: ver que pasa si agregamos el mismo directorio como dos user_entry distintos... el open da el mismo file descriptor?
	boost::weak_ptr<user_entry> root_user_entry_; //avoid circular references
	collection_type subitems_;
};




} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_FILESYSTEM_ITEM_HPP
