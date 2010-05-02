//#include <iostream>
//
//#include <boost/shared_ptr.hpp>
//
//
//struct a_class
//{
//	typedef boost::shared_ptr<a_class> pointer_type;
//
//	a_class()
//	{
//		std::cout << "a_class()" << std::endl;
//	}
//
//	~a_class()
//	{
//		std::cout << "~a_class() - member_: " << member_ << std::endl;
//	}
//
//	int member_;
//};
//
//void temp( a_class::pointer_type a )
//{
//	std::cout << "a->member_: " << a->member_ << std::endl;
//}
//
//
//int main()
//{
//
//	{
//		//a_class* a_permanent = new a_class;
//		//a_permanent->member_ = 9999;
//		a_class::pointer_type a_permanent( new a_class );
//		a_permanent->member_ = 9999;
//
//		void* a_void_ptr = &a_permanent;
//
//
//		//{
//		//	a_class a1;
//		//	a1.member_ = 1;
//		//}
//
//		//{
//		//	a_class* a2 = new a_class;
//		//	a2->member_ = 2;
//		//	delete a2;
//		//}
//
//		//{
//		//	a_class::pointer_type a3( new a_class );
//		//	a3->member_ = 3;
//		//}
//
//		//{
//		//	a_class::pointer_type a_temp( a_permanent );
//		//}
//
//
//		{
//			a_class::pointer_type a_temp = *reinterpret_cast<a_class::pointer_type*>( a_void_ptr );
//
//			temp( a_temp );
//
//			std::cout << "inmediatamente luego de esto no deberia llamarse al constructor" << std::endl;
//		}
//
//
//		std::cout << "recien ahora quiero eliminar a_permanent..." << std::endl;
//		//delete a_permanent;
//	}
//
//
//	std::cout << "press ENTER to finish..." << std::endl;
//	std::cin.sync();
//	std::cin.get();
//
//	return 0;
//}
