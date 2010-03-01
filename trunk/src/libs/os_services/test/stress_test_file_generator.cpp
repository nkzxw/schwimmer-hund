//TODO: comentarios

#include <iostream>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "test_paths_platform_selector.hpp" //definition of the paths

const int default_max_files = 10000;

const std::string file_name = "a";
const std::string file_ext = ".txt";







void stress_file_removal( const std::string& dir, int max_files )
{

	for (int i = 0; i<max_files; ++i)
	{
		//std::stringstream ss;
		//ss << target_file_path;
		//ss << i;
		//ss << ".txt";

		std::stringstream ss;
		ss << dir;
		ss << file_name;
		ss << i;
		ss << file_ext;



		boost::filesystem::remove(ss.str());
	}
}

void stress_file_creation(	const std::string& dir, int max_files )
{
	std::string source_file_path = dir + file_name + file_ext;

	for (int i = 0; i<max_files; ++i)
	{
		std::stringstream ss;
		ss << dir;
		ss << file_name;
		ss << i;
		ss << file_ext;

		//TODO: que no arroje error cuando el archivo origen no exista...
		//TODO: que no arroje error cuando el archivo destino exista...

		boost::filesystem::copy_file( source_file_path, ss.str() );
	}

	

}

void stress_thread( const std::string& dir, int max_files )
{
	std::cout << "CREATING " << max_files << " files, please wait..." << std::endl;
	stress_file_creation(dir, max_files);
	std::cout << "DELETING " << max_files << " files, please wait..." << std::endl;
	stress_file_removal(dir, max_files);
	std::cout << "END..." << std::endl;
}

int main(int argc, char* argv[] )
{
	std::string dir = default_dir;
	int max_files = default_max_files;

	if (argc > 1)
	{
		std::string dir = argv[1];
		//max_files = boost::lexical_cast<int>(argv[1]);
	}

	if (argc > 2)
	{
		//std::string temp = argv[2];
		max_files = boost::lexical_cast<int>(argv[2]);
	}


	std::cout << "Press Enter to begin with FILE PROCESING" << std::endl;
	std::cin.get();


	//boost::thread thrd( boost::bind(&stress_thread, dir, max_files) );
	//thrd.join();

	stress_thread(dir, max_files);

	std::cout << "Press Enter to Exit" << std::endl;
	std::cin.get();

	return 0;
}
