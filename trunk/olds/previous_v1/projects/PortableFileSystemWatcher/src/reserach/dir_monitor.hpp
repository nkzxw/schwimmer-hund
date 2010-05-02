///* 
// * File:   dir_monitor.hpp
// * Author: Andre Paim
// *
// * Created on August 22, 2008, 4:41 PM
// */
//
//#ifndef _DIR_MONITOR_HPP
//#define	_DIR_MONITOR_HPP
//
//#include <stdexcept>
//
//#include <sys/inotify.h>
//
//#include <boost/asio.hpp>
//#include <boost/bimap.hpp>
//#include <boost/bind.hpp>
//#include <boost/function.hpp>
//#include <boost/foreach.hpp>
//#include <boost/noncopyable.hpp>
//
//class dir_monitor : private boost::noncopyable
//{
//public:
//
//    dir_monitor(boost::asio::io_service& io_service);
//    ~dir_monitor();
//
//    // Add directory monitoration
//    void add_directory(const std::string& dir_name) throw (std::invalid_argument, std::runtime_error);
//    // Remove directory monitoration
//    void remove_directory(const std::string& dir_name) throw (std::invalid_argument);
//
//    // Monitor event structure
//    struct event
//    {
//        enum event_type 
//        {
//            created = IN_CREATE,
//            deleted = IN_DELETE,
//            moved_from = IN_MOVED_FROM,
//            moved_to = IN_MOVED_TO
//        };
//        std::string file_name;
//        std::string dir_name;
//        event_type type;
//    };
//
//    typedef boost::function<void (event, boost::system::error_code) > handler_type;
//    // Waits for monitoration events
//    void async_monitor(const handler_type handler);
//
//private:
//    // Reads data from inotify file descriptor 
//    void async_read();
//    // Parse events read
//    void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred);
//    
//
//    boost::asio::io_service& io_service_;
//    bool is_initialized_;
//    boost::asio::posix::stream_descriptor monitor_stream_descriptor_;
//    int fd_;
//    typedef boost::bimap<uint32_t, std::string> watch_descriptors_type;
//    watch_descriptors_type watch_descriptors_;
//    handler_type handler_;
//    enum
//    {
//        max_length = 1024
//    };
//    unsigned char buffer_[max_length];
//};
//
//
//#endif	/* _DIR_MONITOR_HPP */
//
