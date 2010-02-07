#ifndef BOOST_OS_SERVICES_WIN32API_WRAPPER_HPP
#define BOOST_OS_SERVICES_WIN32API_WRAPPER_HPP

#include <string>

#include "windows.h"

//TODO: analizar bien donde deberia estar todo esto...


namespace boost {
namespace os_services {
namespace win32api_wrapper
{
	//WINBASEAPI BOOL WINAPI CreateDirectoryA(__in     LPCSTR lpPathName, /*__in_opt*/ LPSECURITY_ATTRIBUTES lpSecurityAttributes );
	int CreateDirectory ( const std::string& pathName, LPSECURITY_ATTRIBUTES securityAttributes )
	{
		return ::CreateDirectoryA(pathName.c_str(), securityAttributes);
	}


	
		
	void* CreateFile ( const std::string& lpFileName, unsigned long dwDesiredAccess, unsigned long dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, unsigned long dwCreationDisposition, unsigned long dwFlagsAndAttributes, void* hTemplateFile )
	{
		
		#ifdef UNICODE
		//#define CreateFile  CreateFileW
			return ::CreateFileW( lpFileName.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
		#else
			return ::CreateFileA( lpFileName.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
		#endif // !UNICODE
	}

} // namespace win32api_wrapper
} // namespace os_services
} // namespace boost



#endif // BOOST_OS_SERVICES_WIN32API_WRAPPER_HPP
