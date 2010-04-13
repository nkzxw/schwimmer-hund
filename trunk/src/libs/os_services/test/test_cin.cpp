//TODO: comentarios

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

static bool exit_thread = false;

void handle_thread()
{
	while ( !exit_thread )
	{
		std::cout << ".";

		boost::system_time time = boost::get_system_time();
		time += boost::posix_time::milliseconds(300);
		boost::thread::sleep(time);
	}
}

int main(int argc, char* argv[] )
{

	//std::cout << "Press Enter to Stop Monitoring... XXXXXX ......" << std::endl;
	//std::cin.sync();
	//std::cin.clear();
	////std::cin.get();
	//std::cin.peek();
	////std::cin.getline();
	////std::cin.good();
	////std::cin.sync_with_stdio();

	// -------------> OK

	//try
	//{

	//	std::cout << "Press Enter to Stop Monitoring... XXXXXX ......" << std::endl;
	//	std::cin.sync();
	//	std::cin.clear();
	//	//std::cin.get();
	//	std::cin.peek();
	//	//std::cin.getline();
	//	//std::cin.good();
	//	//std::cin.sync_with_stdio();

	//}
	//catch (std::runtime_error& e)
	//{
	//	std::cout << "EXCEPTION: " << e.what() << std::endl;
	//}
	//catch (std::invalid_argument& e)
	//{
	//	std::cout << "EXCEPTION: " << e.what() << std::endl;
	//}


	typedef boost::shared_ptr<boost::thread> HeapThread;

	HeapThread thread_;

	//thread_.reset( new boost::thread( boost::bind(&linux_impl::handle_directory_changes, this) ) );
	thread_.reset( new boost::thread( handle_thread ) );

	//if ( thread_ )
	//{
	//	thread_->join();
	//}

	std::cout << "Press Enter to Stop Monitoring... XXXXXX ......" << std::endl;
	std::cin.sync();
	std::cin.get();

	exit_thread = true;


	std::cout << "Press Enter to Exit" << std::endl;
	std::cin.sync();
	std::cin.get();

	return 0;
}
