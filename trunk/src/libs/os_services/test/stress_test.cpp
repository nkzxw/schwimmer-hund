//#include <iostream>
//#include <sstream>
//#include <string>
//
//#include <boost/filesystem.hpp>
//#include <boost/lexical_cast.hpp>
//
//const int default_max_files = 10000;
//const std::string default_dir = "C:\\temp1\\";
////std::string path = "C:\\temp1";
////std::string path = "J:\\temp1";
////std::string path = "/home/fernando/temp1";
//
//int main(int argc, char* argv[] )
//{
//
//	std::string file_name = "a";
//	std::string file_ext = ".txt";
//
//	std::string dir = default_dir;
//	int max_files = default_max_files;
//
//	if (argc > 1)
//	{
//		std::string dir = argv[1];
//		//max_files = boost::lexical_cast<int>(argv[1]);
//	}
//
//	if (argc > 2)
//	{
//		//std::string temp = argv[2];
//		max_files = boost::lexical_cast<int>(argv[2]);
//	}
//
//	
//	std::string source_file_path = dir + file_name + file_ext;
//
//	for (int i = 0; i<max_files; ++i)
//	{
//		std::stringstream ss;
//		ss << dir;
//		ss << file_name;
//		ss << i;
//		ss << file_ext;
//
//		
//		//TODO: que no arroje error cuando el archivo existe...
//		//TODO: si el archivo existe, borrarlo...
//		boost::filesystem::copy_file( source_file_path, ss.str() );
//	}
//
//
//	std::cout << "Press Enter to Exit" << std::endl;
//	std::cin.get();
//
//	return 0;
//}
