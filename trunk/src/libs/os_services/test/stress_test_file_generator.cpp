//TODO: comentarios

#include <iostream>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include "test_paths_platform_selector.hpp" //definition of the paths


int main(int argc, char* argv[] )
{
	std::string dir = default_dir;
	int max_files = default_max_files;

	if (argc > 1)
	{
		dir = argv[1];
		//max_files = boost::lexical_cast<int>(argv[1]);
	}

	if (argc > 2)
	{
		//std::string temp = argv[2];
		max_files = boost::lexical_cast<int>(argv[2]);
	}

	//boost::thread thrd( boost::bind(&stress_thread, dir, max_files) );
	//thrd.join();

	stress_thread(dir, max_files);

	std::cout << "Press Enter to Exit" << std::endl;
	std::cin.sync();
	std::cin.get();

	return 0;
}
