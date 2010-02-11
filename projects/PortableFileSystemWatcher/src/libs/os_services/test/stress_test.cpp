//#include <iostream>
//#include <sstream>
//#include <string>
//
//#include <boost/filesystem.hpp>
//#include <boost/lexical_cast.hpp>
//
//const int default_max_files = 10000;
//
//int main(int argc, char* argv[] )
//{
//	//std::string path = "C:\\temp1";
//	//std::string path = "J:\\temp1";
//	//std::string path = "/home/fernando/temp1";
//
//
//	std::string source_file_path = "D:\\temp1\\a.txt";
//	std::string target_file_path = "D:\\temp1\\\\a";
//
//	int max_files = default_max_files;
//
//	if (argc > 1)
//	{
//		std::string temp = argv[1];
//		max_files = boost::lexical_cast<int>(argv[1]);
//	}
//	
//	for (int i = 0; i<max_files; ++i)
//	{
//		std::stringstream ss;
//		ss << target_file_path;
//		ss << i;
//		ss << ".txt";
//
//		
//		//TODO: que no arroje error cuando el archivo existe...
//		//TODO: si el archivo existe, borrarlo...
//		boost::filesystem::copy_file( source_file_path, ss.str() );
//	}
//
//	std::cout << "DELETING..." << std::endl;
//
//	for (int i = 0; i<max_files; ++i)
//	{
//		std::stringstream ss;
//		ss << target_file_path;
//		ss << i;
//		ss << ".txt";
//
//		boost::filesystem::remove(ss.str());
//	}
//
//	std::cout << "Press Enter to Exit" << std::endl;
//	std::cin.get();
//
//	return 0;
//}
