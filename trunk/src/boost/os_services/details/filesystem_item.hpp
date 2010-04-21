#ifndef BOOST_OS_SERVICES_DETAIL_FILESYSTEM_ITEM_HPP
#define BOOST_OS_SERVICES_DETAIL_FILESYSTEM_ITEM_HPP

//#include <iostream>


namespace boost {
namespace os_services {
namespace detail {


//TODO: renombrar
class filesystem_item
{
public:

	//typedef boost::shared_ptr<filesystem_item> pointer_type;
	//typedef std::vector<pointer_type> collection_type;

	typedef filesystem_item* pointer_type;
	typedef boost::ptr_vector<filesystem_item> collection_type;

	//TODO: asignar lo que el usuario quiere monitorear...
	explicit filesystem_item( const boost::filesystem::path& path, user_entry_pointer_type root_user_entry )
		: root_user_entry_(root_user_entry), parent_(0), is_directory_(false), file_descriptor_(0), mask_(PN_ALL_EVENTS)
	{
		set_path( path );
	}

	//TODO: asignar lo que el usuario quiere monitorear...
	explicit filesystem_item ( const boost::filesystem::path& path, user_entry_pointer_type root_user_entry, filesystem_item::pointer_type parent )
		: root_user_entry_(root_user_entry), parent_(parent), is_directory_(false), file_descriptor_(0), mask_(PN_ALL_EVENTS)
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

	filesystem_item::pointer_type parent_;

	file_inode_info inode_info_;
	collection_type subitems_;

	//TODO: ver que pasa si agregamos el mismo directorio como dos user_entry distintos... el open da el mismo file descriptor?
	//user_entry* root_user_entry_; 
	//user_entry::pointer_type root_user_entry_;
	user_entry_pointer_type root_user_entry_; 
};




} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_FILESYSTEM_ITEM_HPP
