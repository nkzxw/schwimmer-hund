//TODO: comentarios

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>

#include "test_paths_platform_selector.hpp" //definition of the paths


//void stress_file_creation( const std::string& dir, int max_files )
//{
//	ptime now = second_clock::local_time();
//	//std::cout << to_simple_string(now) << std::endl;
//	std::cout << now << std::endl;
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
//		//TODO: que no arroje error cuando el archivo origen no exista...
//		//TODO: que no arroje error cuando el archivo destino exista...
//
//		boost::filesystem::copy_file( source_file_path, ss.str() );
//	}
//
//
//	now = second_clock::local_time();
//	//std::cout << to_simple_string(now) << std::endl;
//	std::cout << now << std::endl;
//}


struct action_type
{
	static const int create = 0;
	static const int change = 1;
	static const int rename = 2;
	static const int remove = 3;
};
//TODO: actiones que impliquen la modificacion de atributos del archivo.
//           ver como hace FileSystemWatchet para capturar estos eventos...


//const std::string file_name = "random_generated_file_";
//const std::string file_ext = ".txt";



template <typename Vector>
std::string get_random_file_from_vector(const Vector& vec)
{
	boost::mt19937 rng;                 
	boost::uniform_int<> four(0, vec.size());
	boost::variate_generator<boost::mt19937&, boost::uniform_int<> > get_pos(rng, four);

	int pos = get_pos();

	if ( pos > vec.size() )
	{
		return "";
	}

	return vec[pos];
}

void stress_random_thread( const std::string& dir, int max_operations )
{
	std::vector<std::string> file_list;

	
	std::cout << "Press Enter to begin with file ??? process" << std::endl;
	std::cin.get();


	std::ofstream log_file;
	log_file.open ("log_file.txt");

	std::string source_file_path = dir + file_name + file_ext;

	for (int i = 0; i<max_operations; ++i)
	{

		boost::mt19937 rng;                 
		boost::uniform_int<> four(0, 3);
		boost::variate_generator<boost::mt19937&, boost::uniform_int<> > get_action(rng, four);

		int action = get_action();


		switch (action)
		{
			case action_type::create:
			{
				std::stringstream ss;
				ss << dir;
				ss << file_name;
				ss << i;
				ss << file_ext;

				boost::filesystem::copy_file( source_file_path, ss.str() );

				ptime now = second_clock::local_time();

				file_list.push_back( ss.str() );
				log_file << now << " - Action: CREATED - File: '" << ss.str() << "'" << std::endl;

				break;
			}
			case action_type::change:
			{
				std::string source = get_random_file_from_vector(file_list);

				//TODO: modificar el contenido del archivo...
				
				ptime now = second_clock::local_time();
				log_file << now << " - Action: CHANGED - File: '" << source << "'" << std::endl;


				break;
			}
			case action_type::rename:
			{
				std::string source = get_random_file_from_vector(file_list);

				std::stringstream target;
				target << dir;
				target << file_name;
				target << i;
				target << file_ext;

				boost::filesystem::rename(source, target.str());

				ptime now = second_clock::local_time();
				log_file << now << " - Action: RENAMED - Source File: '" << source << "' - Target File: '" << target.str() << "'" << std::endl;


				break;
			}
			case action_type::remove:
			{
				std::string source = get_random_file_from_vector(file_list);

				boost::filesystem::remove( source );


				file_list.erase(std::remove(file_list.begin(), file_list.end(), source), file_list.end()); 

				ptime now = second_clock::local_time();
				log_file << now << " - Action: REMOVED - File: '" << source << "'" << std::endl;

				break;
			}

		}



	}

	log_file.close();

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

	//boost::thread thrd( boost::bind(&stress_thread, dir, max_files) );
	//thrd.join();

	stress_thread(dir, max_files);

	std::cout << "Press Enter to Exit" << std::endl;
	std::cin.get();

	return 0;
}
