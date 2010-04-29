#ifndef BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_DUMMY_HPP_INCLUDED
#define BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_DUMMY_HPP_INCLUDED

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include <stdexcept>

#include <boost/noncopyable.hpp>


namespace boost {
namespace os_services {
namespace detail {

namespace kqueue_event_types
{
	static const int none = 0;
	static const int write = 1;
	static const int remove = 2;
	static const int rename = 3;
}

class kevent_timeout : public std::runtime_error 
{
public:
	kevent_timeout ( const std::string& err ) : std::runtime_error ( err ) 
	{}
};


struct null_deleter
{
	void operator()(void const *) const
	{
		std::cout << "void null_deleter::operator()(void const *) const" << std::endl;
	}
};


//TODO: debe ser singleton, ver como implementarlo...
//TODO: heredar de una clase abstracta, para poder hacer un dummy object con las mismas funcionalidades, salvo que se templarice
class kqueue_wrapper_dummy : private boost::noncopyable
{
public:

	kqueue_wrapper_dummy()
		: is_initialized_( false ), file_descriptor_( 0 ), executor_counter_( 0 )
	{
		std::cout << "kqueue_wrapper_dummy()" << std::endl;
	}

	~kqueue_wrapper_dummy()
	{
		std::cout << "~kqueue_wrapper_dummy()" << std::endl;
		close( true );
	}

	void initialize()
	{
		if ( ! is_initialized_ )
		{
			file_descriptor_ = 1;
			is_initialized_ = true;
		}
	}

	void close( bool no_throw = false )
	{
		std::cout << "void kqueue_wrapper_dummy::close( bool no_throw = false )" << std::endl;

		if ( file_descriptor_ != 0 )
		{
			file_descriptor_ = 0;
		}
		is_initialized_ = false;
	}

	template <typename T>
	void add_watch( const boost::shared_ptr<T>& watch )
	{
		tempWatchAdded = watch.get();
	}

	template <typename T>
	boost::shared_ptr<T> get( int& event_type )
	{
		boost::shared_ptr<T> watch;
		event_type = kqueue_event_types::none;
		

		if ( executor_counter_ == 0) 
		{
			void* void_pointer = (void*)0x003971a0;
			watch = create_watch_item<T>( void_pointer ); //null deleter shared_ptr
			std::cout << "NOTE_WRITE" << std::endl;
			event_type = kqueue_event_types::write;
			++executor_counter_;
		}
		else if ( executor_counter_ == 1) 
		{
			//lanzo siempre time-out
			std::ostringstream oss;
			oss << "kevent timeout - Reason: " << "XXXXXXX";
			std::cout << "THROW - boost::shared_ptr<T> kqueue_wrapper_dummy::get( int& event_type )" << std::endl;
			++executor_counter_;
			throw ( kevent_timeout( oss.str() ) );
		}
		else if ( executor_counter_ == 2) 
		{
			void* void_pointer = (void*)0x00398c60;
			watch = create_watch_item<T>( void_pointer ); //null deleter shared_ptr
			std::cout << "NOTE_DELETE" << std::endl;
			event_type = kqueue_event_types::remove;
			++executor_counter_;
		}
		else
		{
			//lanzo siempre time-out
			std::ostringstream oss;
			oss << "kevent timeout - Reason: " << "XXXXXXX";
			std::cout << "THROW - boost::shared_ptr<T> kqueue_wrapper_dummy::get( int& event_type )" << std::endl;
			++executor_counter_;
			throw ( kevent_timeout( oss.str() ) );
		}




		//watch = create_watch_item<T>( event.udata ); //null deleter shared_ptr

		//std::cout << "event.udata: " << event.udata << std::endl;
		//std::cout << "watch->path().native_file_string(): " << watch->path().native_file_string() << std::endl;
		//std::cout << "watch.use_count() ----- 2: " << watch.use_count() << std::endl;

		//if ( event.fflags & NOTE_DELETE )
		//{
		//	std::cout << "NOTE_DELETE" << std::endl;
		//	event_type = kqueue_event_types::remove;
		//}
		//else if ( event.fflags & NOTE_RENAME )
		//{
		//	std::cout << "NOTE_RENAME" << std::endl;
		//	event_type = kqueue_event_types::rename;
		//}
		//else if ( event.fflags & NOTE_WRITE )
		//{
		//	std::cout << "NOTE_WRITE" << std::endl;
		//	event_type = kqueue_event_types::write;
		//}

		return watch;
	}

protected:

	template <typename T>
	static boost::shared_ptr<T> create_watch_item ( void* raw_pointer )
	{
		boost::shared_ptr<T> px( reinterpret_cast<T*>( raw_pointer ), null_deleter() );
		std::cout << "px.use_count() ----- 1: " << px.use_count() << std::endl;
		return px;
	}

	bool is_initialized_;
	int file_descriptor_;

	void* tempWatchAdded;


	int executor_counter_;

};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_KQUEUE_WRAPPER_DUMMY_HPP_INCLUDED
