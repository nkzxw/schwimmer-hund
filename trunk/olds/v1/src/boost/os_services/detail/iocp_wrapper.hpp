#ifndef BOOST_OS_SERVICES_DETAIL_IOCP_WRAPPER_HPP_INCLUDED
#define BOOST_OS_SERVICES_DETAIL_IOCP_WRAPPER_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif


#include <boost/noncopyable.hpp>

#include <boost/os_services/detail/smart_ptr_manager.hpp>
#include <boost/os_services/detail/win32api_wrapper.hpp>


//TODO: sacar de todos los otros lugares donde esté repetido
//TODO: como cambiarlo en tiempo de ejecución?
#define MAX_BUFFER  8192 //4096	

namespace boost {
namespace os_services {
namespace detail {

template <typename T>
class iocp_wrapper : private boost::noncopyable
{
public:

	typedef boost::shared_ptr<T> pointer_type; //TODO: cambiar el nombre del typedef... no me gusta...

	iocp_wrapper()
		: is_initialized_( false ), handle_( 0 ), last_notify_information_( 0 ) //offset_( 0 ), 
	{
	}

	~iocp_wrapper()
	{
		close( true ); //Destructor -> no throw
	}

	void initialize()
	{
		//TODO:
		if ( ! is_initialized_ )
		{
			is_initialized_ = true;
		}
	}

	//boost::uint32_t add_watch( const std::string& file_name )
	//{
	//	boost::uint32_t watch_descriptor = ::inotify_add_watch( handle_, file_name.c_str(), IN_CREATE | IN_DELETE | IN_MODIFY | IN_MOVED_FROM | IN_MOVED_TO );
	//	
	//	if ( watch_descriptor == -1 )
	//	{
	//		std::ostringstream oss;
	//		oss << "inotify_add_watch error - File: " << file_name << " - Reason: " << std::strerror(errno);
	//		throw ( std::invalid_argument(oss.str()) );
	//	}
	//}

	//TODO:
	//	void* add_watch( const std::string& path ) //throw (std::invalid_argument, std::runtime_error)

	//TODO: podemos aplicar la misma tecnica de "template template" usada en smart_ptr_manager para evitar acoplarnos con shared_ptr

	//template <typename T>
	//void add_watch( const boost::shared_ptr<T>& watch )

	void add_watch( const pointer_type& watch )
	{
		//TODO: datos necesarios para crear el watch
		//      const std::string& path					// OK -> Dato
		//		this->include_subdirectories_			// OK -> Dato
		//		this->notify_filters_,					// OK -> Dato

		//		completion_port_handle_					// pertenece a la clase
		//		directory_info->directory_handle,		// Se crea acá mismo
		//		directory_info
		//		&(*it)->overlapped
		//		(*it)->buffer,							// Un buffer por directorio???????????? Probar usando uno independiente...
		//		MAX_BUFFER, 
		//		&(*it)->buffer_length, 



		//LPDIRECTORY_INFO directory_info = (LPDIRECTORY_INFO) malloc(sizeof(DIRECTORY_INFO));
		//memset(directory_info, 0, sizeof(DIRECTORY_INFO));

		//directory_info->directory_handle = win32api_wrapper::create_file( path, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL );

		////TODO: esto no me gusta
		//lstrcpy( directory_info->directory_name, path.c_str() );

		////unsigned long addr = (unsigned long) &directory_info;

		//completion_port_handle_ = win32api_wrapper::create_io_completion_port( directory_info->directory_handle, completion_port_handle_, (DWORD) directory_info, 0 );

		//win32api_wrapper::read_directory_changes( (*it)->directory_handle, (*it)->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &(*it)->buffer_length, &(*it)->overlapped, NULL );

		smart_ptr_manager<T>::add( watch );
		watch->handle_ = win32api_wrapper::create_file( watch->path_, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL );
		handle_ = win32api_wrapper::create_io_completion_port( watch->handle_, handle_, (DWORD) watch.get(), 0 );


		//TODO: this->include_subdirectories_
		//TODO: this->notify_filters_
		//win32api_wrapper::read_directory_changes( watch->handle_, watch->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &watch->buffer_length, &watch->overlapped, NULL );
		win32api_wrapper::read_directory_changes( watch->handle_, watch->buffer, MAX_BUFFER, true ? 1 : 0, 51, &watch->buffer_length, &watch->overlapped, NULL );

	}


	//void remove_watch( boost::uint32_t& watch_descriptor ) const
	//{
	//	//TODO:
	//}

	void close( bool no_throw = false )
	{
		if ( handle_ != 0 )
		{
			if ( no_throw )
			{
				try
				{
					win32api_wrapper::post_queued_completion_status( handle_, 0, 0, NULL );
					win32api_wrapper::close_handle( handle_ );
				}
				catch ( const std::runtime_error& e )
				{
					//Destructor -> NO_THROW
					std::cerr << e.what() << std::endl;
				}
			}
			else
			{
				win32api_wrapper::post_queued_completion_status( handle_, 0, 0, NULL );
				win32api_wrapper::close_handle( handle_ );
			}
			handle_ = 0;
		}
		is_initialized_ = false;
	}


	//TODO: podemos aplicar la misma tecnica de "template template" usada en smart_ptr_manager para evitar acoplarnos con shared_ptr
	//template <typename T>
	//boost::shared_ptr<T> get( std::string& directory_path, std::string& file_name, unsigned long& action ) //TODO: file_name y action podrian incluirse en una clase que contenga todos los datos del evento necesarios...


	bool get( std::string& directory_path, std::string& file_name, unsigned long& action ) //TODO: file_name y action podrian incluirse en una clase que contenga todos los datos del evento necesarios...
	{
		//boost::shared_ptr<T> dir_info;

		if ( last_notify_information_ == 0 ) //(offset_ == 0 )
		{
			unsigned long num_bytes;
			unsigned long address = 0;
			LPOVERLAPPED overlapped;


			win32api_wrapper::get_queued_completion_status( handle_, &num_bytes, &address, &overlapped, INFINITE );

			if ( address != 0 && num_bytes > 0 )
			{
				last_dir_info_ = smart_ptr_manager<T>::get( address );

				if ( last_dir_info_ )
				{
					//TODO: opciones...
					//win32api_wrapper::read_directory_changes( last_dir_info_->handle_, last_dir_info_->buffer, MAX_BUFFER, this->include_subdirectories_ ? 1 : 0, this->notify_filters_, &last_dir_info_->buffer_length, &last_dir_info_->overlapped, NULL );
					win32api_wrapper::read_directory_changes( last_dir_info_->handle_, last_dir_info_->buffer, MAX_BUFFER, true ? 1 : 0, 51, &last_dir_info_->buffer_length, &last_dir_info_->overlapped, NULL );


					//PFILE_NOTIFY_INFORMATION notify_information = (PFILE_NOTIFY_INFORMATION)dir_info->buffer;
					//PFILE_NOTIFY_INFORMATION notify_information = reinterpret_cast<PFILE_NOTIFY_INFORMATION>( dir_info->buffer );
					last_notify_information_ = reinterpret_cast<PFILE_NOTIFY_INFORMATION>( last_dir_info_->buffer );
				} 
			}
		}

		if ( last_notify_information_ != 0 )
		{
			//std::string temp(last_notify_information_->FileName, last_notify_information_->FileName + (last_notify_information_->FileNameLength/sizeof(WCHAR)) );
			//file_name = temp;

			//TODO: ver si hay algun metodo en std::string que permita hacer esto sin construir otro objeto...
			file_name = std::string(last_notify_information_->FileName, last_notify_information_->FileName + (last_notify_information_->FileNameLength/sizeof(WCHAR)) );

			directory_path = last_dir_info_->path_;
			action = last_notify_information_->Action;
			//offset_ = last_notify_information_->NextEntryOffset;

			if ( last_notify_information_->NextEntryOffset == 0 )
			{

				last_notify_information_ = 0;
			}
			else
			{
				last_notify_information_ = (PFILE_NOTIFY_INFORMATION)((LPBYTE) last_notify_information_ + last_notify_information_->NextEntryOffset);
			}


			//while( offset );//

			return true;
		}
		else
		{
			return false;
		}


		//return last_dir_info_;
	}

protected:

	bool is_initialized_;
	void* handle_;
	//unsigned long offset_;
	PFILE_NOTIFY_INFORMATION last_notify_information_;
	pointer_type last_dir_info_;	//TODO: cambiar el nombre de last_dir_info...

};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_IOCP_WRAPPER_HPP_INCLUDED
