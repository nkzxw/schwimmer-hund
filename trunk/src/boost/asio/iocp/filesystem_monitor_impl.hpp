// 
// Copyright (c) 2008, 2009 Boris Schaeling <boris@highscore.de> 
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_ASIO_FILESYSTEM_MONITOR_IMPL_HPP_INCLUDED
#define BOOST_ASIO_FILESYSTEM_MONITOR_IMPL_HPP_INCLUDED

#include <deque> 
#include <string> 

#include <windows.h> 

#include <boost/noncopyable.hpp> 
#include <boost/ptr_container/ptr_unordered_map.hpp> 
#include <boost/thread.hpp> 

namespace boost { 
namespace asio { 

	
class filesystem_monitor_impl 
{ 
public: 
    class windows_handle : public boost::noncopyable 
    { 
    public: 
        windows_handle( HANDLE handle )
            : handle_( handle )
        {} 

        ~windows_handle()
        { 
            CloseHandle( handle_ ); 
        } 

    private: 
        HANDLE handle_; 
    }; 

    filesystem_monitor_impl() 
        : run_(true) 
    {} 

    void add_directory( std::string dirname, HANDLE handle ) //TODO: const reference????
    { 
        dirs_.insert( dirname, new windows_handle(handle) );
    } 

    void remove_directory( const std::string& dirname )
    { 
        dirs_.erase( dirname ); 
    } 

    void destroy()
    { 
        boost::unique_lock<boost::mutex> lock( events_mutex_ );
        run_ = false;
        events_cond_.notify_all();
    } 

    filesystem_monitor_event popfront_event( boost::system::error_code& ec )
    { 
        boost::unique_lock<boost::mutex> lock(events_mutex_);

        while (run_ && events_.empty())
		{
            events_cond_.wait(lock); 
		}

        filesystem_monitor_event ev; 
        
		if (!events_.empty()) 
        { 
            ec = boost::system::error_code(); 
            ev = events_.front(); 
            events_.pop_front(); 
        } 
        else 
		{
            ec = boost::asio::error::operation_aborted;
		}


        return ev; 
    } 

    void pushback_event( filesystem_monitor_event ev ) 
    { 
        boost::unique_lock<boost::mutex> lock( events_mutex_ ); 
        if (run_) 
        { 
            events_.push_back(ev); 
            events_cond_.notify_all(); 
        } 
    } 

private: 
    boost::ptr_unordered_map<std::string, windows_handle> dirs_; 
    boost::mutex events_mutex_; 
    boost::condition_variable events_cond_; 
    bool run_; 
    std::deque<filesystem_monitor_event> events_; 
}; 

} // namespace asio
} // namespace boost

#endif // BOOST_ASIO_FILESYSTEM_MONITOR_IMPL_HPP_INCLUDED
