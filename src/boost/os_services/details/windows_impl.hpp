#ifndef BOOST_OS_SERVICES_DETAIL_WINDOWS_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_WINDOWS_IMPL_HPP

#include <string>

#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <boost/os_services/details/base_impl.hpp>
#include <boost/os_services/notify_filters.hpp>
#include <boost/os_services/win32api_wrapper.hpp>
#include <boost/os_services/win32_legacy.hpp>		// directoryInfo




//TODO: ver de usar ASIO... hay varias cosas que se denominan IOCP

namespace boost {
namespace os_services {
namespace detail {

//typedef boost::shared_ptr<boost::thread> HeapThread; //TODO: renombrar
typedef shared_ptr<thread> HeapThread; //TODO: renombrar

class windows_impl : public base_impl<windows_impl>
{
public:
	windows_impl()
		: completionPortHandle_(0)
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
		
		if ( directoryInfo.directoryHandle != 0 )
		{
			//TODO: manejo de errores
			::CloseHandle( directoryInfo.directoryHandle );
		}

		if ( completionPortHandle_ != 0 )
		{
			//TODO: manejo de errores
			::CloseHandle( completionPortHandle_ );
		}
	}

	void start(const std::string& path)
	{
		
		directoryInfo.directoryHandle = win32api_wrapper::CreateFile( path, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL );

		if ( directoryInfo.directoryHandle == INVALID_HANDLE_VALUE )
		{
			//TODO: manejo de excepciones
			std::cout << "Unable to open directory " << path << ". GLE=" << GetLastError() << ". Terminating..." << std::endl; 
			return;
		}

		lstrcpy( directoryInfo.directoryName, path.c_str() );

		unsigned long addr = (unsigned long) &directoryInfo;

		//TODO: manejo de errores
		// Set up a key(directory info) for each directory
		completionPortHandle_ = ::CreateIoCompletionPort ( directoryInfo.directoryHandle, completionPortHandle_, (unsigned long) addr, 0 );


		//ReadDirectoryChangesW
		//	( 
		//	directoryInfo.directoryHandle,		// HANDLE TO DIRECTORY
		//	directoryInfo.buffer,               // Formatted buffer into which read results are returned.  This is a
		//	MAX_BUFFER,                         // Length of previous parameter, in bytes
		//	this->include_subdirectories_ ? 1 : 0,	//TRUE,  // Monitor sub trees?
		//	this->notify_filters_,						// FILE_NOTIFY_CHANGE_LAST_WRITE,      // What we are watching for
		//	&directoryInfo.bufferLength,        // Number of bytes returned into second parameter
		//	&directoryInfo.overlapped,          // OVERLAPPED structure that supplies data to be used during an asynchronous operation.  If this is NULL, ReadDirectoryChangesW does not return immediately.
		//	NULL								// Completion routine
		//	);      

		//TODO: manejo de errores
		int retValue = ::ReadDirectoryChangesW ( directoryInfo.directoryHandle, directoryInfo.buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &directoryInfo.bufferLength, &directoryInfo.overlapped, NULL);

		thread_.reset( new boost::thread( boost::bind(&windows_impl::HandleDirectoryChange, this) ) );
	}

public: //private:  //TODO:

	void printBuffer(CHAR* buffer, unsigned long numBytes, DWORD bufferLength)
	{
		printf("%d bytes: \n", numBytes);

		for (int i = 0; i<numBytes; ++i)
		{
			printf("%u ", (unsigned int)buffer[i]);
		}

		printf("\n");
	}

	void HandleDirectoryChange()
	{
		unsigned long numBytes;
		unsigned long cbOffset;
		LPDIRECTORY_INFO directoryInfo;
		//DirectoryInfo* di;
		LPOVERLAPPED overlapped;
		PFILE_NOTIFY_INFORMATION notifyInformation;

		do
		{
			//TODO: manejo de errores
			::GetQueuedCompletionStatus( this->completionPortHandle_, &numBytes, (LPDWORD) &directoryInfo, &overlapped, INFINITE );

			//boost::asio::buffer bx;  //(directoryInfo->buffer, numBytes);

			if ( directoryInfo )
			{
				//printBuffer(directoryInfo->buffer, numBytes, directoryInfo->bufferLength);

				//fni = (PFILE_NOTIFY_INFORMATION)di->lpBuffer;
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

							break;
						case FILE_ACTION_RENAMED_NEW_NAME:
							break;
						default: 
							std::cout << "unknown event: ";
							break;
					}

					notifyInformation = (PFILE_NOTIFY_INFORMATION)((LPBYTE) notifyInformation + cbOffset);

				} while( cbOffset );

				//TODO: manejo de errores
				// this->notify_filters_ = //FILE_NOTIFY_CHANGE_LAST_WRITE,            
				::ReadDirectoryChangesW ( directoryInfo->directoryHandle, directoryInfo->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_,	&directoryInfo->bufferLength, &directoryInfo->overlapped, NULL );
			}

		} while( directoryInfo );


	}

	void* completionPortHandle_;
	HeapThread thread_;
};

//typedef WindowsImpl PlatformImpl;

} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_WINDOWS_IMPL_HPP