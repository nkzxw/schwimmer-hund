//// TODO: LEER: URGENTE
//// http://msdn.microsoft.com/en-us/library/aa383745(VS.85).aspx
//// http://blogs.msdn.com/oldnewthing/archive/2007/04/11/2079137.aspx
//// http://msdn.microsoft.com/en-us/library/6sehtctf.aspx
//// http://stackoverflow.com/questions/142508/how-do-i-check-os-with-a-preprocessor-directive
//
//
//// WINDOWS VERSION AT RUNTIME
////		http://support.microsoft.com/kb/189249
////		http://msdn.microsoft.com/en-us/library/ms724439(VS.85).aspx
////		http://msdn.microsoft.com/en-us/library/ms724832(VS.85).aspx
////		http://msdn.microsoft.com/en-us/library/ms724451(VS.85).aspx
////      http://msdn.microsoft.com/en-us/library/ms725492(VS.85).aspx
////		http://msdn.microsoft.com/en-us/library/ms724833(VS.85).aspx
//
//
//// LINUX VERSION AT RUNTIME
////		http://linux.die.net/man/2/uname
////		cat /proc/sys/kernel/osrelease
//
//// FREEBSD VERSION AT RUNTIME
////		PROBABLEMENTE: http://linux.die.net/man/2/uname
//
//
//// TODO: BOOST: Nomenclaturas...
//
////TODO: boost::function_requires
////TODO: #include <boost/throw_exception.hpp>
//
//
////TODO:
//	//BOOST_TRY{}
//	//BOOST_CATCH(...){}
//	//BOOST_CATCH_END
//
//
//#include <iostream>
//#include <sstream>
//#include <string>
//
//#include <boost/filesystem.hpp>
//#include <boost/lexical_cast.hpp>
//
//#include <boost/os_services/file_system_monitor.hpp>
//
//
//
//using namespace boost::os_services;
//
//
//// Event Handlers
//static void OnChanged(filesystem_event_args e) // object source,
//{
//	std::cout << "Changed: '" << e.full_path << "'" << std::endl;
//}
//
//static void OnCreated(filesystem_event_args e) // object source,
//{
//	std::cout << "Created: '" << e.full_path << "'" << std::endl;
//}
//
//static void OnDeleted(filesystem_event_args e) // object source,
//{
//	std::cout << "Deleted: '" << e.full_path << "'" << std::endl;
//}
//
//
//static void OnRenamed(renamed_event_args e) // object source,
//{
//	std::cout << "File: '" << e.old_full_path << "' renamed to: '" << e.full_path  << "'" << std::endl;
//}
//
//
//
//
//
//#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
//
//std::string temp_path_1("/home/fernando/temp1/");
//std::string temp_path_2("/home/fernando/temp2/");
//
//#elif defined(__FreeBSD__) // || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
//
//std::string temp_path_1("/home/fernando/temp1/");
//std::string temp_path_2("/home/fernando/temp2/");
//
//#elif defined(__CYGWIN__)
//#  error Platform not supported
//#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
//
//std::string temp_path_1("C:\\temp1\\");
//std::string temp_path_2("C:\\temp2\\");
//
//#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
//#  error No test for the moment
//#else
//#  error Platform not supported
//#endif
//
//
//
//
//
//
//
//
//void test_with_boost_filesystem_path()
//{
//	boost::filesystem::path path1 ( temp_path_1, boost::filesystem::native );
//	boost::filesystem::path path2 ( temp_path_2, boost::filesystem::native );
//
//	boost::shared_ptr<file_system_monitor> monitor;
//
//	try
//	{
//		monitor.reset(new file_system_monitor);
//
//		monitor->add_watch( path1 );
//		monitor->add_watch( path2 );
//
//		//TODO: mapear los notify filters de Windows con otras plataformas...
//
//		monitor->set_notify_filters( notify_filters::last_access | notify_filters::last_write | notify_filters::file_name | notify_filters::directory_name );
//		monitor->set_filter("*.txt"); //TODO: implementar este filtro
//		monitor->set_changed_event_handler(OnChanged);
//		monitor->set_created_event_handler(OnCreated);
//		monitor->set_deleted_event_handler(OnDeleted);
//		monitor->set_renamed_event_handler(OnRenamed);
//
//		monitor->start();
//		//monitor->start(); //Probar de que no se pueda ejecutar dos veces.
//		//monitor->stop(); //TODO: implementar
//
//		std::cout << "Press Enter to Stop Monitoring..." << std::endl;
//		std::cin.get();
//
//	}
//	catch ( const std::runtime_error& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//	catch ( const std::invalid_argument& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//
//
//	//delete monitor;
//}
//
//
//
//void test_invalid_platform_path()
//{
//#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) || defined(__FreeBSD__) // || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
//	std::string invalid_path_1("C:\\temp1\\");
//	std::string invalid_path_2("C:\\temp2\\");
//#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
//	std::string invalid_path_1("/home/fernando/temp1/");
//	std::string invalid_path_2("/home/fernando/temp2/");
//#endif
//
//	boost::shared_ptr<file_system_monitor> monitor;
//
//	try
//	{
//		monitor.reset(new file_system_monitor);
//
//		//std::cout << "invalid_path_1: " << invalid_path_1 << std::endl;
//		//std::cout << "invalid_path_2: " << invalid_path_2 << std::endl;
//
//		monitor->add_watch( invalid_path_1 );
//		monitor->add_watch( invalid_path_2 );
//
//		monitor->set_notify_filters( notify_filters::last_access | notify_filters::last_write | notify_filters::file_name | notify_filters::directory_name );
//		monitor->set_filter("*.txt"); //TODO: implementar este filtro
//		monitor->set_changed_event_handler(OnChanged);
//		monitor->set_created_event_handler(OnCreated);
//		monitor->set_deleted_event_handler(OnDeleted);
//		monitor->set_renamed_event_handler(OnRenamed);
//
//		monitor->start();
//
//		std::cout << "Press Enter to Stop Monitoring..." << std::endl;
//		std::cin.get();
//
//	}
//	catch ( const std::runtime_error& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//	catch ( const std::invalid_argument& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//
//}
//
//
//void test_empty_string_path()
//{
//	std::string invalid_path_1("");
//	std::string invalid_path_2("");
//
//	boost::shared_ptr<file_system_monitor> monitor;
//
//	try
//	{
//		monitor.reset(new file_system_monitor);
//
//		monitor->add_watch( invalid_path_1 );
//		monitor->add_watch( invalid_path_2 );
//
//		monitor->set_notify_filters( notify_filters::last_access | notify_filters::last_write | notify_filters::file_name | notify_filters::directory_name );
//		monitor->set_filter("*.txt"); //TODO: implementar este filtro
//		monitor->set_changed_event_handler(OnChanged);
//		monitor->set_created_event_handler(OnCreated);
//		monitor->set_deleted_event_handler(OnDeleted);
//		monitor->set_renamed_event_handler(OnRenamed);
//
//		monitor->start();
//
//		std::cout << "Press Enter to Stop Monitoring..." << std::endl;
//		std::cin.get();
//
//	}
//	catch ( const std::runtime_error& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//	catch ( const std::invalid_argument& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//
//}
//
//
//void test_white_space_string_path()
//{
//	std::string invalid_path_1(" ");
//	std::string invalid_path_2(" ");
//
//	boost::shared_ptr<file_system_monitor> monitor;
//
//	try
//	{
//		monitor.reset(new file_system_monitor);
//
//		monitor->add_watch( invalid_path_1 );
//		monitor->add_watch( invalid_path_2 );
//
//		monitor->set_notify_filters( notify_filters::last_access | notify_filters::last_write | notify_filters::file_name | notify_filters::directory_name );
//		monitor->set_filter("*.txt"); //TODO: implementar este filtro
//		monitor->set_changed_event_handler(OnChanged);
//		monitor->set_created_event_handler(OnCreated);
//		monitor->set_deleted_event_handler(OnDeleted);
//		monitor->set_renamed_event_handler(OnRenamed);
//
//		monitor->start();
//
//		std::cout << "Press Enter to Stop Monitoring..." << std::endl;
//		std::cin.get();
//
//	}
//	catch ( const std::runtime_error& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//	catch ( const std::invalid_argument& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//
//}
//
//
//void test_invalid_directory()
//{
//#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) || defined(__FreeBSD__) // || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
//	std::string invalid_path_1("/home/pepe/temp1/");
//	std::string invalid_path_2("/home/pepe/temp2/");
//#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
//	std::string invalid_path_1("J:\\temp1\\");
//	std::string invalid_path_2("J:\\temp2\\");
//#endif
//
//	boost::shared_ptr<file_system_monitor> monitor;
//
//	try
//	{
//		monitor.reset(new file_system_monitor);
//
//		monitor->add_watch( invalid_path_1 );
//		monitor->add_watch( invalid_path_2 );
//
//		monitor->set_notify_filters( notify_filters::last_access | notify_filters::last_write | notify_filters::file_name | notify_filters::directory_name );
//		monitor->set_filter("*.txt"); //TODO: implementar este filtro
//		monitor->set_changed_event_handler(OnChanged);
//		monitor->set_created_event_handler(OnCreated);
//		monitor->set_deleted_event_handler(OnDeleted);
//		monitor->set_renamed_event_handler(OnRenamed);
//
//		monitor->start();
//
//		std::cout << "Press Enter to Stop Monitoring..." << std::endl;
//		std::cin.get();
//
//	}
//	catch ( const std::runtime_error& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//	catch ( const std::invalid_argument& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//
//}
//
//void test_slash_path_terminated()
//{
//#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) || defined(__FreeBSD__) // || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
//	std::string invalid_path_1("/home/fernando/temp1/");
//	std::string invalid_path_2("/home/fernando/temp2/");
//#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
//	std::string invalid_path_1("C:\\temp1\\");
//	std::string invalid_path_2("C:\\temp2\\");
//#endif
//
//	boost::shared_ptr<file_system_monitor> monitor;
//
//	try
//	{
//		monitor.reset(new file_system_monitor);
//
//		monitor->add_watch( invalid_path_1 );
//		monitor->add_watch( invalid_path_2 );
//
//		monitor->set_notify_filters( notify_filters::last_access | notify_filters::last_write | notify_filters::file_name | notify_filters::directory_name );
//		monitor->set_filter("*.txt"); //TODO: implementar este filtro
//		monitor->set_changed_event_handler(OnChanged);
//		monitor->set_created_event_handler(OnCreated);
//		monitor->set_deleted_event_handler(OnDeleted);
//		monitor->set_renamed_event_handler(OnRenamed);
//
//		monitor->start();
//
//		std::cout << "Press Enter to Stop Monitoring..." << std::endl;
//		std::cin.get();
//
//	}
//	catch ( const std::runtime_error& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//	catch ( const std::invalid_argument& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//
//}
//
//
//void test_non_slash_path_terminated()
//{
//#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) || defined(__FreeBSD__) // || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
//	std::string invalid_path_1("/home/fernando/temp1");
//	std::string invalid_path_2("/home/fernando/temp2");
//#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
//	std::string invalid_path_1("C:\\temp1");
//	std::string invalid_path_2("C:\\temp2");
//#endif
//
//	boost::shared_ptr<file_system_monitor> monitor;
//
//	try
//	{
//		monitor.reset(new file_system_monitor);
//
//		monitor->add_watch( invalid_path_1 );
//		monitor->add_watch( invalid_path_2 );
//
//		monitor->set_notify_filters( notify_filters::last_access | notify_filters::last_write | notify_filters::file_name | notify_filters::directory_name );
//		monitor->set_filter("*.txt"); //TODO: implementar este filtro
//		monitor->set_changed_event_handler(OnChanged);
//		monitor->set_created_event_handler(OnCreated);
//		monitor->set_deleted_event_handler(OnDeleted);
//		monitor->set_renamed_event_handler(OnRenamed);
//
//		monitor->start();
//
//		std::cout << "Press Enter to Stop Monitoring..." << std::endl;
//		std::cin.get();
//
//	}
//	catch ( const std::runtime_error& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//	catch ( const std::invalid_argument& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//}
//
//
//void test_two_start_execution()
//{
//#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) || defined(__FreeBSD__) // || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
//	std::string invalid_path_1("/home/fernando/temp1");
//	std::string invalid_path_2("/home/fernando/temp2");
//#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
//	std::string invalid_path_1("C:\\temp1");
//	std::string invalid_path_2("C:\\temp2");
//#endif
//
//	boost::shared_ptr<file_system_monitor> monitor;
//
//	try
//	{
//		monitor.reset(new file_system_monitor);
//
//		monitor->add_watch( invalid_path_1 );
//		monitor->add_watch( invalid_path_2 );
//
//		monitor->set_notify_filters( notify_filters::last_access | notify_filters::last_write | notify_filters::file_name | notify_filters::directory_name );
//		monitor->set_filter("*.txt"); //TODO: implementar este filtro
//		monitor->set_changed_event_handler(OnChanged);
//		monitor->set_created_event_handler(OnCreated);
//		monitor->set_deleted_event_handler(OnDeleted);
//		monitor->set_renamed_event_handler(OnRenamed);
//
//		monitor->start();
//		monitor->start();
//
//		std::cout << "Press Enter to Stop Monitoring..." << std::endl;
//		std::cin.get();
//
//	}
//	catch ( const std::runtime_error& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//	catch ( const std::invalid_argument& e )
//	{
//		std::cout << "EXCEPTION: " << e.what() << std::endl;
//	}
//}
//
//
////TODO: Test: monitor->start(); //monitor->start(); //Probar de que no se pueda ejecutar dos veces.
//
//void test_stress( int argc, char* argv[] )
//{
//	//TODO: levantar un thread que monitoree la aplicacion y otro thread que vaya modificando el FileSystem
//
//#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) || defined(__FreeBSD__) // || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
//const std::string default_dir = "/home/fernando/temp1/";
//#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
//const std::string default_dir = "C:\\temp1\\";
//#endif
//
//	const int default_max_files = 10000;
//
//	std::string file_name = "a";
//	std::string file_ext = ".txt";
//
//	std::string dir = default_dir;
//	int max_files = default_max_files;
//
//	if (argc > 1)
//	{
//		dir = argv[1];
//		//max_files = boost::lexical_cast<int>(argv[1]);
//	}
//
//	if (argc > 2)
//	{
//		//std::string temp = argv[2];
//		max_files = boost::lexical_cast<int>(argv[2]);
//	}
//
//
//	std::string source_file_path = dir + file_name + file_ext;
//
//	for (int i = 0; i<max_files; ++i)
//	{
//		std::stringstream ss;
//		ss << dir;
//		ss << file_name;
//		ss << i;
//		ss << file_ext;
//
//
//		//TODO: que no arroje error cuando el archivo existe...
//		//TODO: si el archivo existe, borrarlo...
//		boost::filesystem::copy_file( source_file_path, ss.str() );
//	}
//
//	std::cout << "DELETING..." << std::endl;
//
//	for (int i = 0; i<max_files; ++i)
//	{
//		std::stringstream ss;
//		ss << dir;
//		ss << file_name;
//		ss << i;
//		ss << file_ext;
//
//		boost::filesystem::remove(ss.str());
//	}
//
//}
//
//
//int main(int argc, char** argv)
//{
//	//TODO: boost test
//
//	//test_two_start_execution()	//TODO: testear
//	//test_white_space_string_path();
//	//test_empty_string_path();
//	//test_invalid_platform_path();
//	test_stress( argc, argv );
//	//test_with_boost_filesystem_path();
//
//	std::cout << "Press Enter to Exit" << std::endl;
//
//	//std::cin.clear();
//	//std::cin.sync();
//	//std::cin.ignore();
//	std::cin.get();
//
//	return 0;
//}
//
//
//
////_WIN32_WINNT
////std::cout << "BOOST_WINDOWS: '" << BOOST_WINDOWS << "'" << std::endl;
////std::cout << "_WIN32: '" << _WIN32 << "'" << std::endl;
//
////std::cout << "_WIN32_WINNT: '" << _WIN32_WINNT << "'" << std::endl;
////std::cout << "WINVER: '" << WINVER << "'" << std::endl;
//
////#if defined(WINVER)
////	std::cout << "_WIN32_WINNT: '" << _WIN32_WINNT << "'" << std::endl;
////	std::cout << "WINVER: '" << WINVER << "'" << std::endl;
////#endif
//
//
////std::cout << "__WIN32__: '" << __WIN32__ << "'" << std::endl;
////std::cout << "WIN32: '" << WIN32 << "'" << std::endl;
//
////#elif defined() || defined() || defined(WIN32)
//
//
//
//
//
//
////#include "FileSystemMonitor.hpp"
//////#include "StaticOSSelector.hpp"
////
//////#include "windows.h"
//////#include "winuser.h"
////// c:\Program Files\Microsoft SDKs\Windows\v6.0A\Include\sdkddkver.h
////
////
//////#include <boost/asio/detail/push_options.hpp>
//////
//////#include <boost/asio/detail/push_options.hpp>
//////#include <boost/config.hpp>
//////#include <boost/system/system_error.hpp>
//////#include <boost/asio/detail/pop_options.hpp>
//////
//////#include <boost/asio/error.hpp>
//////#include <boost/asio/detail/noncopyable.hpp>
//////#include <boost/asio/detail/socket_types.hpp>
//////
//////#include <boost/asio/detail/push_options.hpp>
//////#include <boost/throw_exception.hpp>
//////#include <memory>
//////#include <process.h>
//////#include <boost/asio/detail/pop_options.hpp>
////
//
//
//
//
//////------------------------------------------------------------------------------------------------------
////#include <vector>
////#include <utility>
////#include <list>
////#include <boost/bind.hpp>
////#include <boost/foreach.hpp>
////
////
//////template <typename PairType, typename T>
//////struct pair_first_equals : public std::binary_function<PairType, T, bool> 
//////{
//////	bool operator()(const PairType& p, const T& value) const 
//////	{
//////		return p.first == value;
//////	}
//////};
////
////
//////------------------------------------------------------------------------------------------------------
////
////typedef std::pair<std::string, boost::uint32_t> pair_type;
////typedef std::vector<pair_type> vector_type;
////
////vector_type vec;
////vec.push_back(std::make_pair("hola", 0));
////vec.push_back(std::make_pair("como", 1));
////vec.push_back(std::make_pair("estas", 2));
////
////vector_type::const_iterator it = std::find_if( vec.begin(), vec.end(), boost::bind( &pair_type::first, _1 ) == "como" );
////
////if ( it != vec.end() )
////{
////	std::cout << "ENCONTRO";
////}
////else
////{
////	std::cout << "NO ENCONTRO";
////}
////
//
//
////------------------------------------------------------------------------------------------------------
//
//
//
//
////------------------------------------------------------------------------------------------------------------------------------------------------------
//
////#include <map>
////#include <utility>
////#include <list>
////#include <boost/bimap.hpp>
////#include <boost/bimap/list_of.hpp>
////#include <boost/bimap/unordered_set_of.hpp>
////#include <boost/bimap/vector_of.hpp>
////#include <boost/foreach.hpp>
//
//
////struct Finder
////{
////	Finder(const std::string& text)
////		: text_(text)
////	{
////	}
////	//boost::bimaps::relation::structured_pair<FirstType,SecondType,Info,Layout>
////	//bool operator()( const std::string& test )
////
////	bool operator()( bmtype::left_value_type test )
////	{
////		return test.first == text_;
////	}
////
////	std::string text_;
////};
//
//////TODO: pasar todo esto a bose-katse (BOOST BIMAP)
//////------------------------------------------------------------------------------------------------------
//
//////typedef boost::bimap<std::string, boost::uint32_t> bmtype;
//////typedef boost::bimap< std::string, boost::bimaps::list_of<int> > bmtype;
//////typedef boost::bimap< std::string, boost::bimaps::unordered_set_of<int> > bmtype;
//
////bmtype bm;
////
//
//////bm.insert( bmtype::relation("hola", 0) );
//////bm.insert( bmtype::relation("pepe", 0) );
//
////bm.insert( bmtype::value_type("hola", 0) );
////bm.insert( bmtype::value_type("pepe", 0) );
//
//////for (bmtype::iterator it = bm.begin(); it != bm.end(); ++it)
////for (bmtype::left_iterator it = bm.left.begin(); it != bm.left.end(); ++it)
////{
////	(*it).second = 15;
////}
//
////   BOOST_FOREACH( bmtype::left_reference p, bm.left )
////   {
////       p.second = 15;
////   }
//
//////bmtype::left_iterator it = bm.left.find( "" ); // event->wd );
//////if ( it != bm.left.end() )
//////{
//////	//directory_name = it->second;
//////}
//////else
//////{
//////	//TODO: que pasa si no lo encontramos en la lista... DEBERIA SER UN RUN-TIME ERROR
//////}
//
//////bmtype::right_iterator it2 = bm.right.find( "" ); // event->wd );
//////if ( it != bm.right.end() )
//////{
//////	//directory_name = it->second;
//////}
//////else
//////{
//////	//TODO: que pasa si no lo encontramos en la lista... DEBERIA SER UN RUN-TIME ERROR
//////}
//
////
////// Try find_if.
//////bmtype::left_iterator it = std::find_if (bm.left.begin(), bm.left.begin(), std::bind1st (std::equal_to<bmtype::left_value_type>(), ""));
////bmtype::left_iterator it = std::find_if (bm.left.begin(), bm.left.begin(), Finder("puto") );
//
////if ( it != bm.left.end() )
////{
////	std::cout << "ENCONTRO";
////}
////else
////{
////	std::cout << "NO ENCONTRO";
////}
//
//
//////BOOST_FOREACH(watch_descriptors_type::right_reference p, watch_descriptors_.right)
//////{
//////	uint32_t watch_descriptor = 0;
//////	//uint32_t watch_descriptor = inotify_add_watch(fd_, p.first.c_str(), IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
//////	if (watch_descriptor == -1)
//////	{
//////		std::ostringstream oss;
//////		oss << "Failed to monitor directory - Directory: " << p.first << " - Reason:" << std::strerror(errno);
//////		throw (std::invalid_argument(oss.str()));
//////	}
//
//////	//p.first = ""; // = watch_descriptor;
//
//////	std::cout << typeid(p.first).name() << std::endl;
//////	std::cout << typeid(p.second).name() << std::endl;
//////}
//
//////------------------------------------------------------------------------------------------------------
