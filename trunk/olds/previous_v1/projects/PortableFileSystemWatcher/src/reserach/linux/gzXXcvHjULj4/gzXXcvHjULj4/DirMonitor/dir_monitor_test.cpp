#include "dir_monitor.hpp"

std::string event_type_string(dir_monitor::event::event_type type)
{
    std::string event_type_str;
    switch (type)
    {
    case dir_monitor::event::created:
        return("created");
        break;
    case dir_monitor::event::deleted:
        return("deleted");
        break;
    case dir_monitor::event::moved_from:
        return("moved from");
        break;
    case dir_monitor::event::moved_to:
        return("moved to");
        break;
    default:
        return("invalid event type");
        break;
    };
}

void handle_modification(dir_monitor::event ev, boost::system::error_code e)
{
    if (!e)
    {
        std::cout << "File: " << ev.dir_name << "/" << ev.file_name << " - Action: " << event_type_string(ev.type) << std::endl;
    }
}

int main()
{
    boost::asio::io_service io_service;
    dir_monitor dir_monitor(io_service);
    try
    {
        dir_monitor.add_directory("/tmp");
        dir_monitor.add_directory("/home");
        dir_monitor.remove_directory("/home");
                

        dir_monitor::handler_type f;
        f = &handle_modification;
        dir_monitor.async_monitor(f);
        std::cout << "Monitoring directory /tmp..." << std::endl;

        io_service.run();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

}
