#ifndef BOOST_OS_SERVICES_WIN32_LEGACY_HPP
#define BOOST_OS_SERVICES_WIN32_LEGACY_HPP


#include <boost/array.hpp>

//----------------------------------------------------------------------------------------------------------------
// TODO: Legacy-Code, me gustaría reemplazarlo por código C++
// WIN32

#define MAX_DIRS    256
#define MAX_BUFFER  8192 //4096

typedef struct _DIRECTORY_INFO 
{
	HANDLE      directory_handle;
	TCHAR       directory_name[MAX_PATH];
	CHAR        buffer[MAX_BUFFER];
	DWORD       buffer_length;
	OVERLAPPED  overlapped;
} DIRECTORY_INFO, *PDIRECTORY_INFO, *LPDIRECTORY_INFO;

DIRECTORY_INFO  directory_info_array[MAX_DIRS];        // Buffer for all of the directories
//boost::array<DIRECTORY_INFO, MAX_DIRS> directory_info_array;

//DIRECTORY_INFO  directoryInfo;
//TCHAR           FileList[MAX_FILES*MAX_PATH];        // Buffer for all of the files

//DWORD           numDirs;	// TODO: global... hay que sacarla...




//----------------------------------------------------------------------------------------------------------------
#endif // BOOST_OS_SERVICES_WIN32_LEGACY_HPP
