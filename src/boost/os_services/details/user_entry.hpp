#ifndef BOOST_OS_SERVICES_DETAIL_USER_ENTRY_HPP
#define BOOST_OS_SERVICES_DETAIL_USER_ENTRY_HPP


//#include <iostream>
#include <boost/filesystem/path.hpp>

#include <boost/os_services/details/filesystem_item.hpp>

namespace boost {
namespace os_services {
namespace detail {

//TODO: ver si es necesario aplicar traits...
//template <typename T> 
//struct traits
//{
//	typedef T & reference;
//};
//
//template<> struct shared_ptr_traits<void>
//{
//	typedef void reference;
//};
//
//#if !defined(BOOST_NO_CV_VOID_SPECIALIZATIONS)
//
//template<> 
//struct type_traits<void const>
//{
//	typedef void reference;
//};


//TODO: class
struct user_entry //: public enable_shared_from_this<user_entry>
{
	typedef boost::shared_ptr<user_entry> pointer_type;
	typedef std::vector<pointer_type> collection_type;


	//typedef T element_type;
	//typedef T value_type;
	//typedef T * pointer;
	//typedef typename boost::detail::shared_ptr_traits<T>::reference reference;


	//typedef user_entry* pointer_type;
	//typedef boost::ptr_vector<user_entry> collection_type;

	explicit user_entry( const boost::filesystem::path& path)
		: path_(path)
	{}

	//TODO: crear un metodo CREATE y ocultar los constructores...
	//user_entry::pointer_type item ( new user_entry( dir_name ) );

	~user_entry()
	{
		std::cout << "~user_entry()" << std::endl;
	}

	//void add_watch( boost::shared_ptr< filesystem_item > item )
	void add_watch( filesystem_item::pointer_type item )
	{
		all_watches_.push_back(item);
	}

	void remove_watch( filesystem_item::pointer_type watch )
	{
		std::cout << "debug user_entry::remove_watch-1" << std::endl;

		//TODO: usar STL
		filesystem_item::collection_type::iterator it = all_watches_.begin();
		while ( it != all_watches_.end() )
		{
			//if ( watch->is_equal( **it ) )
			if ( (*it)->is_equal( *watch ) )
			{
				std::cout << "debug user_entry::remove_watch-2" << std::endl;
				it = all_watches_.erase(it);
				std::cout << "debug user_entry::remove_watch-3" << std::endl;
				break;
			}
			else
			{
				++it;
			}
		}

		std::cout << "debug user_entry::remove_watch-4" << std::endl;

	}

	const boost::filesystem::path& path() const 
	{ 
		return this->path_; 
	}

	void set_root( filesystem_item::pointer_type root )
	{
		this->root_ = root_;
	}

protected:
	boost::filesystem::path path_;
	filesystem_item::pointer_type root_;			//este tiene la estructura de arbol
	filesystem_item::collection_type all_watches_;
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_USER_ENTRY_HPP
