#ifndef BOOST_OS_SERVICES_DETAIL_FILE_INODE_INFO_DUMMY_HPP
#define BOOST_OS_SERVICES_DETAIL_FILE_INODE_INFO_DUMMY_HPP

//#include <sstream>
//#include <string>

#include <boost/filesystem/path.hpp>

////from sys/stat.h	//redefinition on winodows...
//struct stat {
//	int st_dev;               /* inode's device */
//	int st_ino;               /* inode's number */
//};



namespace boost {
namespace os_services {
namespace detail {

	
//TODO: class
struct file_inode_info_dummy
{
	//TODO: public, protected, private ?????
	file_inode_info_dummy()
		: device_id_(0), inode_number_(0)
	{}

	file_inode_info_dummy( int device_id, int inode_number )
		: device_id_(device_id), inode_number_(inode_number)
	{}

	file_inode_info_dummy( const boost::filesystem::path& path )
	{
		set( path );
	}

	file_inode_info_dummy( const file_inode_info_dummy& other )
	{
		this->device_id_ = other.device_id_;
		this->inode_number_ = other.inode_number_;
	}

	file_inode_info_dummy& operator=( const file_inode_info_dummy& other )
	{
		if ( this != &other )
		{
			this->device_id_ = other.device_id_;
			this->inode_number_ = other.inode_number_;
		}
		return *this;
	}

	void set( int device_id, int inode_number )
	{
		this->device_id_ = device_id;
		this->inode_number_ = inode_number;
	}

	//void set( const struct stat& st )
	//{
	//	this->device_id_ = st.st_dev;
	//	this->inode_number_ = st.st_ino;
	//}

	void set( const boost::filesystem::path& path )
	{
		this->device_id_ = 1;
		this->inode_number_ = 1;
	}

	bool operator==( const file_inode_info_dummy& other ) const
	{
		return ( this->device_id_ == other.device_id_ && this->inode_number_ == other.inode_number_ );
	}

	//bool operator==(const struct stat& other) const
	//{
	//	return ( this->device_id_ == other.st_dev && this->inode_number_ == other.st_ino );
	//}

	int device_id_;
	int inode_number_;
};

} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_FILE_INODE_INFO_DUMMY_HPP
