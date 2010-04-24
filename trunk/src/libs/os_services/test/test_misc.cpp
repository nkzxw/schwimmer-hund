#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

struct A;

struct B
{
	int data_;

	boost::shared_ptr<A> a;
};

struct A
{
	boost::shared_ptr<B> getB() const
	{
		return this->b.lock();
	}

	boost::weak_ptr<B> b;
};

//filesystem_item::pointer_type create_filesystem_item( const boost::filesystem::path& path, user_entry_pointer_type& entry, filesystem_item::pointer_type& parent ) //static 
void func( boost::shared_ptr<B>& b ) //static 
{
	std::cout << b->data_ << std::endl;
}

int main()
{
	A a;

	func( a.getB() );

	return 0;
}





//#include <iostream>
//
//
//#include <boost/shared_ptr.hpp>
//
//struct A
//{
//
//};
//
//template <typename T>
//struct AT
//{
//
//};
//
//
//struct X
//{
//	A get() const
//	{
//		return A();
//	}
//
//	AT<A> get_2() const
//	{
//		return AT<A>();
//	}
//
//	AT<A> get_3() const
//	{
//		return at_a;
//	}
//
//	AT<A> at_a;
//};
//
//void put( A& a )
//{
//	std::cout << "put" << std::endl;
//}
//
//typedef AT<A> AT_A_type;
//
//void put_2( AT<A>& a )
//{
//	std::cout << "put_2" << std::endl;
//
//}
//
//void put_3( AT_A_type& a )
//{
//	std::cout << "put_3" << std::endl;
//}
//
//int main()
//{
//	X x;
//
//	put( x.get() );
//	put_2( x.get_2() );
//	put_3( x.get_2() );
//	put_3( x.get_3() );
//
//
//	return 0;
//}
