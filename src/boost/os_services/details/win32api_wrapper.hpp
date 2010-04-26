#ifndef BOOST_OS_SERVICES_WIN32API_WRAPPER_HPP
#define BOOST_OS_SERVICES_WIN32API_WRAPPER_HPP

#include <string>

#include "windows.h"

//TODO: analizar bien donde deberia estar todo esto...


namespace boost {
namespace os_services {
namespace win32api_wrapper
{
	//WINBASEAPI BOOL WINAPI CreateDirectoryA(__in     LPCSTR lpPathName, /*__in_opt*/ LPSECURITY_ATTRIBUTES security_attributes );
	int CreateDirectory ( const std::string& pathName, LPSECURITY_ATTRIBUTES securityAttributes )
	{
		return ::CreateDirectoryA( pathName.c_str(), securityAttributes );
	}

	//void* CreateFile ( const std::string& file_name, unsigned long desired_access, unsigned long share_mode, LPSECURITY_ATTRIBUTES security_attributes, unsigned long creation_disposition, unsigned long flags_and_attributes, void* template_file )
	//{
	//	#ifdef UNICODE
	//	//#define CreateFile  CreateFileW
	//		return ::CreateFileW( file_name.c_str(), desired_access, share_mode, security_attributes, creation_disposition, flags_and_attributes, template_file );
	//	#else
	//		return ::CreateFileA( file_name.c_str(), desired_access, share_mode, security_attributes, creation_disposition, flags_and_attributes, template_file );
	//	#endif // !UNICODE
	//}

	void* CreateFile ( const std::string& file_name, unsigned long desired_access, unsigned long share_mode, LPSECURITY_ATTRIBUTES security_attributes, unsigned long creation_disposition, unsigned long flags_and_attributes, void* template_file )
	{
		void* file_handle;

		#ifdef UNICODE
			//#define CreateFile  CreateFileW
			file_handle = ::CreateFileW( file_name.c_str(), desired_access, share_mode, security_attributes, creation_disposition, flags_and_attributes, template_file );
		#else
			file_handle = ::CreateFileA( file_name.c_str(), desired_access, share_mode, security_attributes, creation_disposition, flags_and_attributes, template_file );
		#endif // !UNICODE


		if ( file_handle == INVALID_HANDLE_VALUE )
		{
			std::ostringstream oss;
			//oss << "Failed to monitor directory - Directory: " << dir_name << " - Reason: " << GetLastError();
			oss << "CreateFile Win32API failed - Directory: " << file_name << " - Reason: " << GetLastError();
			throw ( std::invalid_argument(oss.str()) );
		}

		return file_handle;
	}




} // namespace win32api_wrapper
} // namespace os_services
} // namespace boost



#endif // BOOST_OS_SERVICES_WIN32API_WRAPPER_HPP
