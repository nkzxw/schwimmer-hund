#ifndef BOOST_OS_SERVICES_DETAIL_WINDOWS_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_WINDOWS_IMPL_HPP

#include <string>

//#include <boost/asio.hpp>

#include <boost/bind.hpp>
//#include <boost/foreach.hpp>
#include <boost/integer.hpp> 
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

//typedef boost::shared_ptr<boost::thread> HeapThread; //TODO: renombrar
typedef shared_ptr<thread> HeapThread; //TODO: renombrar

//template <typename T>
//struct mallocDeleter
//{
//	void operator() (T* ptr)
//	{
//		if (ptr != 0)
//		{
//			free(ptr);
//			ptr = 0; //NULL;
//		}
//	}
//};

void directoryInfoDeleter(LPDIRECTORY_INFO ptr)
{
	if (ptr != 0)
	{
		if ( ptr->directory_handle != 0 )
		{
			//TODO: manejo de errores
			::CloseHandle( ptr->directory_handle );
		}

		free(ptr);
		ptr = 0; //NULL;
	}
};


class windows_impl : public base_impl<windows_impl>
{
public:
	windows_impl()
		: completionPortHandle_(0), number_of_directories(0)
	{}

	virtual ~windows_impl()
	{
		if ( completionPortHandle_ != 0 )
		{
			//TODO: manejo de errores
			::PostQueuedCompletionStatus( completionPortHandle_, 0, 0, NULL );
		}

		if ( thread_ )
		{
			thread_->join();
		}
		
		//if ( directoryInfo.directory_handle != 0 )
		//{
		//	//TODO: manejo de errores
		//	::CloseHandle( directoryInfo.directory_handle );
		//}

		if ( completionPortHandle_ != 0 )
		{
			//TODO: manejo de errores
			::CloseHandle( completionPortHandle_ );
		}
	}


	void add_directory_impl (const std::string& dir_name) throw (std::invalid_argument)
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

		unsigned long addr = (unsigned long) &directory_info;

		//TODO: manejo de errores
		completionPortHandle_ = ::CreateIoCompletionPort ( directory_info->directory_handle, completionPortHandle_, (DWORD) directory_info, 0 );

	
		//watch_descriptors_.insert(watch_descriptors_type::relation(directory_info->directory_handle, dir_name));
		////watch_descriptors_.insert(watch_descriptors_type::relation(directory_info, dir_name));

		//number_of_directories++;

		//boost::shared_ptr<int> p((int*) malloc(sizeof(int)), free);
		//directories_.push_back( DirectoryInfoPointerType( directory_info, mallocDeleter<DIRECTORY_INFO>() ) );
		directories_.push_back( DirectoryInfoPointerType( directory_info, directoryInfoDeleter ) );
		
		
	}


	void start() throw (std::invalid_argument, std::runtime_error) //const std::string& path)
	{

		for (VectorType::const_iterator it = directories_.begin(); it!=directories_.end(); ++it)
		{
			//TODO: manejo de errores
			int retValue = ::ReadDirectoryChangesW ( (*it)->directory_handle, (*it)->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &(*it)->buffer_length, &(*it)->overlapped, NULL);
		}

		//for (int i=0; i < number_of_directories; ++i)
		//{
		//	//LPDIRECTORY_INFO directory_info = directories_[i];

		//	//TODO: manejo de errores
		//	//int retValue = ::ReadDirectoryChangesW ( directory_info_array[i].directory_handle, directory_info_array[i].buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &directory_info_array[i].buffer_length, &directory_info_array[i].overlapped, NULL);
		//	//int retValue = ::ReadDirectoryChangesW ( directory_info->directory_handle, directory_info->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &directory_info->buffer_length, &directory_info->overlapped, NULL);
		//	int retValue = ::ReadDirectoryChangesW ( directories_[i]->directory_handle, directories_[i]->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &directories_[i]->buffer_length, &directories_[i]->overlapped, NULL);
		//}

		thread_.reset( new boost::thread( boost::bind(&windows_impl::HandleDirectoryChange, this) ) );
	}




public: //private:  //TODO:

	//void printBuffer(CHAR* buffer, unsigned long numBytes, DWORD buffer_length)
	//{
	//	printf("%d bytes: \n", numBytes);

	//	for (int i = 0; i<numBytes; ++i)
	//	{
	//		printf("%u ", (unsigned int)buffer[i]);
	//	}

	//	printf("\n");
	//}

	void HandleDirectoryChange()
	{
		unsigned long numBytes;
		unsigned long cbOffset;
		LPDIRECTORY_INFO directoryInfo;
		//DirectoryInfo* di;
		LPOVERLAPPED overlapped;
		PFILE_NOTIFY_INFORMATION notifyInformation;

		//std::vector<boost::asio::const_buffer> buffers;


		do
		{
			//TODO: manejo de errores
			BOOL tempXBool = ::GetQueuedCompletionStatus( this->completionPortHandle_, &numBytes, (LPDWORD) &directoryInfo, &overlapped, INFINITE );

			//if (tempXBool == 0)
			//{
			//	std::cout << "tempBool: " << tempXBool << std::endl;
			//}

			//std::cout << "Readed: " << numBytes << " bytes." << std::endl;

			//if ( numBytes == 0)
			//{
			//	std::cout << "Readed: 0 bytes." << std::endl;
			//}

			if ( directoryInfo )
			{
				if ( numBytes > 0 )
				{
					notifyInformation = (PFILE_NOTIFY_INFORMATION)directoryInfo->buffer;
					//notifyInformation = static_cast<PFILE_NOTIFY_INFORMATION>(directoryInfo->buffer);

					do
					{
						cbOffset = notifyInformation->NextEntryOffset;

						//if( fni->Action == FILE_ACTION_MODIFIED )
						//      CheckChangedFile( di, fni );

						//
						switch ( notifyInformation->Action )
						{
							case FILE_ACTION_ADDED:
								if ( this->created_callback_ )
								{
									std::string fileName(notifyInformation->FileName, notifyInformation->FileName + (notifyInformation->FileNameLength/sizeof(WCHAR)) ); 
									filesystem_event_args temp;
									
									temp.name = fileName;

									//threadObject->This->Created(temp);
									this->created_callback_(temp);
								}

								//{
								//	std::string fileName(notifyInformation->FileName, notifyInformation->FileName + (notifyInformation->FileNameLength/sizeof(WCHAR)) ); 
								//	tempVec.push_back(fileName);
								//	//std::cout << fileName << std::endl;
								//}

								break;
							case FILE_ACTION_REMOVED:
								{

									//std::cout << "file deleted: ";
									//if (threadObject->This->Deleted)
									//if ( this->deleted_callback_ )
									//{
									//	std::string fileName(notifyInformation->FileName, notifyInformation->FileName + (notifyInformation->FileNameLength/sizeof(WCHAR)) ); 
									//	filesystem_event_args temp;
									//	temp.name = fileName;

									//	//threadObject->This->Deleted(temp);
									//	this->deleted_callback_(temp);
									//}


									std::string fileName(notifyInformation->FileName, notifyInformation->FileName + (notifyInformation->FileNameLength/sizeof(WCHAR)) ); 
									//tempVec.push_back(fileName);
									//std::cout << fileName << std::endl;

									filesystem_event_args temp;
									temp.name = fileName;

									do_callback(deleted_callback_, temp);

								}


								break;
							case FILE_ACTION_MODIFIED:
								//if (threadObject->This->Changed)
								if ( this->changed_callback_ )
								{
									std::string fileName(notifyInformation->FileName, notifyInformation->FileName + (notifyInformation->FileNameLength/sizeof(WCHAR)) ); 

									filesystem_event_args temp;
									temp.name = fileName;

									//threadObject->This->Changed(temp);
									this->changed_callback_(temp);
								}


								//{
								//	std::string fileName(notifyInformation->FileName, notifyInformation->FileName + (notifyInformation->FileNameLength/sizeof(WCHAR)) ); 
								//	tempVec.push_back(fileName);
								//	std::cout << fileName << std::endl;
								//}

								break;
							case FILE_ACTION_RENAMED_OLD_NAME:

								//if (threadObject->this->renamed_callback_)
								if ( this->renamed_callback_ )
								{
									std::string fileName(notifyInformation->FileName, notifyInformation->FileName + (notifyInformation->FileNameLength/sizeof(WCHAR)) ); 

									renamed_event_args temp;
									temp.name = fileName;

									//threadObject->this->renamed_callback_(temp);
									this->renamed_callback_(temp);
								}

								//{
								//	std::string fileName(notifyInformation->FileName, notifyInformation->FileName + (notifyInformation->FileNameLength/sizeof(WCHAR)) ); 
								//	tempVec.push_back(fileName);
								//	//std::cout << fileName << std::endl;
								//}


								break;
							case FILE_ACTION_RENAMED_NEW_NAME:
								break;
							default: 
								//std::cout << "unknown event: ";
								break;
						}

						notifyInformation = (PFILE_NOTIFY_INFORMATION)((LPBYTE) notifyInformation + cbOffset);

					} while( cbOffset );

				}

				//TODO: manejo de errores
				// this->notify_filters_ = //FILE_NOTIFY_CHANGE_LAST_WRITE,            
				BOOL tempBoolRead = ::ReadDirectoryChangesW ( directoryInfo->directory_handle, directoryInfo->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_,	&directoryInfo->buffer_length, &directoryInfo->overlapped, NULL );

				//if (tempBoolRead == 0)
				//{
				//	std::cout << "tempBoolRead: " << tempBoolRead << std::endl;
				//}
			}

		} while( directoryInfo );



		std::cout << std::endl;

	}





protected:
	//typedef boost::bimap<HANDLE, std::string> watch_descriptors_type;
	//watch_descriptors_type watch_descriptors_;
	//int number_of_directories;
	
	//TODO: boost::ptr_vector
	typedef boost::shared_ptr<DIRECTORY_INFO> DirectoryInfoPointerType;
	typedef std::vector<DirectoryInfoPointerType> VectorType;
	VectorType directories_;


	HANDLE completionPortHandle_; //void*
	HeapThread thread_;
};

} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_WINDOWS_IMPL_HPP