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
	B getB_1() const
	{
		return b_1;
	}

	boost::shared_ptr<B> getB_2() const
	{
		return this->b_2.lock();
	}


	B b_1;
	boost::weak_ptr<B> b_2;
};

void func_1( B& b ) //static 
{
	std::cout << b.data_ << std::endl;
}


void func_2( boost::shared_ptr<B>& b ) //static 
{
	std::cout << b->data_ << std::endl;
}

int main()
{
	A a;

	func_1( a.getB_1() ); //Esto da error en gcc //invalid initialization of non-const reference of type 'B&' from a temporary of type 'B'
	func_2( a.getB_2() ); //Esto da error en gcc //IDEM anterior con diferencia de tipos...

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
