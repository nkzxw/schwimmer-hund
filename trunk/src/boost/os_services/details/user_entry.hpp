#ifndef BOOST_OS_SERVICES_DETAIL_USER_ENTRY_HPP
#define BOOST_OS_SERVICES_DETAIL_USER_ENTRY_HPP


//#include <iostream>
#include <boost/filesystem/path.hpp>

#include <boost/os_services/details/filesystem_item.hpp>

namespace boost {
namespace os_services {
namespace detail {

//TODO: class
struct user_entry //: public enable_shared_from_this<user_entry>
{
	//typedef boost::shared_ptr<user_entry> pointer_type;
	//typedef std::vector<pointer_type> collection_type;
	typedef user_entry* pointer_type;
	typedef boost::ptr_vector<user_entry> collection_type;

	explicit user_entry( const boost::filesystem::path& path)
		: path_(path)
	{}

	//~user_entry()
	//{
	//	//std::cout << "--------------------- ~fsitem() ------------------------------" << std::endl;
	//	//std::cout << "this->path.native_file_string(): " << this->path.native_file_string() << std::endl;
	//}

	void add_watch( filesystem_item::pointer_type item )
	{
		all_watches_.push_back(item);
	}

	filesystem_item::collection_type::iterator remove_watch( filesystem_item::collection_type::iterator it )
	{
		return all_watches_.erase(it);
	}

	//void initialize()
	//{
	//	//TODO: estas dos instrucciones ponerlas en un factory
	//	//filesystem_item::pointer_type item ( new filesystem_item (path_, this ) );
	//	//filesystem_item::pointer_type item ( new filesystem_item (path_, shared_from_this() ) );
	//	//filesystem_item::pointer_type item = new filesystem_item (path_, shared_from_this() );
	//	//filesystem_item::pointer_type item = new filesystem_item ( path_, this );
	//	//root_ = item;

	//	root_ = new filesystem_item ( path_, this ); //TODO: no me gusta que este sea el responsable de hacer el new... debe venir como parámetro...
	//	all_watches_.push_back(root_);
	//	create_watch( root_, false );
	//}

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
