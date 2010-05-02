////TODO: comentarios
//
//#include <iostream>
//#include <sstream>
//#include <string>
//
//#include <boost/filesystem.hpp>
//#include <boost/lexical_cast.hpp>
//
//const int default_max_files = 10000;
//
//#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) || defined(__FreeBSD__) // || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
//
//const std::string default_dir = "/home/fernando/temp1/";
//
//#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
//
//const std::string default_dir = "C:\\temp1\\";
//
//#elif defined(__CYGWIN__)
//#  error Platform not supported
//#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
//#  error No test for the moment
//#else
//#  error Platform not supported
//#endif
//
//const std::string file_name = "a";
//const std::string file_ext = ".txt";
//
//int main(int argc, char* argv[] )
//{
//	std::string dir = default_dir;
//	int max_files = default_max_files;
//
//	if (argc > 1)
//	{
//		dir = argv[1];
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
//	std::cout << "DELETING..." << std::endl;
//
//	for (int i = 0; i<max_files; ++i)
//	{
//		//std::stringstream ss;
//		//ss << target_file_path;
//		//ss << i;
//		//ss << ".txt";
//
//		std::stringstream ss;
//		ss << dir;
//		ss << file_name;
//		ss << i;
//		ss << file_ext;
//
//
//
//		boost::filesystem::remove(ss.str());
//	}
//
//	std::cout << "Press Enter to Exit" << std::endl;
//	std::cin.get();
//
//	return 0;
//}
