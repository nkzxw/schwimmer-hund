// http://www.ureader.com/msg/1474286.aspx

#include <cstdio>

#include "windows.h"


typedef struct MyOVERLAPPED 
{
	OVERLAPPED overlapped;
	int data;	//era un puntero
} MyOVERLAPPED;


VOID CALLBACK MyCallback(DWORD errorCode, DWORD tferred, LPOVERLAPPED over)
{
	printf("Notify...");

	MyOVERLAPPED *overlapped = (MyOVERLAPPED *) over;
	int data = overlapped->data;
	free(over);
	// etc.
}


int main()
{
	//open the directory to watch....
	//HANDLE hDir = CreateFile("C:\\temp1", FILE_LIST_DIRECTORY | GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
	HANDLE hDir = CreateFile("C:\\temp1", FILE_LIST_DIRECTORY,  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
	if( hDir == INVALID_HANDLE_VALUE )
	{
		DWORD dwError = GetLastError();
		return dwError;
	}


	//HANDLE completion_port_handle_;
	//completion_port_handle_ = CreateIoCompletionPort( hDir, completion_port_handle_, (DWORD) watch.get(), 0 );


	//OVERLAPPED overlapped;
	//overlapped.Internal = overlapped.InternalHigh = overlapped.Offset = overlapped.OffsetHigh = 0;
	//overlapped.hEvent = NULL;


	MyOVERLAPPED* overlapped = (MyOVERLAPPED*) malloc(sizeof(*overlapped));
	memset(overlapped, 0, sizeof(*overlapped));
	overlapped->data = 1;
	//ReadDirectoryChangesW(/* ... */, &overlapped->overlapped, MyCallback);



	DWORD byteRerurned = 0;
	DWORD dwRecSize = sizeof(FILE_NOTIFY_INFORMATION) + MAX_PATH * 2;
	DWORD dwCount = 32;
	
	FILE_NOTIFY_INFORMATION* pNI = (FILE_NOTIFY_INFORMATION*)malloc(dwRecSize * dwCount);


	
	
	//LPCOMPETION cb = ChDirCompletionRoutine;

//	if ( !ReadDirectoryChangesW( hDir, pNI, dwRecSize * dwCount, FALSE, FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE   | FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME, &byteRerurned, &overlapped, &MyCallback) )
//	if ( !ReadDirectoryChangesW( hDir, pNI, dwRecSize * dwCount, FALSE, FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_LAST_WRITE   | FILE_NOTIFY_CHANGE_LAST_ACCESS | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_FILE_NAME, &byteRerurned, &overlapped->overlapped, MyCallback) )
	if ( !ReadDirectoryChangesW( hDir, pNI, dwRecSize * dwCount, TRUE, 51, &byteRerurned, &overlapped->overlapped, MyCallback) )
	{
		DWORD dw = GetLastError();
	}

	//do {
	//	Sleep(3000);
	//}
	//while (1);

	SleepEx( INFINITE, TRUE );


	return 0;
}



//--------------------------------------------------------------------------------------------------------
//
//int main( int /*argc*/, char** /*argv*/ )
//{
//	return 0;
//}

