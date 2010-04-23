#ifndef BOOST_OS_SERVICES_DETAIL_KQUEUE_WATCH_ITEM_HPP
#define BOOST_OS_SERVICES_DETAIL_KQUEUE_WATCH_ITEM_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif

#include <boost/integer.hpp>

namespace boost {
namespace os_services {
namespace detail {

class kqueue_wrapper; //forward declaration

//TODO: cambiar nombre
class kqueue_watch_item
{
public:
	friend class kqueue_wrapper;

	kqueue_watch_item()
		: file_descriptor_( 0 ), mask_( 0 )
	{}

	boost::uint32_t mask() const
	{
		return this->mask_;
	}


	int file_descriptor() const
	{
		return this->file_descriptor_;
	}

protected:
	int file_descriptor_;
	boost::uint32_t mask_;
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_KQUEUE_WATCH_ITEM_HPP
