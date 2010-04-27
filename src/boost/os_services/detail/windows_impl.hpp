#ifndef BOOST_OS_SERVICES_DETAIL_WINDOWS_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_WINDOWS_IMPL_HPP

#include <sstream>
#include <string>

#include <boost/bind.hpp>
#include <boost/integer.hpp> 
#include <boost/optional.hpp>
//#include <boost/smart_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/os_services/change_types.hpp>
#include <boost/os_services/detail/base_impl.hpp>
#include <boost/os_services/detail/utils.hpp>
#include <boost/os_services/detail/win32api_wrapper.hpp>
#include <boost/os_services/detail/win32_legacy.hpp>		// directoryInfo
#include <boost/os_services/notify_filters.hpp>


//TODO: ver de usar ASIO... hay varias cosas que se denominan IOCP

namespace boost {
namespace os_services {
namespace detail {

typedef boost::shared_ptr<boost::thread> thread_type;

void directory_info_deleter(LPDIRECTORY_INFO ptr)
{
	if (ptr != 0)
	{
		if ( ptr->directory_handle != 0 )
		{
			//BOOL ret_value = ::CloseHandle( ptr->directory_handle );
			//if ( ret_value == 0 )
			//{
			//	//Destructor -> NO_THROW
			//	std::cerr << "Failed to close directory port handle. Reason: " << GetLastError() << std::endl;
			//}

			try
			{
				win32api_wrapper::close_handle( ptr->directory_handle );
			}
			catch ( const std::runtime_error& )
			{
				//Destructor -> NO_THROW
				std::cerr << "Failed to close directory port handle. Reason: " << GetLastError() << std::endl;
			}
		}

		free( ptr ); //TODO: mmmmmmmmmm WRAPPEAR ESTA CLASE
		ptr = 0;
	}
};


class windows_impl : public base_impl<windows_impl>
{
public:
	windows_impl()
		: completion_port_handle_(0) //, is_started_(false)
	{}

	~windows_impl() //virtual -> deber�amos impedir que esta clase sea heredada.
	{
		if ( completion_port_handle_ != 0 )
		{
			try
			{
				win32api_wrapper::post_queued_completion_status( completion_port_handle_, 0, 0, NULL );
			}
			catch ( const std::runtime_error& )
			{
				//Destructor -> NO_THROW
				std::cerr << "Failed to post to completion port. Reason: " << GetLastError() << std::endl;
				//TODO: Si el Post falla, el Thread nunca va a morir !!
			}
		}

		if ( thread_ )
		{
			thread_->join();
		}

		if ( completion_port_handle_ != 0 )
		{
			try
			{
				win32api_wrapper::close_handle( completion_port_handle_ );
			}
			catch ( const std::runtime_error& )
			{
				//Destructor -> NO_THROW
				std::cerr << "Failed to close completion port handle. Reason: " << GetLastError() << std::endl;
			}
		}
	}


	//TODO: agregar
	//void add_watch_impl( const boost::filesystem::path& dir ) //throw (std::invalid_argument, std::runtime_error)


	void add_watch_impl( const std::string& dir_name ) //throw (std::invalid_argument, std::runtime_error)
	{ 
		LPDIRECTORY_INFO directory_info = (LPDIRECTORY_INFO) malloc(sizeof(DIRECTORY_INFO));
		memset(directory_info, 0, sizeof(DIRECTORY_INFO));

		directory_info->directory_handle = win32api_wrapper::create_file( dir_name, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL );
		//if ( directory_info->directory_handle == INVALID_HANDLE_VALUE )
		//{
		//	std::ostringstream oss;
		//	oss << "Failed to monitor directory - Directory: " << dir_name << " - Reason: " << GetLastError();
		//	throw (std::invalid_argument(oss.str()));
		//}

		//TODO: esto no me gusta
		lstrcpy( directory_info->directory_name, dir_name.c_str() );

		//unsigned long addr = (unsigned long) &directory_info;

		//TODO: usar win32api_wrapper::create_io_completion_port
		//completion_port_handle_ = ::CreateIoCompletionPort( directory_info->directory_handle, completion_port_handle_, (DWORD) directory_info, 0 );
	
		//if ( completion_port_handle_ == 0 )
		//{
		//	std::ostringstream oss;
		//	oss << "Failed to monitor directory - Directory: " << dir_name << " - Reason: " << GetLastError();
		//	throw (std::runtime_error(oss.str()));
		//}

		completion_port_handle_ = win32api_wrapper::create_io_completion_port( directory_info->directory_handle, completion_port_handle_, (DWORD) directory_info, 0 );


		directories_.push_back( directory_info_pointer_type( directory_info, directory_info_deleter ) );
	}

	//TODO: revisar que hacer con las EXCEPTION-SPECIFICATION
	void start() //throw (std::runtime_error)
	{
		//TODO: is_started_ debe ser protegida con MUTEX.
		//if (!is_started_)
		//{
			//TODO: BOOST_FOREACH
			for (vector_type::const_iterator it = directories_.begin(); it!=directories_.end(); ++it)
			{
				//BOOL ret_value = ::ReadDirectoryChangesW ( (*it)->directory_handle, (*it)->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &(*it)->buffer_length, &(*it)->overlapped, NULL);

				//if ( ret_value == 0 )
				//{
				//	std::ostringstream oss;
				//	oss << "Failed to monitor directory - Directory: " << (*it)->directory_name << " - Reason: " << GetLastError();
				//	throw (std::runtime_error(oss.str()));
				//}
				
				win32api_wrapper::read_directory_changes( (*it)->directory_handle, (*it)->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &(*it)->buffer_length, &(*it)->overlapped, NULL);
			}

			thread_.reset( new boost::thread( boost::bind(&windows_impl::handle_directory_changes, this) ) );

		//	is_started_ = true;
		//}
	}

public: //private:  //TODO:

	void handle_directory_changes() //throw (std::runtime_error)
	{
		unsigned long num_bytes;
		unsigned long offset;
		LPDIRECTORY_INFO directory_info;
		LPOVERLAPPED overlapped;
		PFILE_NOTIFY_INFORMATION notify_information;

		do
		{
			win32api_wrapper::get_queued_completion_status( this->completion_port_handle_, &num_bytes, (LPDWORD) &directory_info, &overlapped, INFINITE );

			if ( directory_info ) //TODO: esta pregunta no est� bueno siendo un puntero raw
			{
				if ( num_bytes > 0 )
				{
					notify_information = (PFILE_NOTIFY_INFORMATION)directory_info->buffer;
					//notify_information = static_cast<PFILE_NOTIFY_INFORMATION>(directory_info->buffer);

					boost::optional<std::string> old_name;

					do
					{
						offset = notify_information->NextEntryOffset;

						//if( fni->Action == FILE_ACTION_MODIFIED )
						//      CheckChangedFile( di, fni ); //TODO: chequear en FWATCH

						//TODO: no me gusta, ver de cambiarlo
						std::string file_name(notify_information->FileName, notify_information->FileName + (notify_information->FileNameLength/sizeof(WCHAR)) ); 

						if (notify_information->Action == FILE_ACTION_RENAMED_OLD_NAME)
						{
							old_name.reset( file_name );
						}
						else if (notify_information->Action == FILE_ACTION_RENAMED_NEW_NAME)
						{
							if ( old_name )
							{
								notify_rename_event_args(change_types::renamed, directory_info->directory_name, file_name, *old_name);
								old_name.reset();
							}
							else
							{
								notify_rename_event_args(change_types::renamed, directory_info->directory_name, file_name, "");
								old_name.reset();
							}
						}
						else
						{
							if (old_name)
							{
								notify_rename_event_args(change_types::renamed, directory_info->directory_name, "", *old_name);
								old_name.reset();
							}

							notify_file_system_event_args(notify_information->Action, directory_info->directory_name, file_name);

						}

						notify_information = (PFILE_NOTIFY_INFORMATION)((LPBYTE) notify_information + offset);

					} while( offset );

					//TODO: esto ocasionaba problemas en Linux. Habria que evaluar si tambien pasa en Windows.
					if (old_name)
					{
						notify_rename_event_args(change_types::renamed, directory_info->directory_name, "", *old_name);
						old_name.reset();
					}
				}

				win32api_wrapper::read_directory_changes( directory_info->directory_handle, directory_info->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &directory_info->buffer_length, &directory_info->overlapped, NULL );
			}

		} while( directory_info );
	}

	inline void notify_file_system_event_args( int action, const std::string& directory, const std::string& name )
	{
		//TODO: ver en .Net
		//if (!MatchPattern(name))
		//{
		//	return;
		//}

		switch (action)
		{
			case FILE_ACTION_ADDED:
				{
					do_callback(created_handler_, filesystem_event_args(change_types::created, directory, name));
				}
				break;
			case FILE_ACTION_REMOVED:
				{
					do_callback(deleted_handler_, filesystem_event_args(change_types::deleted, directory, name));
				}
				break;
			case FILE_ACTION_MODIFIED:
				{
					do_callback(changed_handler_, filesystem_event_args(change_types::changed, directory, name));
				}
				break;
			default:
				//Debug.Fail("Unknown FileSystemEvent action type!  Value: " + action);
				break;
		}
	}

	//private void NotifyInternalBufferOverflowEvent()
	//{
	//	InternalBufferOverflowException ex = new InternalBufferOverflowException(SR.GetString(SR.FSW_BufferOverflow, directory));

	//	ErrorEventArgs errevent = new ErrorEventArgs(ex);

	//	OnError(errevent);
	//}

	inline void notify_rename_event_args(int action, const std::string& directory, const std::string& name, const std::string& old_name)
	{
		//filter if neither new name or old name are a match a specified pattern 
		//TODO:
		//if (!MatchPattern(name) && !MatchPattern(oldName))
		//{
		//	return;
		//}
		do_callback(renamed_handler_, renamed_event_args(action, directory, name, old_name));
	}


protected:
	//typedef boost::bimap<HANDLE, std::string> watch_descriptors_type;
	//watch_descriptors_type watch_descriptors_;
	
	//TODO: boost::ptr_vector
	typedef boost::shared_ptr<DIRECTORY_INFO> directory_info_pointer_type;
	typedef std::vector<directory_info_pointer_type> vector_type;
	vector_type directories_;

	HANDLE completion_port_handle_; //HANDLE -> void*
	thread_type thread_;
	//bool is_started_;
};

} // namespace detail
} // namespace os_services
} // namespace boost

//void printBuffer(CHAR* buffer, unsigned long num_bytes, DWORD buffer_length)
//{
//	printf("%d bytes: \n", num_bytes);
//	for (int i = 0; i<num_bytes; ++i)
//	{
//		printf("%u ", (unsigned int)buffer[i]);
//	}
//	printf("\n");
//}

#endif // BOOST_OS_SERVICES_DETAIL_WINDOWS_IMPL_HPP