//TODO: comentarios

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>

#include "test_paths_platform_selector.hpp" //definition of the paths

typedef boost::minstd_rand base_generator_type;
//typedef boost::mt19937 base_generator_type;
//typedef boost::ecuyer1988 base_generator_type;


//TODO: pasar a archivo hpp comun...
template <typename Vector>
typename Vector::iterator get_random_pos_from_vector(base_generator_type& generator, Vector& vec) //const Vector& vec
{
	if ( vec.size() == 0 )
	{
		return vec.end();
	}

	boost::uniform_int<> vec_dist(0, vec.size()-1);	////boost::mt19937 rng;
	boost::variate_generator<base_generator_type&, boost::uniform_int<> > get_pos(generator, vec_dist);

	int pos = get_pos();

	return (vec.begin() + pos);
}



void stress_random_thread( const std::string& dir, int max_operations )
{
	std::vector<std::string> file_list;
	
	std::cout << "Press Enter to begin with the process" << std::endl;
	std::cin.sync();
	std::cin.get();


	std::ofstream instructions_file;
	instructions_file.open ("instructions_file.txt"); //TODO: agregar la fecha al nombre

	std::string source_file_path = dir + file_name + file_ext;


	//TODO: usar el tiempo actual como semilla... (VER EJEMPLO en libs de Boost)
	base_generator_type generator(42u);			//boost::mt19937 rng;
	base_generator_type vector_generator(42u);	//boost::mt19937 rng;
	boost::uniform_int<> dist_four(0, 3);
	boost::variate_generator<base_generator_type&, boost::uniform_int<> > get_action(generator, dist_four);

	//generator.seed(static_cast<unsigned int>(std::time(0)));


	for (int i = 0; i<max_operations; ++i)
	{
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

				file_list.push_back( ss.str() );
				instructions_file << action << "|" << ss.str() << std::endl;

				break;
			}
			case action_type::change:
			{
				if ( file_list.size() > 0)
				{
					std::vector<std::string>::iterator it = get_random_pos_from_vector(vector_generator, file_list);
					instructions_file << action << "|" << *it << std::endl;
				}
				break;
			}
			case action_type::rename:
			{
				if ( file_list.size() > 0)
				{
					std::vector<std::string>::iterator it = get_random_pos_from_vector(vector_generator, file_list);

					std::stringstream target;
					target << dir;
					target << file_name;
					target << i;
					target << file_ext;

					instructions_file << action << "|" << *it << "|" << target.str() << std::endl;

					*it = target.str();
				}

				break;
			}
			case action_type::remove:
			{
				if ( file_list.size() > 0)
				{
					std::vector<std::string>::iterator it = get_random_pos_from_vector(vector_generator, file_list);
					instructions_file << action << "|" << *it << std::endl;

					file_list.erase(std::remove(file_list.begin(), file_list.end(), *it), file_list.end()); 
				}

				break;
			}

		}



	}

	instructions_file.close();

	std::cout << "END..." << std::endl;
}


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

	stress_random_thread(dir, max_files);

	std::cout << "Press Enter to Exit" << std::endl;
	std::cin.sync();
	std::cin.get();

	return 0;
}
