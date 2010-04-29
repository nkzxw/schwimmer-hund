#ifndef BOOST_OS_SERVICES_DETAIL_WIN32_LEGACY_HPP
#define BOOST_OS_SERVICES_DETAIL_WIN32_LEGACY_HPP

//TODO: no me gusta haber separado en este arhivo, para ello tenemos el windows_impl

#include <string>

//#include <boost/array.hpp>

#include <boost/os_services/detail/win32api_wrapper.hpp>

#define MAX_DIRS    256
#define MAX_BUFFER  8192 //4096	//TODO: como cambiarlo en tiempo de ejecución?

//DIRECTORY_INFO  directory_info_array[MAX_DIRS];        // Buffer for all of the directories
//boost::array<DIRECTORY_INFO, MAX_DIRS> directory_info_array;

//DIRECTORY_INFO  directoryInfo;
//TCHAR           FileList[MAX_FILES*MAX_PATH];        // Buffer for all of the files
//DWORD           numDirs;	// TODO: global... hay que sacarla...


//TODO: namespace


struct directory_info
{

	directory_info()
		: handle_ ( 0 ), buffer_length ( 0 )
	{
		initialize();
	}

	directory_info( const std::string& path )
		: path_( path ), handle_ ( 0 ), buffer_length ( 0 )
	{
		initialize();
	}

	~directory_info()
	{
		close( true );
	}

	void initialize()
	{
		memset( buffer, 0, sizeof(buffer) );
		memset( (void*) &overlapped , 0, sizeof(overlapped ) );
	}


	void close( bool no_throw = false )
	{
		if ( handle_ != 0 )
		{
			try
			{
				boost::os_services::detail::win32api_wrapper::close_handle( handle_ );
			}
			catch ( const std::runtime_error& e)
			{
				//Destructor -> NO_THROW
				std::cerr << e.what() << std::endl;
			}
		}
	}


	HANDLE      handle_;
	std::string path_;

	CHAR        buffer[MAX_BUFFER];
	DWORD       buffer_length;
	OVERLAPPED  overlapped;
};


#endif // BOOST_OS_SERVICES_DETAIL_WIN32_LEGACY_HPP
