// 
// Copyright (c) 2008, 2009 Boris Schaeling <boris@highscore.de> 
// 
// Distributed under the Boost Software License, Version 1.0. (See accompanying 
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt) 
// 

#ifndef BOOST_ASIO_DIRECTORY_HPP_INCLUDED
#define BOOST_ASIO_DIRECTORY_HPP_INCLUDED

//#include <fstream> 

#include <boost/filesystem.hpp> 
//#include <boost/thread.hpp> 


namespace boost {
namespace asio {


class directory 
{ 
public: 
    //directory( const char *name )
    //    : name_( boost::filesystem::complete(name) )
    //{ 
    //    boost::filesystem::create_directory(name_); 
    //    BOOST_REQUIRE(boost::filesystem::is_directory(name_)); 
    //} 

	//TODO: agregar opcion para crear directorio en caso de que no exista
	explicit directory( const char *path )
		: path_( boost::filesystem::complete(path) )
	{ 
		initialize( path );
	} 

	explicit directory( const std::string& path )
		: path_( boost::filesystem::complete(path) )
	{ 
		initialize( path );
	} 


	void initialize( const std::string& path )
	{ 
		if ( ! boost::filesystem::is_directory(path) )
		{
			throw std::runtime_error("Directory dont exists..."); //TODO: ver bien el mensaje...
		}
	} 

	
    //~directory() 
    //{ 
    //    bool again; 
    //    do 
    //    { 
    //        try 
    //        { 
    //            boost::filesystem::remove_all(name_); 
    //            again = false; 
    //        } 
    //        catch (...) 
    //        { 
    //            boost::this_thread::yield(); 
    //            again = true; 
    //        } 
    //    } while (again); 
    //} 

    //void create_file( const char *file )
    //{ 
    //    boost::filesystem::current_path(name_); 
    //    BOOST_REQUIRE(boost::filesystem::equivalent(name_, boost::filesystem::current_path())); 
    //    std::ofstream ofs(file); 
    //    BOOST_REQUIRE(boost::filesystem::exists(file)); 
    //    boost::filesystem::current_path(boost::filesystem::initial_path()); 
    //    BOOST_REQUIRE(boost::filesystem::equivalent(boost::filesystem::current_path(), boost::filesystem::initial_path())); 
    //} 

    //void rename_file(const char *from, const char *to) 
    //{ 
    //    boost::filesystem::current_path(name_); 
    //    BOOST_REQUIRE(boost::filesystem::equivalent(name_, boost::filesystem::current_path())); 
    //    BOOST_REQUIRE(boost::filesystem::exists(from)); 
    //    boost::filesystem::rename(from, to); 
    //    BOOST_REQUIRE(boost::filesystem::exists(to)); 
    //    boost::filesystem::current_path(boost::filesystem::initial_path()); 
    //    BOOST_REQUIRE(boost::filesystem::equivalent(boost::filesystem::current_path(), boost::filesystem::initial_path())); 
    //} 

    //void remove_file(const char *file) 
    //{ 
    //    boost::filesystem::current_path(name_); 
    //    BOOST_REQUIRE(boost::filesystem::equivalent(name_, boost::filesystem::current_path())); 
    //    BOOST_REQUIRE(boost::filesystem::exists(file)); 
    //    boost::filesystem::remove(file); 
    //    boost::filesystem::current_path(boost::filesystem::initial_path()); 
    //    BOOST_REQUIRE(boost::filesystem::equivalent(boost::filesystem::current_path(), boost::filesystem::initial_path())); 
    //} 

private: 
    boost::filesystem::path path_; 
}; 

} // namespace asio
} // namespace boost

#endif // BOOST_ASIO_DIRECTORY_HPP_INCLUDED
