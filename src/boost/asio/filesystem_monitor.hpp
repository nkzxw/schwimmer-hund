// 
// Copyright (c) 2008, 2009 Boris Schaeling <boris@highscore.de> 
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_ASIO_FILESYSTEM_MONITOR_HPP_INCLUDED
#define BOOST_ASIO_FILESYSTEM_MONITOR_HPP_INCLUDED 

#include "basic_filesystem_monitor.hpp" 

//TODO: ver lo hecho en impl_selector

#if defined(_WIN32) || defined(__WIN32__) || defined(WIN32) 
#  include "iocp/basic_filesystem_monitor_service.hpp" 
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) 
#  include "inotify/basic_filesystem_monitor_service.hpp" 
#else 
#  error "Platform not supported." 
#endif 

namespace boost { 
namespace asio { 

typedef basic_filesystem_monitor< basic_filesystem_monitor_service<> > filesystem_monitor; 

} // namespace asio
} // namespace boost


#endif // BOOST_ASIO_FILESYSTEM_MONITOR_HPP_INCLUDED
