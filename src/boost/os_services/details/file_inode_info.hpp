#ifndef BOOST_OS_SERVICES_DETAIL_FILE_INODE_INFO_HPP
#define BOOST_OS_SERVICES_DETAIL_FILE_INODE_INFO_HPP


//#include <sstream>
//#include <string>
//#include <cstring>	// for strerror

#include <sys/stat.h>

#include <boost/filesystem/path.hpp>


namespace boost {
namespace os_services {
namespace detail {

	
//TODO: class
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
		if ( return_code == -1) //TODO: pasar "-1" como una macro SYSTEM_CALL_ERROR o algo así...
		{
			std::ostringstream oss;
			oss << "lstat error - File: " << path.native_file_string() << " - Reason: " << std::strerror(errno);

			//std::cout << "THROW - void file_inode_info::set ( const boost::filesystem::path& path )" << std::endl;

			throw (std::runtime_error(oss.str()));
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

} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_FILE_INODE_INFO_HPP
