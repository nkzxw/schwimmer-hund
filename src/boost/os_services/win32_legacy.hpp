#ifndef BOOST_OS_SERVICES_WIN32_LEGACY_HPP
#define BOOST_OS_SERVICES_WIN32_LEGACY_HPP

//----------------------------------------------------------------------------------------------------------------
// TODO: Legacy-Code, me gustaría reemplazarlo por código C++
// WIN32

#define MAX_BUFFER  16384	//8192	//4096

typedef struct _DIRECTORY_INFO 
{
	HANDLE      directoryHandle;
	TCHAR       directoryName[MAX_PATH];
	CHAR        buffer[MAX_BUFFER];
	DWORD       bufferLength;
	OVERLAPPED  overlapped;
} DIRECTORY_INFO, *PDIRECTORY_INFO, *LPDIRECTORY_INFO;

//DIRECTORY_INFO  directoryInfo[MAX_DIRS];        // Buffer for all of the directories
DIRECTORY_INFO  directoryInfo;        // Buffer for all of the directories
//TCHAR           FileList[MAX_FILES*MAX_PATH];   // Buffer for all of the files

DWORD           numDirs;	// TODO: global... hay que sacarla...




//----------------------------------------------------------------------------------------------------------------
#endif // BOOST_OS_SERVICES_WIN32_LEGACY_HPP
