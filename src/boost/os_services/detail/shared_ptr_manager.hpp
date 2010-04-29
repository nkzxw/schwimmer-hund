#ifndef BOOST_OS_SERVICES_DETAIL_SHARED_PTR_MANAGER_HPP
#define BOOST_OS_SERVICES_DETAIL_SHARED_PTR_MANAGER_HPP

//#include <string>
//#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace boost {
namespace os_services {
namespace detail 
{


template <typename T>
struct shared_ptr_manager
{
	typedef boost::shared_ptr<T> pointer_type;
	typedef int address_type;

	static void add( const pointer_type& pointer )
	{
		pointers_.insert( pointer.get(), pointer );
	}

	static pointer_type get( address_type address )
	{
		return pointers_[address];
	}
	
	static pointer_type release( address_type address )
	{
		pointer_type pointer = pointers_[address];

		pointers_.erase( address );

		return pointer;
	}

	static pointer_type release( const pointer_type& pointer )
	{
		return release( pointer.get() );
	}

	static boost::unordered_map<address_type, pointer_type> pointers_;
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_SHARED_PTR_MANAGER_HPP
