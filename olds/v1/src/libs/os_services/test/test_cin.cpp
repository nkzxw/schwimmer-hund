////TODO: comentarios
//
//#include <fstream>
//#include <iostream>
//#include <ostream>
//#include <sstream>
//#include <string>
//
//#include <sys/inotify.h>
//#include <sys/types.h>
//
//#include <boost/bind.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/thread.hpp>
//
//
////-----------------------------------------------------
//
//#include <boost/bimap.hpp>
//#include <boost/filesystem/path.hpp>
//#include <boost/noncopyable.hpp>
//
//#include <boost/os_services/detail/impl_selector.hpp>
//#include <boost/os_services/detail/utils.hpp>
//
//#include <string>
//
//#include <boost/filesystem/path.hpp>
//
//#include <boost/os_services/detail/utils.hpp>
//#include <boost/os_services/event_args.hpp>
//#include <boost/os_services/event_handlers.hpp>
//
//#include <string>
//
//// C-Std Headers
//#include <cerrno>
//#include <cstdio>
//#include <cstdlib>
//#include <cstring>		// for strerror
//
//#include <sys/inotify.h>
//#include <sys/types.h>
//
////#include <boost/bimap.hpp>
////#include <boost/bimap/list_of.hpp>
//#include <boost/bind.hpp>
//#include <boost/foreach.hpp>
//#include <boost/integer.hpp>
//#include <boost/smart_ptr.hpp>
//#include <boost/thread.hpp>
//
//#include <boost/os_services/change_types.hpp>
//#include <boost/os_services/detail/base_impl.hpp>
//#include <boost/os_services/notify_filters.hpp>
//
//
////-----------------------------------------------------
//
//static bool exit_thread = false;
//
//
//
//#define EVENT_SIZE  ( sizeof (struct inotify_event) )
//#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )
//
//int file_descriptor_; 
//
//
//
//
//
//void handle_thread()
//{
//	while ( !exit_thread )
//	{
//		std::cout << ".";
//		std::cout.flush();
//
//		boost::system_time time = boost::get_system_time();
//		time += boost::posix_time::milliseconds(300);
//		boost::thread::sleep(time);
//	}
//}
//
////void handle_thread_inotify()
////{
////	std::cout << "5" << std::endl;
////	std::cin.sync();
////	std::cin.get();
////
////	while ( !exit_thread )
////	{
////		char buffer[BUF_LEN];
////
////		std::cout << "6" << std::endl;
////		std::cin.sync();
////		std::cin.get();
////
////		std::cout << ".";
////		std::cout.flush();
////
////		int length = ::read( file_descriptor_, buffer, BUF_LEN );
////		std::cout << "length: " << length << std::endl;
////
////		std::cout << "7" << std::endl;
////		std::cin.sync();
////		std::cin.get();
////
////		boost::system_time time = boost::get_system_time();
////		time += boost::posix_time::milliseconds(100);
////		boost::thread::sleep(time);
////	}
////}
//
////TODO: TESTS
////        1. poner el thread dentro de una clase
////						thread_.reset( new boost::thread( boost::bind(&XXX_impl::handle_directory_changes, this) ) );
////        2. libraries included
//
//
//
//
//
//
//
//
//class XXX_impl : public boost::os_services::detail::base_impl<XXX_impl>
//{
//public:
//
//	//void start()
//	//{
//	//	thread_.reset( new boost::thread( boost::bind(&XXX_impl::handle_directory_changes, this) ) );
//	//}
//
//public: 
//	void handle_directory_changes()
//	{
//		std::cout << "B5" << std::endl;
//		std::cin.sync();
//		std::cin.get();
//
//		while ( !exit_thread )
//		{
//			char buffer[BUF_LEN];
//
//			std::cout << "B6" << std::endl;
//			std::cin.sync();
//			std::cin.get();
//
//			std::cout << "B.";
//			std::cout.flush();
//
//			int length = ::read( file_descriptor_, buffer, BUF_LEN );
//			std::cout << "Blength: " << length << std::endl;
//
//			std::cout << "B7" << std::endl;
//			std::cin.sync();
//			std::cin.get();
//
//			boost::system_time time = boost::get_system_time();
//			time += boost::posix_time::milliseconds(100);
//			boost::thread::sleep(time);
//		}
//	}
//};
//
//
//
//int main(int argc, char* argv[] )
//{
//
//
//	// ------------------------------------------------------------
//	// Test1: OK
//	// ------------------------------------------------------------
//
//	//std::cout << "Press Enter to Stop Monitoring... XXXXXX ......" << std::endl;
//	//std::cin.sync();
//	//std::cin.clear();
//	////std::cin.get();
//	//std::cin.peek();
//	////std::cin.getline();
//	////std::cin.good();
//	////std::cin.sync_with_stdio();
//
//	// ------------------------------------------------------------
//
//
//
//
//
//	// ------------------------------------------------------------
//	// Test2: OK
//	// ------------------------------------------------------------
//
//	//try
//	//{
//
//	//	std::cout << "Press Enter to Stop Monitoring... XXXXXX ......" << std::endl;
//	//	std::cin.sync();
//	//	std::cin.get();
//
//	//}
//	//catch ( const std::runtime_error& e )
//	//{
//	//	std::cout << "EXCEPTION: " << e.what() << std::endl;
//	//}
//	//catch ( const std::invalid_argument& e )
//	//{
//	//	std::cout << "EXCEPTION: " << e.what() << std::endl;
//	//}
//
//	// ------------------------------------------------------------
//
//	// ------------------------------------------------------------
//	// Test3: OK
//	// ------------------------------------------------------------
//
//	//typedef boost::shared_ptr<boost::thread> thread_type;
//
//	//thread_type thread_;
//
//	////thread_.reset( new boost::thread( boost::bind(&XXX_impl::handle_directory_changes, this) ) );
//	//thread_.reset( new boost::thread( handle_thread ) );
//
//	////if ( thread_ )
//	////{
//	////	thread_->join();
//	////}
//
//	//std::cout << "Press Enter to Stop Monitoring... XXXXXX ......" << std::endl;
//	//std::cin.sync();
//	//std::cin.get();
//
//	//exit_thread = true;
//
//
//
//	// ------------------------------------------------------------
//
//
//	// ------------------------------------------------------------
//	// Test4:
//	// ------------------------------------------------------------
//
//	std::string dir = "/home/fernando/temp1";
//	std::cout << "1" << std::endl;
//	std::cin.sync();
//	std::cin.get();
//
//	file_descriptor_ = 0;
//	file_descriptor_ = ::inotify_init();
//
//	std::cout << "2" << std::endl;
//	std::cin.sync();
//	std::cin.get();
//
//	std::cout << "dir: '" << dir << "'" << std::endl;
//	std::cout << "file_descriptor_: " << file_descriptor_ << std::endl;
//	boost::uint32_t watch_descriptor = ::inotify_add_watch(file_descriptor_, dir.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
//	std::cout << "watch_descriptor: " << watch_descriptor << std::endl;
//
//	std::cout << "3" << std::endl;
//	std::cin.sync();
//	std::cin.get();
//
//	typedef boost::shared_ptr<boost::thread> thread_type;
//
//	thread_type thread_;
//
//	//thread_.reset( new boost::thread( handle_thread_inotify ) );
//
//	XXX_impl* impl = new XXX_impl;
//	//thread_.reset( new boost::thread( boost::bind(&XXX_impl::handle_directory_changes, this) ) );
//	thread_.reset( new boost::thread( boost::bind(&XXX_impl::handle_directory_changes, impl) ) );
//
//	std::cout << "4" << std::endl;
//	std::cin.sync();
//	std::cin.get();
//
//	std::cout << "Press Enter to Stop Monitoring..." << std::endl;
//	std::cin.sync();
//	std::cin.get();
//
//	// ------------------------------------------------------------
//
//	std::cout << "Press Enter to Exit" << std::endl;
//	std::cin.sync();
//	std::cin.get();
//
//	return 0;
//}
