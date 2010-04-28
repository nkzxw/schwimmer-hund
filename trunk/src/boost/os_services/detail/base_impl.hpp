#ifndef BOOST_OS_SERVICES_DETAIL_BASE_IMPL_HPP
#define BOOST_OS_SERVICES_DETAIL_BASE_IMPL_HPP

#include <string>

#include <boost/filesystem/path.hpp>

#include <boost/os_services/detail/utils.hpp>
#include <boost/os_services/event_args.hpp>
#include <boost/os_services/event_handlers.hpp>

namespace boost {
namespace os_services {
namespace detail {

//TODO: el nombre base_impl es relativo porque la base no es una implementacion...
//TODO: el typename Type no me gusta el nombre... ver como se resuelve en en idiom PImpl
template <typename Type>
class base_impl // : private boost::noncopyable -> No tiene sentido ya que es "detail" //TODO: evaluar si sirve para que las clases de implementacion no sean copiadas... o minimo la FSM
{
public:

	// Event Handlers
	filesystem_event_handler changed_handler_;
	filesystem_event_handler created_handler_;
	filesystem_event_handler deleted_handler_;
	renamed_event_handler renamed_handler_;

	void add_directory( const std::string& dir_name ) //throw (std::invalid_argument, std::runtime_error)
	{ 
		if ( ! utils::directory_exists( dir_name ) )
		{
			throw( std::invalid_argument("'" + dir_name + "' is not a valid directory.") );
		}

		static_cast<Type*>(this)->add_directory_impl( dir_name );
	}

	void add_directory( const boost::filesystem::path& directory ) //throw (std::invalid_argument, std::runtime_error)
	{ 
		if ( ! utils::directory_exists( directory ) )
		{
			throw( std::invalid_argument("'" + directory.native_file_string() + "' is not a valid directory.") );
		}

		static_cast<Type*>(this)->add_directory_impl( directory );
	}

	//TODO: ver si es necesario el metodo start() en base y start_impl() en *_impl
	//void start() {}
	//TODO: cada metodo publico debe ser expuesto por via esta clase...
	

public: // private: //TODO:
	//TODO: estos tres atributos son a nivel de watch o user_entry
	int notify_filters_;			//TODO: deberia ser un enum
	std::string filter_;
	bool include_subdirectories_;	

protected:
	template <typename Callback, typename Args>
	inline void do_callback(Callback callback, Args args )
	{
		if ( callback )
		{
			callback( args );
		}
	}
};

} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_BASE_IMPL_HPP
