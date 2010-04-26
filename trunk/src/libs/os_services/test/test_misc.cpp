#include <iostream>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

struct A
{
	int file_descriptor_;
	std::string name_;
};

typedef boost::shared_ptr<A> a_pointer_type;
typedef std::vector<a_pointer_type> collection_type;

struct X
{

	void add( a_pointer_type item )
	{
		a_collection.push_back( item );

		register_event_system_call ( item->file_descriptor_, item.get() ); // boost::shared_ptr<A>::get() retorna A*

		// int register_event_system_call (int file_descriptor, void* data);
		// Supongamos que "register_event_system_call" registra un evento en el sistema operativo para este monitoree si el archivo "file_descriptor_" fue modificado.
		// A esta misma funcion se le pasa un void* que es un dato que será retornado cuando el evento sea capturado ...
	}

	void notify ( a_pointer_type item )
	{
		std::cout << "el archivo " << item->name_ << " ha cambiado." << std::endl;
	}

	void do_work( )
	{
		void* data;
		
		while ( true )
		{
			get_notification_system_call ( data );
			// get_notification_system_call es una funcion que aguarda a que se produzca un evento (sistema operativo) y retorna un void* que se relaciona con el "void* data" pasado a la funcion "register_event_system_call" que es llamada más arriba

			//*1 - Notificar que "data" cambio...

			a_pointer_type item; // ¿Como relaciono data con item?????
			
			notify ( item );
		}

	}

	collection_type a_collection;
};

int main( int /*argc*/, char** /*argv*/ )
{
	X x;
	
	a_pointer_type a (new A);

	x.add( a );
	x.do_work();

	return 0;
}






//--------------------------------------------------------------------------------------------------------


//#include <iostream>
//
//#include <boost/shared_ptr.hpp>
//#include <boost/weak_ptr.hpp>
//
//struct A;
//
//struct B
//{
//	int data_;
//
//	boost::shared_ptr<A> a;
//};
//
//struct A
//{
//	B getB_1() const
//	{
//		return b_1;
//	}
//
//	boost::shared_ptr<B> getB_2() const
//	{
//		return this->b_2.lock();
//	}
//
//
//	B b_1;
//	boost::weak_ptr<B> b_2;
//};
//
//void func_1( B& b ) //static 
//{
//	std::cout << b.data_ << std::endl;
//}
//
//
//void func_2( boost::shared_ptr<B>& b ) //static 
//{
//	std::cout << b->data_ << std::endl;
//}
//
//int main()
//{
//	A a;
//
//	func_1( a.getB_1() ); //Esto da error en gcc //invalid initialization of non-const reference of type 'B&' from a temporary of type 'B'
//	func_2( a.getB_2() ); //Esto da error en gcc //IDEM anterior con diferencia de tipos...
//
//	return 0;
//}





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
