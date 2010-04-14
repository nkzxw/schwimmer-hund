//TODO: comentarios

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include <sys/inotify.h>
#include <sys/types.h>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

static bool exit_thread = false;



#define EVENT_SIZE  ( sizeof (struct inotify_event) )
#define BUF_LEN     ( 1024 * ( EVENT_SIZE + 16 ) )

int file_descriptor_; 
char buffer[BUF_LEN];




void handle_thread()
{
	while ( !exit_thread )
	{
		std::cout << ".";
		std::cout.flush();

		boost::system_time time = boost::get_system_time();
		time += boost::posix_time::milliseconds(300);
		boost::thread::sleep(time);
	}
}

void handle_thread_inotify()
{
	while ( !exit_thread )
	{
		std::cout << ".";
		std::cout.flush();

		int length = ::read( file_descriptor_, buffer, BUF_LEN );
		std::cout << "length: " << length << std::endl;

		boost::system_time time = boost::get_system_time();
		time += boost::posix_time::milliseconds(300);
		boost::thread::sleep(time);
	}
}

int main(int argc, char* argv[] )
{


	// ------------------------------------------------------------
	// Test1: OK
	// ------------------------------------------------------------

	//std::cout << "Press Enter to Stop Monitoring... XXXXXX ......" << std::endl;
	//std::cin.sync();
	//std::cin.clear();
	////std::cin.get();
	//std::cin.peek();
	////std::cin.getline();
	////std::cin.good();
	////std::cin.sync_with_stdio();

	// ------------------------------------------------------------





	// ------------------------------------------------------------
	// Test2: OK
	// ------------------------------------------------------------

	//try
	//{

	//	std::cout << "Press Enter to Stop Monitoring... XXXXXX ......" << std::endl;
	//	std::cin.sync();
	//	std::cin.get();

	//}
	//catch (std::runtime_error& e)
	//{
	//	std::cout << "EXCEPTION: " << e.what() << std::endl;
	//}
	//catch (std::invalid_argument& e)
	//{
	//	std::cout << "EXCEPTION: " << e.what() << std::endl;
	//}

	// ------------------------------------------------------------

	// ------------------------------------------------------------
	// Test3: OK
	// ------------------------------------------------------------

	//typedef boost::shared_ptr<boost::thread> ThreadType;

	//ThreadType thread_;

	////thread_.reset( new boost::thread( boost::bind(&linux_impl::handle_directory_changes, this) ) );
	//thread_.reset( new boost::thread( handle_thread ) );

	////if ( thread_ )
	////{
	////	thread_->join();
	////}

	//std::cout << "Press Enter to Stop Monitoring... XXXXXX ......" << std::endl;
	//std::cin.sync();
	//std::cin.get();

	//exit_thread = true;



	// ------------------------------------------------------------


	// ------------------------------------------------------------
	// Test4:
	// ------------------------------------------------------------

	file_descriptor_ = ::inotify_init();
	std::cout << "file_descriptor_: " << file_descriptor_ << std::endl;
	boost::uint32_t watch_descriptor = ::inotify_add_watch(file_descriptor_, "~/temp1", IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO);
	std::cout << "watch_descriptor: " << watch_descriptor << std::endl;


	typedef boost::shared_ptr<boost::thread> ThreadType;

	ThreadType thread_;

	//thread_.reset( new boost::thread( boost::bind(&linux_impl::handle_directory_changes, this) ) );
	thread_.reset( new boost::thread( handle_thread_inotify ) );




	std::cout << "Press Enter to Stop Monitoring... XXXXXX ......" << std::endl;
	std::cin.sync();
	std::cin.get();


	// ------------------------------------------------------------



	std::cout << "Press Enter to Exit" << std::endl;
	std::cin.sync();
	std::cin.get();

	return 0;
}