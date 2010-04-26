#ifndef BOOST_OS_SERVICES_DETAIL_WIN32API_WRAPPER_HPP
#define BOOST_OS_SERVICES_DETAIL_WIN32API_WRAPPER_HPP

#include <string>

#include "windows.h"

//TODO: analizar bien donde deberia estar todo esto...


namespace boost {
namespace os_services {
namespace detail {
namespace win32api_wrapper
{
	//TODO: agregar las variantes NO-THROW de cada una de estas funciones... Ver si es apropiado conceptualmente, ya que justamente estas variantes NO-THROW son las API directamente...

	//WINBASEAPI BOOL WINAPI CreateDirectoryA(__in     LPCSTR lpPathName, /*__in_opt*/ LPSECURITY_ATTRIBUTES security_attributes );
	int create_directory ( const std::string& path_name, LPSECURITY_ATTRIBUTES security_attributes )
	{
		return ::CreateDirectoryA( path_name.c_str(), security_attributes );

		//TODO: CreateDirectoryW ????
		//TODO: throw
	}

	void* create_file ( const std::string& file_name, unsigned long desired_access, unsigned long share_mode, LPSECURITY_ATTRIBUTES security_attributes, unsigned long creation_disposition, unsigned long flags_and_attributes, void* template_file )
	{
		#ifdef UNICODE
			//#define CreateFile  CreateFileW
			void* handle = ::CreateFileW( file_name.c_str(), desired_access, share_mode, security_attributes, creation_disposition, flags_and_attributes, template_file );
		#else
			void* handle = ::CreateFileA( file_name.c_str(), desired_access, share_mode, security_attributes, creation_disposition, flags_and_attributes, template_file );
		#endif // !UNICODE


		if ( handle == INVALID_HANDLE_VALUE )
		{
			std::ostringstream oss;
			//oss << "Failed to monitor directory - Directory: " << dir_name << " - Reason: " << GetLastError();
			oss << "CreateFile Win32API failed - Directory: " << file_name << " - Reason: " << GetLastError();
			throw ( std::invalid_argument(oss.str()) );
		}

		return handle;
	}

	void* create_io_completion_port( void* file_handle, void* existing_completion_port, unsigned long completion_key, unsigned long number_of_concurrent_threads )
	{
		//WINBASEAPI __out_opt HANDLE WINAPI CreateIoCompletionPort( __in     HANDLE FileHandle, __in_opt HANDLE ExistingCompletionPort, __in     ULONG_PTR CompletionKey, __in     DWORD NumberOfConcurrentThreads );
		void* handle = ::CreateIoCompletionPort ( file_handle, existing_completion_port, completion_key, number_of_concurrent_threads );

		if ( handle == 0 ) //TODO: poner CERO como # define
		{
			std::ostringstream oss;
			oss << "CreateIoCompletionPort Win32API failed - Reason: " << GetLastError();
			throw ( std::runtime_error(oss.str()) ); //TODO: analizar si conviene lanzar un runtime_error o excepciones personalizadas para win32Api
		}

		return handle;
	}

	void close_handle( void* handle )
	{
		//WINBASEAPI BOOL WINAPI CloseHandle(__in HANDLE hObject );
		int ret_value = ::CloseHandle( handle );

		if ( ret_value == 0 )
		{
			//std::cerr << "Failed to close directory port handle. Reason: " << GetLastError() << std::endl;
			std::ostringstream oss;
			oss << "CloseHandle Win32API failed - Reason: " << GetLastError();
			throw ( std::runtime_error(oss.str()) ); //TODO: analizar si conviene lanzar un runtime_error o excepciones personalizadas para win32Api

		}
	}


	int post_queued_completion_status( void* completion_port, unsigned long number_of_bytes_transferred, unsigned long completion_key, LPOVERLAPPED overlapped )
	{
		// WINBASEAPI BOOL WINAPI PostQueuedCompletionStatus( __in     HANDLE CompletionPort, __in     DWORD dwNumberOfBytesTransferred, __in     ULONG_PTR dwCompletionKey, __in_opt LPOVERLAPPED lpOverlapped );
		int ret_value = ::PostQueuedCompletionStatus( completion_port, number_of_bytes_transferred, completion_key, overlapped );

		if ( ret_value == 0 )
		{
			//std::cerr << "Failed to close directory port handle. Reason: " << GetLastError() << std::endl;
			std::ostringstream oss;
			oss << "PostQueuedCompletionStatus Win32API failed - Reason: " << GetLastError();
			throw ( std::runtime_error(oss.str()) ); //TODO: analizar si conviene lanzar un runtime_error o excepciones personalizadas para win32Api
		}

		return ret_value;
	}



	int read_directory_changes( void* directory, void far* buffer, unsigned long buffer_length, int watch_subtree, unsigned long notify_filter, unsigned long far* bytes_returned, LPOVERLAPPED overlapped, LPOVERLAPPED_COMPLETION_ROUTINE completion_routine )
	{
		//WINBASEAPI BOOL WINAPI ReadDirectoryChangesW( __in        HANDLE hDirectory, __out_bcount_part(nBufferLength, *lpBytesReturned) LPVOID lpBuffer, __in        DWORD nBufferLength, __in        BOOL bWatchSubtree, __in        DWORD dwNotifyFilter, __out_opt   LPDWORD lpBytesReturned, __inout_opt LPOVERLAPPED lpOverlapped, __in_opt    LPOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine );
		int ret_value = ::ReadDirectoryChangesW ( directory, buffer, buffer_length, watch_subtree, notify_filter, bytes_returned, overlapped, completion_routine );

		if ( ret_value == 0 )
		{
			std::ostringstream oss;
			oss << "ReadDirectoryChangesW Win32API failed - Reason: " << GetLastError();
			throw ( std::runtime_error(oss.str()) );
		}

		return ret_value;
	}

	//TODO: todas las apis que retornan BOOL (int) y este es solo un codigo de error, retornar void, ya que el error es arrojado via excepciones
	int get_queued_completion_status( void* completion_port, unsigned long far* number_of_bytes_transferred, unsigned long* completion_key, LPOVERLAPPED* overlapped, unsigned long milliseconds )
	{
		//WINBASEAPI BOOL WINAPI GetQueuedCompletionStatus( __in  HANDLE CompletionPort, __out LPDWORD lpNumberOfBytesTransferred, __out PULONG_PTR lpCompletionKey, __out LPOVERLAPPED *lpOverlapped, __in  DWORD dwMilliseconds );
		int ret_value = ::GetQueuedCompletionStatus( completion_port, number_of_bytes_transferred, completion_key, overlapped, milliseconds );

		if ( ret_value == 0 )
		{
			std::ostringstream oss;
			oss << "GetQueuedCompletionStatus Win32API failed - Reason: " << GetLastError();
			throw ( std::runtime_error(oss.str()) );
		}

		return ret_value;
	}



} // namespace win32api_wrapper
} // namespace detail
} // namespace os_services
} // namespace boost



#endif // BOOST_OS_SERVICES_DETAIL_WIN32API_WRAPPER_HPP
