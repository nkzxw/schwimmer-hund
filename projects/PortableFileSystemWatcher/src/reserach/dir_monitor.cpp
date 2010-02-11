//#include "dir_monitor.hpp"
//
//#include <sstream>
//
//#include <boost/filesystem.hpp>
//#include <boost/foreach.hpp>
//
//#include <errno.h>
//#include <string.h>
//
//dir_monitor::dir_monitor(boost::asio::io_service& io_service)
//    : io_service_(io_service), is_initialized_(false), monitor_stream_descriptor_(io_service)  
//{
//}
//
//dir_monitor::~dir_monitor()
//{
//    BOOST_FOREACH(watch_descriptors_type::left_reference p, watch_descriptors_.left)
//    {
//        inotify_rm_watch(fd_, p.first);
//    }
//}
//
//void dir_monitor::add_directory(const std::string& dir_name) throw (std::invalid_argument, std::runtime_error)
//{
//    if (!is_initialized_)
//    {
//        fd_ = inotify_init();
//        if (fd_ == -1)
//        {
//            std::ostringstream oss;
//            oss << "Failed to initialize monitor - Reason: " << std::strerror(errno);
//            throw (std::runtime_error(oss.str()));
//        }
//        is_initialized_ = true;
//    }
//
//    boost::filesystem::path directory(dir_name);
//    if (!boost::filesystem::is_directory(directory))
//    {
//        throw (std::invalid_argument(dir_name + " is not a valid directory entry"));
//    }
//    uint32_t watch_descriptor = inotify_add_watch(fd_, dir_name.c_str(), IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
//    if (watch_descriptor == -1)
//    {
//        std::ostringstream oss;
//        oss << "Failed to monitor directory - Directory: " << dir_name << " - Reason:" << std::strerror(errno);
//        throw (std::invalid_argument(oss.str()));
//    }
//    watch_descriptors_.insert(watch_descriptors_type::relation(watch_descriptor, dir_name));
//}
//
//void dir_monitor::remove_directory(const std::string& dir_name) throw (std::invalid_argument)
//{
//    watch_descriptors_type::right_iterator it = watch_descriptors_.right.find(dir_name);
//    if (it == watch_descriptors_.right.end())
//    {
//        throw (std::invalid_argument("Directory " + dir_name + " is not being monitored"));
//    }
//    int retcod = inotify_rm_watch(fd_, it->second);
//    if (retcod == -1)
//    {
//        std::ostringstream oss;
//        oss << "Failed to remove directory monotaration - Directory: " << dir_name << " - Reason:" << std::strerror(errno);
//        throw (std::invalid_argument(oss.str()));
//    }
//}
//
//void dir_monitor::async_monitor(const handler_type handler)
//{
//    handler_ = handler;
//    monitor_stream_descriptor_.assign(fd_);
//    async_read();
//}
//
//void dir_monitor::async_read()
//{
//    monitor_stream_descriptor_.async_read_some(boost::asio::buffer(buffer_, max_length),
//                                               boost::bind(&dir_monitor::handle_read, this,
//                                                           boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
//}
//
//void dir_monitor::handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
//{
//    if (!e)
//    {
//        struct inotify_event* inotify_event;
//        unsigned int bytes_processed = 0;
//        if (bytes_transferred > sizeof (struct inotify_event))
//        {
//            do
//            {
//                inotify_event = reinterpret_cast<struct inotify_event*> (buffer_ + bytes_processed);
//                event ev;
//                ev.file_name = inotify_event->name;
//                watch_descriptors_type::left_iterator it = watch_descriptors_.left.find(inotify_event->wd);
//                if (it != watch_descriptors_.left.end())
//                    ev.dir_name = it->second;
//                ev.type = static_cast<event::event_type> (inotify_event->mask);
//                handler_(ev, e);
//                bytes_processed += sizeof (struct inotify_event) + inotify_event->len;
//            }
//            while (bytes_transferred - bytes_processed >= sizeof (struct inotify_event) + inotify_event->len);
//        }
//        async_read();
//    }
//    else
//    {
//        handler_(event(), e);
//    }
//}
