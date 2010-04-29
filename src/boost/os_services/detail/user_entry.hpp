#ifndef BOOST_OS_SERVICES_DETAIL_USER_ENTRY_HPP_INCLUDED
#define BOOST_OS_SERVICES_DETAIL_USER_ENTRY_HPP_INCLUDED

#include <boost/filesystem/path.hpp>

#include <boost/os_services/detail/filesystem_item.hpp>


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



//TODO: agregar otra clase para implementar el user_entry de freebsd...
//TODO: diferenciar entre user_entry (entrada del usuario) y una clase de uso interno con todos los otros datos necesarios para el watch


//TODO: class
//TODO: cambiar el nombre a esta clase... a lo sumo se tiene que llamar watch, pero no user_entry, ya que es obvio que es una entrada de datos del usuario
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

	static user_entry::pointer_type create( const boost::filesystem::path& path )
	{
		user_entry::pointer_type entry ( new user_entry ( path ) );
		return entry;
	}

	////TODO: eliminar este destructor. Verificar que destructores hay y cuales deberian ser virtuales...
	//~user_entry()
	//{
	//}

	void add_watch( const filesystem_item::pointer_type& item )
	{
		all_watches_.push_back(item);
	}

	void remove_watch( const filesystem_item::pointer_type& watch )
	{
		//TODO: usar STL
		filesystem_item::collection_type::iterator it = all_watches_.begin();

		while ( it != all_watches_.end() )
		{
			if ( (*it)->is_equal( *watch ) )
			{
				it = all_watches_.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}
	}

	const boost::filesystem::path& path() const 
	{ 
		return this->path_; 
	}

	void set_root( const filesystem_item::pointer_type& root )
	{
		this->root_ = root;
	}

private:

	explicit user_entry( const boost::filesystem::path& path )
		: path_(path)
	{}

protected:

	boost::filesystem::path path_;
	filesystem_item::pointer_type root_;			//este tiene la estructura de arbol	//TODO: solo para freebsd


//TODO: agregar datos:
	//		this->include_subdirectories_			// OK -> Dato
	//		this->notify_filters_,					// OK -> Dato
	//		monitor->set_filter("*.txt");			// OK -> Dato


public: //TODO poner en private...
	filesystem_item::collection_type all_watches_;	//TODO: solo para freebsd
};


} // namespace detail
} // namespace os_services
} // namespace boost

#endif // BOOST_OS_SERVICES_DETAIL_USER_ENTRY_HPP_INCLUDED
