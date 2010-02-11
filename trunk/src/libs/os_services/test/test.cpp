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


// TODO: BOOST: Nomenclaturas...

//TODO: boost::function_requires
//TODO: #include <boost/throw_exception.hpp>


//TODO:
	//BOOST_TRY{}
	//BOOST_CATCH(...){}
	//BOOST_CATCH_END


#include <iostream>
#include <string>

//#include <boost/asio.hpp>

#include <boost/os_services/file_system_monitor.hpp>

using namespace boost::os_services;


static void OnChanged(filesystem_event_args e) // object source,
{
	std::cout << "Changed: " << e.full_path << std::endl;
}

static void OnCreated(filesystem_event_args e) // object source,
{
	std::cout << "Created: " << e.full_path << std::endl;
}

static void OnDeleted(filesystem_event_args e) // object source,
{
	std::cout << "Deleted: " << e.full_path << std::endl;
}


static void OnRenamed(renamed_event_args e) // object source,
{
	std::cout << "File: " << e.old_full_path << " renamed to: " << e.full_path  << std::endl;
}


void simple_test()
{
	//std::string path1 = "C:\\temp1";
	std::string path1 = "C:\\temp1\\";
	//std::string path1 = "D:\\temp1";
	//std::string path1 = "J:\\temp1";	//Invalid dir test
	//std::string path1 = "/home/fernando/temp1";

	std::string path2 = "C:\\temp2";
	//std::string path2 = "D:\\temp2";
	//std::string path2 = "J:\\temp2";	//Invalid dir test
	//std::string path2 = "/home/fernando/temp2";


	boost::shared_ptr<file_system_monitor> monitor;

	try
	{
		monitor.reset(new file_system_monitor);

		monitor->add_directory(path1);
		monitor->add_directory(path2);

		//TODO: mapear los notify filters de Windows con otras plataformas...

		monitor->set_notify_filters( notify_filters::last_access | notify_filters::last_write | notify_filters::file_name | notify_filters::directory_name );
		monitor->set_filter("*.txt"); //TODO: implementar este filtro
		monitor->set_changed_event_handler(OnChanged);
		monitor->set_created_event_handler(OnCreated);
		monitor->set_deleted_event_handler(OnDeleted);
		monitor->set_renamed_event_handler(OnRenamed);

		monitor->start();
		//monitor->start(); //Probar de que no se pueda ejecutar dos veces.
		//monitor->stop(); //TODO: implementar
	}
	catch (std::runtime_error& e)
	{
		std::cout << "EXCEPTION: " << e.what() << std::endl;
	}
	catch (std::invalid_argument& e)
	{
		std::cout << "EXCEPTION: " << e.what() << std::endl;
	}

	std::cout << "Press Enter to Stop Monitoring..." << std::endl;
	std::cin.get();

	//delete monitor;

}


int main(int /*argc*/, char** /*argv*/)
{
	simple_test();

	std::cout << "Press Enter to Exit" << std::endl;

	//std::cin.clear();
	//std::cin.sync();
	//std::cin.ignore();
	std::cin.get();

	return 0;
}



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






//#include "FileSystemMonitor.hpp"
////#include "StaticOSSelector.hpp"
//
////#include "windows.h"
////#include "winuser.h"
//// c:\Program Files\Microsoft SDKs\Windows\v6.0A\Include\sdkddkver.h
//
//
////#include <boost/asio/detail/push_options.hpp>
////
////#include <boost/asio/detail/push_options.hpp>
////#include <boost/config.hpp>
////#include <boost/system/system_error.hpp>
////#include <boost/asio/detail/pop_options.hpp>
////
////#include <boost/asio/error.hpp>
////#include <boost/asio/detail/noncopyable.hpp>
////#include <boost/asio/detail/socket_types.hpp>
////
////#include <boost/asio/detail/push_options.hpp>
////#include <boost/throw_exception.hpp>
////#include <memory>
////#include <process.h>
////#include <boost/asio/detail/pop_options.hpp>
//
