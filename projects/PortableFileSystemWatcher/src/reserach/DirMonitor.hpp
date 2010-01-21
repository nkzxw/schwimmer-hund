// http://mostlycoding.blogspot.com/2009/05/asio-library-has-been-immensely-helpful.html


/* 
 * File:   DirMonitor.hpp
 * Author: 
 *
 * Created on August 22, 2008, 4:41 PM
 */

#ifndef _DIRMONITOR_HPP
#define	_DIRMONITOR_HPP

#include <sstream>
#include <stdexcept>
#include <string>  //#include <string.h>

#include <errno.h>

#include <sys/inotify.h>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bimap.hpp>
#include <boost/bind.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/noncopyable.hpp>

#include <boost/thread.hpp>

typedef boost::shared_ptr<boost::thread> HeapThread;


class DirMonitor : private boost::noncopyable
{
public:

//    DirMonitor(boost::asio::io_service& io_service)
//		: io_service_(io_service), is_initialized_(false), monitor_stream_descriptor_(io_service)
//	{}


//    DirMonitor()
//		: work_( new boost::asio::io_service::work(io_service_) ), is_initialized_(false), monitor_stream_descriptor_(io_service_)

	DirMonitor()
		: is_initialized_(false), monitor_stream_descriptor_(io_service_)
	{
//		io_service_.run();
//		io_service_.stop();

        work_.reset( new boost::asio::io_service::work(io_service_) );
//        threadGroup_.create_thread( boost::bind(&boost::asio::io_service::run, &m_io_service) );
//		boost::thread thread_( boost::bind(&boost::asio::io_service::run, &io_service_) );

		thread_.reset( new boost::thread( boost::bind(&boost::asio::io_service::run, &io_service_) ) );
	}


    ~DirMonitor()
	{
    	std::cout << "~DirMonitor()" << std::endl;

		BOOST_FOREACH(watch_descriptors_type::left_reference p, watch_descriptors_.left)
		{

			std::cout << "fileDescriptor_: " << fileDescriptor_ << std::endl;
			std::cout << "Watch Descriptor p.first: " << p.first << std::endl;
			std::cout << "p.second: " << p.second << std::endl;

			if ( p.first != 0)
			{
				int retError = ::inotify_rm_watch(fileDescriptor_, p.first);

				std::cout << "retError: " << retError << std::endl;

				if ( retError != 0 ) // == -1 )
				{
					std::ostringstream oss;
					oss << "Failed to remove watch - Reason:" << strerror(errno);
					throw (std::invalid_argument(oss.str()));
				}
			}

		}

    	monitor_stream_descriptor_.cancel();

    	std::cout << "0" << std::endl;

		if ( fileDescriptor_ != 0 )
		{
			// TODO: parece que close(0) cierra el standard input (CIN)
//			printf("closing file descriptor...\n");
			int retClose =  close( fileDescriptor_ );
//			printf("retClose: %d\n", retClose);

			std::cout << "fileDescriptor_ closed....." << std::endl;
		}

    	std::cout << "1" << std::endl;


		work_.reset();	// ioservice_.stop();
//    	io_service_.stop();


    	std::cout << "2" << std::endl;

		if ( thread_ )
		{
	    	std::cout << "3" << std::endl;

			thread_->join();
		}

    	std::cout << "4" << std::endl;



	}

    // Add directory monitoration
    void add_directory(const std::string& dir_name) throw (std::invalid_argument, std::runtime_error)
	{
		if (!is_initialized_)
		{
			fileDescriptor_ = ::inotify_init();
			if (fileDescriptor_ == -1)
			{
				std::ostringstream oss;
				oss << "Failed to initialize monitor - Reason: " << strerror(errno);
				throw (std::runtime_error(oss.str()));
			}
			is_initialized_ = true;

			std::cout << "CREATION fileDescriptor_: " << fileDescriptor_ << std::endl;

		}

		boost::filesystem::path directory(dir_name);
		if (!boost::filesystem::is_directory(directory))
		{
			throw (std::invalid_argument(dir_name + " is not a valid directory entry"));
		}

		//uint32_t watch_descriptor = inotify_add_watch(fileDescriptor_, dir_name.c_str(), IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
		int watch_descriptor = ::inotify_add_watch(fileDescriptor_, dir_name.c_str(), IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
		if ( watch_descriptor == -1 )
		{
			std::ostringstream oss;
			oss << "Failed to monitor directory - Directory: " << dir_name << " - Reason:" << strerror(errno);
			throw (std::invalid_argument(oss.str()));
		}

		std::cout << "watch_descriptor: " << watch_descriptor << std::endl;


		watch_descriptors_.insert(watch_descriptors_type::relation(watch_descriptor, dir_name));
	}


    // Remove directory monitoration
    void remove_directory(const std::string& dir_name) throw (std::invalid_argument)
	{
		watch_descriptors_type::right_iterator it = watch_descriptors_.right.find(dir_name);
		if (it == watch_descriptors_.right.end())
		{
			throw (std::invalid_argument("Directory " + dir_name + " is not being monitored"));
		}

		int retcod = ::inotify_rm_watch(fileDescriptor_, it->second);
		if (retcod == -1)
		{
			std::ostringstream oss;
			oss << "Failed to remove directory monotaration - Directory: " << dir_name << " - Reason:" << strerror(errno);
			throw (std::invalid_argument(oss.str()));
		}
	}

    // Monitor event structure
    struct event
    {
        enum event_type 
        {
            created = IN_CREATE,
            deleted = IN_DELETE,
            moved_from = IN_MOVED_FROM,
            moved_to = IN_MOVED_TO
        };
        std::string file_name;
        std::string dir_name;
        event_type type;
    };

    typedef boost::function<void (event, boost::system::error_code) > handler_type;

	
	
	// Waits for monitoration events
    void async_monitor(const handler_type handler)
	{
		handler_ = handler;
		monitor_stream_descriptor_.assign(fileDescriptor_);
		async_read();
	}



private:
    // Reads data from inotify file descriptor 
    void async_read()
	{
		monitor_stream_descriptor_.async_read_some(boost::asio::buffer(buffer_, max_length), boost::bind(&DirMonitor::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		//monitor_stream_descriptor_.async_read_some(boost::asio::buffer(buffer_), boost::bind(&DirMonitor::handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}


    // Parse events read
    void handle_read(const boost::system::error_code& e, std::size_t bytes_transferred)
	{
		if (!e)
		{
			struct inotify_event* inotify_event;
			unsigned int bytes_processed = 0;
			if (bytes_transferred > sizeof (struct inotify_event))
			{
				do
				{
					inotify_event = reinterpret_cast<struct inotify_event*> (buffer_ + bytes_processed);
					event ev;
					ev.file_name = inotify_event->name;
					watch_descriptors_type::left_iterator it = watch_descriptors_.left.find(inotify_event->wd);
					if (it != watch_descriptors_.left.end())
						ev.dir_name = it->second;
					ev.type = static_cast<event::event_type> (inotify_event->mask);
					handler_(ev, e);
					bytes_processed += sizeof (struct inotify_event) + inotify_event->len;
				}
				while (bytes_transferred - bytes_processed >= sizeof (struct inotify_event) + inotify_event->len);
			}
			async_read();
		}
		else
		{
			handler_(event(), e);
		}
	}
    

//    boost::asio::io_service& io_service_;
    boost::asio::io_service io_service_;
    boost::shared_ptr<boost::asio::io_service::work> work_; //(new boost::asio::io_service::work(io_service));
//    boost::thread_group threadGroup_;
    HeapThread thread_;


    bool is_initialized_;

    boost::asio::posix::stream_descriptor monitor_stream_descriptor_;
    //boost::asio::posix::stream_descriptor monitor_stream_descriptor_(io_service_);

    int fileDescriptor_;
    typedef boost::bimap<uint32_t, std::string> watch_descriptors_type;
    watch_descriptors_type watch_descriptors_;
    handler_type handler_;

    enum
    {
        max_length = 1024
    };

    unsigned char buffer_[max_length];
    //boost::array<char, 1024> buffer_;





};


#endif	/* _DIRMONITOR_HPP */

