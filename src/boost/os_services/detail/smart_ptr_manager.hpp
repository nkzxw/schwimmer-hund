#ifndef BOOST_OS_SERVICES_DETAIL_SMART_PTR_MANAGER_HPP_INCLUDED
#define BOOST_OS_SERVICES_DETAIL_SMART_PTR_MANAGER_HPP_INCLUDED

#include <boost/shared_ptr.hpp>
#include <boost/unordered_map.hpp>

namespace boost {
namespace os_services {
namespace detail 
{

template < typename T, template <typename> class SmartPtr = boost::shared_ptr >
class smart_ptr_manager
{
public:

	typedef SmartPtr<T> pointer_type;
	typedef unsigned long address_type;		//TODO: encontrar de alguna forma cual es el tipo que coincide con sizeof(void*)

	static void add( const pointer_type& pointer )
	{
		pointers_.insert( std::make_pair( reinterpret_cast<address_type>( pointer.get() ), pointer ) );
	}

	static pointer_type add( T* raw_pointer )
	{
		pointer_type pointer( raw_pointer );
		add( pointer );
		return pointer;
	}

	static pointer_type get( address_type address )
	{
		return pointers_.at( address );
	}

	static bool exists( address_type address )
	{
		collection_type::const_iterator it = pointers_.find( address );
		return ( it != pointers_.end() );
	}

	static bool exists( const pointer_type& pointer )
	{
		collection_type::const_iterator it = pointers_.find( reinterpret_cast<address_type>( pointer.get() ) );
		return ( it != pointers_.end() );
	}

	static pointer_type release( address_type address )
	{
		pointer_type pointer = pointers_.at( address );
		pointers_.erase( address );
		return pointer;
	}

	static pointer_type release( const pointer_type& pointer )
	{
		return release( reinterpret_cast<address_type>( pointer.get() ) );
	}

private:

	typedef boost::unordered_map<address_type, pointer_type> collection_type;
	static collection_type pointers_; 
};

template < typename T, template <typename> class SmartPtr >
boost::unordered_map<typename smart_ptr_manager<T, SmartPtr>::address_type, typename smart_ptr_manager<T, SmartPtr>::pointer_type> smart_ptr_manager<T, SmartPtr>::pointers_; 

} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_SMART_PTR_MANAGER_HPP_INCLUDED
