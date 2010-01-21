#include <iostream>

#include <boost/thread.hpp>

#include "DirMonitor.hpp"

std::string event_type_string(DirMonitor::event::event_type type)
{
    std::string event_type_str;

    switch (type)
    {
		case DirMonitor::event::created:
			return("created");
			break;
		case DirMonitor::event::deleted:
			return("deleted");
			break;
		case DirMonitor::event::moved_from:
			return("moved from");
			break;
		case DirMonitor::event::moved_to:
			return("moved to");
			break;
		default:
			return("invalid event type");
			break;
    };
}

void handle_modification(DirMonitor::event ev, boost::system::error_code e)
{
    if (!e)
    {
        std::cout << "File: " << ev.dir_name << "/" << ev.file_name << " - Action: " << event_type_string(ev.type) << std::endl;
    }
}


void test_async()
{
//    boost::asio::io_service io_service;
//    DirMonitor dirMonitor(io_service);

    DirMonitor dirMonitor;

    try
    {
//        dirMonitor.add_directory("/tmp");
        dirMonitor.add_directory("/home/fernando/temp1");

        //        dirMonitor.add_directory("/home");		//TODO: hacer esto provoca un error al hacer el remove del watch
//        dirMonitor.remove_directory("/home");


        DirMonitor::handler_type f;
        f = &handle_modification;
        dirMonitor.async_monitor(f);
        std::cout << "Monitoring directory /home/fernando/temp1 ..." << std::endl;


        std::cout << "press ENTER to finish" << std::endl;
        //io_service.run();
        std::cin.get();

//        std::cout << "begin to sleep" << std::endl;
//        //boost::this_thread::sleep(5000);
//
//        boost::system_time time = boost::get_system_time();
//        time += boost::posix_time::seconds(5);
//        boost::thread::sleep(time);


        std::cout << "end sleep" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
}



int main(int argc, char** argv)
{
	test_async();
	//test_sync();
}
