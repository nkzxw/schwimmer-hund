#ifndef WinNT32Impl_h__
#define WinNT32Impl_h__

#include <string>

#include <boost/bind.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>

#include "BaseImpl.hpp"
#include "EventArgs.hpp"
#include "EventHandlers.hpp"
#include "NotifyFilters.hpp"
#include "Win32ApiWrapper.hpp"
#include "Win32Legacy.hpp"			// directoryInfo

namespace detail
{

typedef boost::shared_ptr<boost::thread> HeapThread;

class WinNT32Impl : public BaseImpl
{
public:
	WinNT32Impl()
		: completionPortHandle_(0)
	{}

	virtual ~WinNT32Impl()
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

	void startMonitoring(const std::string& path)
	{
		directoryInfo.directoryHandle = Win32ApiWrapper::CreateFile( path, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL );

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
		//	this->includeSubdirectories_ ? 1 : 0,	//TRUE,  // Monitor sub trees?
		//	this->notifyFilters_,						// FILE_NOTIFY_CHANGE_LAST_WRITE,      // What we are watching for
		//	&directoryInfo.bufferLength,        // Number of bytes returned into second parameter
		//	&directoryInfo.overlapped,          // OVERLAPPED structure that supplies data to be used during an asynchronous operation.  If this is NULL, ReadDirectoryChangesW does not return immediately.
		//	NULL								// Completion routine
		//	);      

		//TODO: manejo de errores
		int retValue = ::ReadDirectoryChangesW ( directoryInfo.directoryHandle, directoryInfo.buffer, MAX_BUFFER, this->includeSubdirectories_ ? 1 : 0, this->notifyFilters_, &directoryInfo.bufferLength, &directoryInfo.overlapped, NULL);

		thread_.reset( new boost::thread( boost::bind(&WinNT32Impl::HandleDirectoryChange, this) ) );
	}

	// Event Handlers
	FileSystemEventHandler Changed;
	FileSystemEventHandler Created;
	FileSystemEventHandler Deleted;
	RenamedEventHandler Renamed;

public: //private:  //TODO:

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

			if ( directoryInfo )
			{
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
							if (this->Created)
							{
								std::string fileName(notifyInformation->FileName, notifyInformation->FileName + (notifyInformation->FileNameLength/sizeof(WCHAR)) ); 
								FileSystemEventArgs temp;
								temp.Name = fileName;

								//threadObject->This->Created(temp);
								this->Created(temp);
							}

							break;
						case FILE_ACTION_REMOVED:
							//std::cout << "file deleted: ";
							//if (threadObject->This->Deleted)
							if (this->Deleted)
							{
								std::string fileName(notifyInformation->FileName, notifyInformation->FileName + (notifyInformation->FileNameLength/sizeof(WCHAR)) ); 
								FileSystemEventArgs temp;
								temp.Name = fileName;

								//threadObject->This->Deleted(temp);
								this->Deleted(temp);
							}

							break;
						case FILE_ACTION_MODIFIED:
							//if (threadObject->This->Changed)
							if (this->Changed)
							{
								std::string fileName(notifyInformation->FileName, notifyInformation->FileName + (notifyInformation->FileNameLength/sizeof(WCHAR)) ); 

								FileSystemEventArgs temp;
								temp.Name = fileName;

								//threadObject->This->Changed(temp);
								this->Changed(temp);
							}

							break;
						case FILE_ACTION_RENAMED_OLD_NAME:

							//if (threadObject->This->Renamed)
							if (this->Renamed)
							{
								std::string fileName(notifyInformation->FileName, notifyInformation->FileName + (notifyInformation->FileNameLength/sizeof(WCHAR)) ); 

								RenamedEventArgs temp;
								temp.Name = fileName;

								//threadObject->This->Renamed(temp);
								this->Renamed(temp);
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
				// this->notifyFilters_ = //FILE_NOTIFY_CHANGE_LAST_WRITE,            
				::ReadDirectoryChangesW ( directoryInfo->directoryHandle, directoryInfo->buffer, MAX_BUFFER, this->includeSubdirectories_ ? 1 : 0, this->notifyFilters_,	&directoryInfo->bufferLength, &directoryInfo->overlapped, NULL );
			}

		} while( directoryInfo );


	}

	void* completionPortHandle_;
	HeapThread thread_;
};

typedef WinNT32Impl PlatformImpl;

}

#endif // WinNT32Impl_h__