//TODO: comentarios

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>

#include "test_paths_platform_selector.hpp" //definition of the paths


struct action_type
{
	static const int create = 0;
	static const int change = 1;
	static const int rename = 2;
	static const int remove = 3;
};
//TODO: actiones que impliquen la modificacion de atributos del archivo.
//           ver como hace FileSystemWatchet para capturar estos eventos...




// This is a typedef for a random number generator.
// Try boost::mt19937 or boost::ecuyer1988 instead of boost::minstd_rand
typedef boost::minstd_rand base_generator_type;
//typedef boost::mt19937 base_generator_type;
//typedef boost::ecuyer1988 base_generator_type;



template <typename Vector>
std::string get_random_file_from_vector(base_generator_type& generator, const Vector& vec)
{

	if ( vec.size() == 0 )
	{
		return "";
	}

	boost::uniform_int<> vec_dist(0, vec.size()-1);	////boost::mt19937 rng;
	boost::variate_generator<base_generator_type&, boost::uniform_int<> > get_pos(generator, vec_dist);

	int pos = get_pos();

	//if ( pos >= vec.size() )
	//{
	//	return "";
	//}

	return vec[pos];
}

//
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

	//typename Vector::iterator it2 = (vec.begin() + pos);

	return (vec.begin() + pos);
}

void edit_file( std::string file_name )
{
	std::ofstream file_stream;
	file_stream.open (file_name.c_str());
	file_stream << "." << std::endl;
	file_stream.close();
}


void stress_random_thread( const std::string& dir, int max_operations )
{
	std::vector<std::string> file_list;

	
	std::cout << "Press Enter to begin with file ??? process" << std::endl;
	std::cin.get();


	std::ofstream log_file;
	log_file.open ("log_file_generator.txt");

	std::string source_file_path = dir + file_name + file_ext;

	base_generator_type generator(42u); //boost::mt19937 rng;
	base_generator_type vector_generator(42u); //boost::mt19937 rng;
	boost::uniform_int<> dist_four(0, 3);
	boost::variate_generator<base_generator_type&, boost::uniform_int<> > get_action(generator, dist_four);

	//generator.seed(static_cast<unsigned int>(std::time(0)));

	//// Define a uniform random number distribution of integer values between
	//// 1 and 6 inclusive.
	//typedef boost::uniform_int<> distribution_type;
	//typedef boost::variate_generator<base_generator_type&, distribution_type> gen_type;
	//gen_type die_gen(generator, distribution_type(1, 6));

	//// If you want to use an STL iterator interface, use iterator_adaptors.hpp.
	//// Unfortunately, this doesn't work on SunCC yet.
	//boost::generator_iterator<gen_type> die(&die_gen);
	//for(int i = 0; i < 10; i++)
	//	std::cout << *die++ << " ";
	//std::cout << '\n';

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

				boost::filesystem::copy_file( source_file_path, ss.str() );

				ptime now = second_clock::local_time();

				file_list.push_back( ss.str() );
				log_file << now << " - Action: CREATED - File: '" << ss.str() << "'" << std::endl;

				break;
			}
			case action_type::change:
			{
				if ( file_list.size() > 0)
				{
					std::vector<std::string>::iterator it = get_random_pos_from_vector(vector_generator, file_list);
					
					edit_file (*it);

					ptime now = second_clock::local_time();
					log_file << now << " - Action: CHANGED - File: '" << *it << "'" << std::endl;
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

					boost::filesystem::rename(*it, target.str());
				

					ptime now = second_clock::local_time();
					log_file << now << " - Action: RENAMED - Source File: '" << *it << "' - Target File: '" << target.str() << "'" << std::endl;


					//TODO: renombrar en el vector.
					*it = target.str();

				}

				break;
			}
			case action_type::remove:
			{
				if ( file_list.size() > 0)
				{
					std::vector<std::string>::iterator it = get_random_pos_from_vector(vector_generator, file_list);

					boost::filesystem::remove( *it );

					ptime now = second_clock::local_time();
					log_file << now << " - Action: REMOVED - File: '" << *it << "'" << std::endl;

					file_list.erase(std::remove(file_list.begin(), file_list.end(), *it), file_list.end()); 

				}

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

	stress_random_thread(dir, max_files);

	std::cout << "Press Enter to Exit" << std::endl;
	std::cin.get();

	return 0;
}
