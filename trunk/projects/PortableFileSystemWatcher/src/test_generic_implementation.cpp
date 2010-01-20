// TODO: LEER: URGENTE
// http://msdn.microsoft.com/en-us/library/aa383745(VS.85).aspx
// http://blogs.msdn.com/oldnewthing/archive/2007/04/11/2079137.aspx
// http://msdn.microsoft.com/en-us/library/6sehtctf.aspx
// http://stackoverflow.com/questions/142508/how-do-i-check-os-with-a-preprocessor-directive


// WINDOWS VERSION AT RUNTIME
//		http://support.microsoft.com/kb/189249
//		http://msdn.microsoft.com/en-us/library/ms724439(VS.85).aspx
//		http://msdn.microsoft.com/en-us/library/ms724832(VS.85).aspx
//		http://msdn.microsoft.com/en-us/library/ms724451(VS.85).aspx
//      http://msdn.microsoft.com/en-us/library/ms725492(VS.85).aspx
//		http://msdn.microsoft.com/en-us/library/ms724833(VS.85).aspx


// LINUX VERSION AT RUNTIME
//		http://linux.die.net/man/2/uname
//		cat /proc/sys/kernel/osrelease

// FREEBSD VERSION AT RUNTIME
//		PROBABLEMENTE: http://linux.die.net/man/2/uname




#include <iostream>
#include <string>



//#include "FileSystemMonitorImplTestGeneric.hpp"
#include "FileSystemMonitorGeneric.hpp"
#include "StaticOSSelector.hpp"

//#include "windows.h"
//#include "winuser.h"
// c:\Program Files\Microsoft SDKs\Windows\v6.0A\Include\sdkddkver.h


//#include <boost/asio/detail/push_options.hpp>
//
//#include <boost/asio/detail/push_options.hpp>
//#include <boost/config.hpp>
//#include <boost/system/system_error.hpp>
//#include <boost/asio/detail/pop_options.hpp>
//
//#include <boost/asio/error.hpp>
//#include <boost/asio/detail/noncopyable.hpp>
//#include <boost/asio/detail/socket_types.hpp>
//
//#include <boost/asio/detail/push_options.hpp>
//#include <boost/throw_exception.hpp>
//#include <memory>
//#include <process.h>
//#include <boost/asio/detail/pop_options.hpp>



static void OnChanged(FileSystemEventArgs e) // object source, 
{
	std::cout << "Changed: " << e.Name << std::endl;

	// Specify what is done when a file is changed, created, or deleted.
	//Console.WriteLine("File: " + e.FullPath + " " + e.ChangeType);
}

static void OnCreated(FileSystemEventArgs e) // object source, 
{
	std::cout << "Created: " << e.Name << std::endl;

	// Specify what is done when a file is changed, created, or deleted.
	//Console.WriteLine("File: " + e.FullPath + " " + e.ChangeType);
}

static void OnDeleted(FileSystemEventArgs e) // object source, 
{
	std::cout << "Deleted: " << e.Name << std::endl;

	// Specify what is done when a file is changed, created, or deleted.
	//Console.WriteLine("File: " + e.FullPath + " " + e.ChangeType);
}

static void OnRenamed(RenamedEventArgs e) // object source, 
{
	std::cout << "Renamed: " << e.Name << std::endl;
	// Specify what is done when a file is renamed.
	//Console.WriteLine("File: {0} renamed to {1}", e.OldFullPath, e.FullPath);
}




int main(int argc, char** argv)
{
	std::string path = "/home/fernando";
	//	std::string path = "D:\\temp1";			// Da error en Linux.
	//	std::string path = "C:\\temp1";

	FileSystemMonitorGeneric* monitor;


	try
	{
		monitor = new FileSystemMonitorGeneric(path);

		monitor->setNotifyFilters( NotifyFilters::LastAccess | NotifyFilters::LastWrite | NotifyFilters::FileName | NotifyFilters::DirectoryName );
		monitor->setFilter("*.txt"); //TODO: implementar este filtro
		monitor->setChangedEventHandler(OnChanged);
		monitor->setCreatedEventHandler(OnCreated);
		monitor->setDeletedEventHandler(OnDeleted);
		monitor->setRenamedEventHandler(OnRenamed);

		//monitor->setEnableRaisingEvents(true); //TODO: cambiar, no est� bueno este dise�o. Crear un m�todo Start.
		monitor->startMonitoring();
		//monitor->startMonitoring();	//TODO: esto crearia otro Thread... si lo implementamos como un Setter "EnableRaisingEvents" podemos manejarlo de otra manera...

	}
	catch (std::runtime_error re)
	{
		std::cout << "EXCEPTION" << std::endl;
		std::cout << re.what() << std::endl;
	}


	std::cin.clear();
	std::cin.sync();
	std::cin.get();

	std::cout << "DELETING" << std::endl;

	std::cout << "monitor: '" << monitor << "'" << std::endl;

//	delete monitor;	//TODO: este delete, en caso de error en el constructor da error...






	//StaticOSSelector::type oss;
	//oss.start();

	//FileSystemMonitorGenericImpl<PLATFORM_IMPL_CLASS> monitor;
	//FileSystemMonitorGenericImpl<PlatformImpl> monitor;
	//monitor.start();




	//_WIN32_WINNT
	//std::cout << "BOOST_WINDOWS: '" << BOOST_WINDOWS << "'" << std::endl;
	//std::cout << "_WIN32: '" << _WIN32 << "'" << std::endl;

	//std::cout << "_WIN32_WINNT: '" << _WIN32_WINNT << "'" << std::endl;
	//std::cout << "WINVER: '" << WINVER << "'" << std::endl;

//#if defined(WINVER)
//	std::cout << "_WIN32_WINNT: '" << _WIN32_WINNT << "'" << std::endl;
//	std::cout << "WINVER: '" << WINVER << "'" << std::endl;
//#endif


	//std::cout << "__WIN32__: '" << __WIN32__ << "'" << std::endl;
	//std::cout << "WIN32: '" << WIN32 << "'" << std::endl;

	//#elif defined() || defined() || defined(WIN32)
	
	std::cin.get();
	return 0;
}
