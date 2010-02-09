#ifndef BOOST_OS_SERVICES_DETAIL_WINDOWS_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_WINDOWS_IMPL_HPP

#include <string>

//#include <boost/asio.hpp>

#include <boost/bind.hpp>
//#include <boost/foreach.hpp>
#include <boost/integer.hpp> 
#include <boost/optional.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

#include <boost/os_services/details/base_impl.hpp>
#include <boost/os_services/notify_filters.hpp>
#include <boost/os_services/win32api_wrapper.hpp>
#include <boost/os_services/win32_legacy.hpp>		// directoryInfo
#include <sstream>

//TODO: ver de usar ASIO... hay varias cosas que se denominan IOCP

namespace boost {
namespace os_services {
namespace detail {

typedef boost::shared_ptr<boost::thread> HeapThread; //TODO: renombrar

void directory_info_deleter(LPDIRECTORY_INFO ptr)
{
	if (ptr != 0)
	{
		if ( ptr->directory_handle != 0 )
		{
			BOOL ret_value = ::CloseHandle( ptr->directory_handle );

			if ( ret_value == 0 )
			{
				//Destructor -> NO_THROW
				std::cerr << "Failed to close directory port handle. Reason: " << GetLastError();
			}

		}

		free(ptr);
		ptr = 0; //NULL;
	}
};




class windows_impl : public base_impl<windows_impl>
{
public:
	windows_impl()
		: completionPortHandle_(0), is_started_(false)
	{}

	virtual ~windows_impl()
	{
		if ( completionPortHandle_ != 0 )
		{
			BOOL ret_value = ::PostQueuedCompletionStatus( completionPortHandle_, 0, 0, NULL );

			if ( ret_value == 0 )
			{
				//Destructor -> NO_THROW
				std::cerr << "Failed to post to completion port. Reason: " << GetLastError();
				//TODO: Si el Post falla, el Thread nunca va a morir !!
			}
		}

		if ( thread_ )
		{
			thread_->join();
		}

		if ( completionPortHandle_ != 0 )
		{
			BOOL ret_value = ::CloseHandle( completionPortHandle_ );

			if ( ret_value == 0 )
			{
				//Destructor -> NO_THROW
				std::cerr << "Failed to close completion port handle. Reason: " << GetLastError();
			}


		}
	}

	void add_directory_impl (const std::string& dir_name) throw (std::invalid_argument, std::runtime_error)
	{ 
		LPDIRECTORY_INFO directory_info = (LPDIRECTORY_INFO) malloc(sizeof(DIRECTORY_INFO));
		memset(directory_info, 0, sizeof(DIRECTORY_INFO));

		directory_info->directory_handle = win32api_wrapper::CreateFile( dir_name, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL );

		if ( directory_info->directory_handle == INVALID_HANDLE_VALUE )
		{
			std::ostringstream oss;
			oss << "Failed to monitor directory - Directory: " << dir_name << " - Reason: " << GetLastError();
			throw (std::invalid_argument(oss.str()));
		}

		lstrcpy( directory_info->directory_name, dir_name.c_str() );

		//unsigned long addr = (unsigned long) &directory_info;

		completionPortHandle_ = ::CreateIoCompletionPort ( directory_info->directory_handle, completionPortHandle_, (DWORD) directory_info, 0 );
	
		if ( completionPortHandle_ == 0 )
		{
			std::ostringstream oss;
			oss << "Failed to monitor directory - Directory: " << dir_name << " - Reason: " << GetLastError();
			throw (std::runtime_error(oss.str()));
		}

		directories_.push_back( DirectoryInfoPointerType( directory_info, directory_info_deleter ) );
	}

	void start() throw (std::runtime_error)
	{
		//TODO: is_started_ debe ser protegida con MUTEX.
		if (!is_started_)
		{
			for (VectorType::const_iterator it = directories_.begin(); it!=directories_.end(); ++it)
			{
				BOOL ret_value = ::ReadDirectoryChangesW ( (*it)->directory_handle, (*it)->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &(*it)->buffer_length, &(*it)->overlapped, NULL);

				if ( ret_value == 0 )
				{
					std::ostringstream oss;
					oss << "Failed to monitor directory - Directory: " << (*it)->directory_name << " - Reason: " << GetLastError();
					throw (std::runtime_error(oss.str()));
				}
			}

			thread_.reset( new boost::thread( boost::bind(&windows_impl::handle_directory_changes, this) ) );

			is_started_ = true;
		}
	}




public: //private:  //TODO:


	void handle_directory_changes() throw (std::runtime_error)
	{
		unsigned long num_bytes;
		unsigned long offset;
		LPDIRECTORY_INFO directory_info;
		LPOVERLAPPED overlapped;
		PFILE_NOTIFY_INFORMATION notify_information;

		do
		{
			BOOL ret_value = ::GetQueuedCompletionStatus( this->completionPortHandle_, &num_bytes, (LPDWORD) &directory_info, &overlapped, INFINITE );

			if ( ret_value == 0 )
			{
				std::ostringstream oss;
				oss << "Runtime error. Reason: " << GetLastError();
				throw (std::runtime_error(oss.str()));
			}

			if ( directory_info )
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
							if ( old_name ) // != null)
							{
								// WatcherChangeTypes.Renamed
								notify_rename_event_args(4, file_name, *old_name);
								old_name.reset(); // = null;
							}
							else
							{
								//NotifyRenameEventArgs(WatcherChangeTypes.Renamed, file_name, oldName);
								//oldName = null;

								notify_rename_event_args(4, file_name, "");
								old_name.reset(); // = null;
							}
						}
						else
						{
							if (old_name) // != null)
							{
								//NotifyRenameEventArgs(WatcherChangeTypes.Renamed, null, oldName);
								//oldName = null;

								notify_rename_event_args(4, "", *old_name);
								old_name.reset(); // = null;

							}

							// Notify each file of change
							//NotifyFileSystemEventArgs(action, file_name);
							notify_file_system_event_args(notify_information->Action, file_name);

						}

						//switch ( notify_information->Action )
						//{
						//	case FILE_ACTION_ADDED:
						//		if ( this->created_callback_ )
						//		{
						//			std::string fileName(notify_information->FileName, notify_information->FileName + (notify_information->FileNameLength/sizeof(WCHAR)) ); 
						//			filesystem_event_args temp;
						//			
						//			temp.name = fileName;
						//			//TODO: chequear si la barra ya está incluida en el path del directorio.
						//			temp.full_path = directory_info->directory_name + '\\' + fileName;

						//			
						//			//threadObject->This->Created(temp);
						//			this->created_callback_(temp);
						//		}
						//		break;
						//	case FILE_ACTION_REMOVED:
						//		{
						//			//std::cout << "file deleted: ";
						//			//if (threadObject->This->Deleted)
						//			//if ( this->deleted_callback_ )
						//			//{
						//			//	std::string fileName(notify_information->FileName, notify_information->FileName + (notify_information->FileNameLength/sizeof(WCHAR)) ); 
						//			//	filesystem_event_args temp;
						//			//	temp.name = fileName;

						//			//	//threadObject->This->Deleted(temp);
						//			//	this->deleted_callback_(temp);
						//			//}


						//			std::string fileName(notify_information->FileName, notify_information->FileName + (notify_information->FileNameLength/sizeof(WCHAR)) ); 
						//			//tempVec.push_back(fileName);
						//			//std::cout << fileName << std::endl;

						//			filesystem_event_args temp;
						//			temp.name = fileName;
						//			//TODO: chequear si la barra ya está incluida en el path del directorio.
						//			temp.full_path = directory_info->directory_name + '\\' + fileName;

						//			do_callback(deleted_callback_, temp);

						//		}
						//		break;
						//	case FILE_ACTION_MODIFIED:
						//		//if (threadObject->This->Changed)
						//		if ( this->changed_callback_ )
						//		{
						//			std::string fileName(notify_information->FileName, notify_information->FileName + (notify_information->FileNameLength/sizeof(WCHAR)) ); 
						//			filesystem_event_args temp;
						//			temp.name = fileName;
						//			//TODO: chequear si la barra ya está incluida en el path del directorio.
						//			temp.full_path = std::string(directory_info->directory_name) + '\\' + fileName;


						//			//threadObject->This->Changed(temp);
						//			this->changed_callback_(temp);
						//		}
						//		break;
						//	case FILE_ACTION_RENAMED_OLD_NAME:
						//		{
						//			std::string fileName(notify_information->FileName, notify_information->FileName + (notify_information->FileNameLength/sizeof(WCHAR)) ); 
						//			old_name.reset( fileName ); //TODO: no me gusta.
						//			
						//		}
						//		break;
						//	case FILE_ACTION_RENAMED_NEW_NAME:
						//		{
						//			if ( this->renamed_callback_ )
						//			{
						//				std::string fileName(notify_information->FileName, notify_information->FileName + (notify_information->FileNameLength/sizeof(WCHAR)) ); 

						//				renamed_event_args temp;
						//				temp.name = fileName;
						//				temp.old_name = old_name;
						//				//TODO: chequear si la barra ya está incluida en el path del directorio.
						//				temp.full_path = directory_info->directory_name + '\\' + fileName;



						//				this->renamed_callback_(temp);

						//				old_name.reset();
						//			}

						//		}
						//		break;
						//	default: 
						//		//std::cout << "unknown event: ";
						//		break;
						//}

						notify_information = (PFILE_NOTIFY_INFORMATION)((LPBYTE) notify_information + offset);

					} while( offset );

					if (old_name) // != null)
					{
						//NotifyRenameEventArgs(WatcherChangeTypes.Renamed, null, oldName);
						//oldName = null;

						notify_rename_event_args(4, "", *old_name);
						old_name.reset(); // = null;
					}

				}

				ret_value = ::ReadDirectoryChangesW ( directory_info->directory_handle, directory_info->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &directory_info->buffer_length, &directory_info->overlapped, NULL );

				if ( ret_value == 0 )
				{
					std::ostringstream oss;
					oss << "Runtime error. Reason: " << GetLastError();
					throw (std::runtime_error(oss.str()));
				}
			}

		} while( directory_info );

		//std::cout << std::endl;
	}




	inline void notify_file_system_event_args( int action, std::string name )
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
					//OnCreated(new FileSystemEventArgs(WatcherChangeTypes.Created, directory, name));
					//TODO: cambiar directory y change_type
					do_callback(created_callback_, filesystem_event_args(1, "directory", name));
				}
				break;
			case FILE_ACTION_REMOVED:
				{
					//OnDeleted(new FileSystemEventArgs(WatcherChangeTypes.Deleted, directory, name));
					//TODO: cambiar directory y change_type
					do_callback(deleted_callback_, filesystem_event_args(2, "directory", name));
				}
				break;
			case FILE_ACTION_MODIFIED:
				{
					//OnChanged(new FileSystemEventArgs(WatcherChangeTypes.Changed, directory, name));
					//TODO: cambiar directory y change_type
					do_callback(changed_callback_, filesystem_event_args(3, "directory", name));

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


	// WatcherChangeTypes action
	inline void notify_rename_event_args(int action, std::string name, std::string old_name)
	{
		//filter if neither new name or old name are a match a specified pattern 

		//TODO:
		//if (!MatchPattern(name) && !MatchPattern(oldName))
		//{
		//	return;
		//}

		//TODO: "directory"
		do_callback(renamed_callback_, renamed_event_args(action, "directory", name, old_name));
	}


protected:
	//typedef boost::bimap<HANDLE, std::string> watch_descriptors_type;
	//watch_descriptors_type watch_descriptors_;
	
	//TODO: boost::ptr_vector
	typedef boost::shared_ptr<DIRECTORY_INFO> DirectoryInfoPointerType;
	typedef std::vector<DirectoryInfoPointerType> VectorType;
	VectorType directories_;


	HANDLE completionPortHandle_; //void*
	HeapThread thread_;

	bool is_started_;
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


