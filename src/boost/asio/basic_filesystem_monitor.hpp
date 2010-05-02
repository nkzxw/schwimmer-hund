// TODO: cambiar el Copyright, ver como hago con Boris para incluirlo.
// TODO: convertir todos los archivos fuente a formato Unix (NO chr10 + chr13)
// TODO: ver que antes de cada enter hay un blanco... eliminarlos...

//
// Copyright (c) 2008, 2009 Boris Schaeling <boris@highscore.de> 
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_ASIO_BASIC_FILESYSTEM_MONITOR_HPP_INCLUDED //TODO: ver como hacen la inclusion de librerias en ASIO
#define BOOST_ASIO_BASIC_FILESYSTEM_MONITOR_HPP_INCLUDED 

#include <string>

#include <boost/asio.hpp> 


namespace boost { 
namespace asio { 

struct filesystem_monitor_event 
{ 
	//TODO: revisar
    enum event_type 
    { 
        null = 0, 
        added = 1, 
        removed = 2, 
        modified = 3, 
        renamed_old_name = 4, 
        renamed_new_name = 5 
    };

    filesystem_monitor_event() 
        : type(null) 
	{ } 

    filesystem_monitor_event(const std::string& d, const std::string& f, event_type t) 
        : dir_name(d), file_name(f), type(t) 
	{ } 

    std::string dir_name;
    std::string file_name;
    event_type type;
};

template <typename Service> 
class basic_filesystem_monitor : public boost::asio::basic_io_object<Service>
{ 
public: 

	explicit basic_filesystem_monitor( boost::asio::io_service& io_service )
        : boost::asio::basic_io_object<Service>(io_service) 
    { } 

	//TODO: ver todo lo que tengo que agregar para poder monitorear un directorio...
    void add_directory( const std::string& dir_name )
    { 
        this->service.add_directory(this->implementation, dir_name);
    } 

    void remove_directory( const std::string& dir_name )
    { 
        this->service.remove_directory(this->implementation, dir_name);
    } 

    filesystem_monitor_event monitor() 
    { 
        boost::system::error_code ec;
        filesystem_monitor_event ev = this->service.monitor( this->implementation, ec );
        boost::asio::detail::throw_error( ec );
        return ev;
    } 

    filesystem_monitor_event monitor(boost::system::error_code& ec) 
    { 
        return this->service.monitor(this->implementation, ec);
    } 

    template <typename Handler> 
    void async_monitor(Handler handler) 
    { 
        this->service.async_monitor(this->implementation, handler);
    } 
};

} // namespace asio
} // namespace boost


#endif // BOOST_ASIO_BASIC_FILESYSTEM_MONITOR_HPP_INCLUDED
