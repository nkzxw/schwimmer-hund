//TODO: comentarios

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/random.hpp>
#include <boost/thread.hpp>

#include "test_paths_platform_selector.hpp" //definition of the paths




void edit_file( std::string file_name )
{
	//std::ofstream file_stream;
	//file_stream.open (file_name.c_str());
	std::ofstream file_stream( file_name.c_str(), std::ios_base::app);
	file_stream << "." << std::endl;
	file_stream.flush();
	file_stream.close();

	//TODO: esto fue una solución temporal para verificar que FSM en Linux cuando habia 2 o más eventos CHANGES simultaneos al mismo archivo algunos se los morfaba.
	//      con esta prueba le dimos un poco de tiempo entre CHANGE y CHANGE y asi se comprobó que el problema está en que el buffer del watcher parece que revienta.
	//      para solucionarlo, vamos a incrementar el tamaño del buffer a ver si con eso alcanza.
	//      Igualmente en una maquina más veloz, tendria nuevamente el mismo problema ya que el buffer no alcanzaria... (o si, porque el procesamiento del FSM seria más rapido tambien)

	//boost::system_time time = boost::get_system_time();
	//time += boost::posix_time::milliseconds(100);
	//boost::thread::sleep(time);
}


void stress_thread( const std::string& dir, const std::string& instructions_file_path )
{
	std::cout << "Press Enter to begin with the process" << std::endl;
	std::cin.sync();
	std::cin.clear();
	std::cin.get();

	//ptime now = second_clock::local_time();
	////std::cout << to_simple_string(now) << std::endl;
	//std::cout << now << std::endl;


	std::string source_file_path = dir + file_name + file_ext;


	std::ofstream log_file;
	log_file.open ("log_file.txt"); //TODO: esta hardcodeado

	std::ifstream instructions_file;
	instructions_file.open ( instructions_file_path.c_str() );

	std::string line;

	while ( !std::getline(instructions_file, line).eof() )
	{
		std::cout << "."; //progress
		std::cout.flush();

		std::vector<std::string> strs;
		boost::split(strs, line, boost::is_any_of("|"));

		if ( strs.size() > 0 )
		{
			int action = boost::lexical_cast<int>(strs[0]);
			

			switch (action)
			{
			case action_type::create:
				{
					if ( strs.size() > 1 )
					{
						boost::filesystem::copy_file( source_file_path, strs[1] );
						//ptime now = second_clock::local_time();
						ptime now = microsec_clock::local_time();
						
						//log_file << to_iso_string(now) << " - Action: CREATED - File: '" << strs[1] << "'" << std::endl;
						log_file << " - Action: CREATED - File: '" << strs[1] << "'" << std::endl;

						

					}
					break;
				}
			case action_type::change:
				{
					if ( strs.size() > 1 )
					{
						edit_file (strs[1]);
						ptime now = microsec_clock::local_time();
						//log_file << to_iso_string(now) << " - Action: CHANGED - File: '" << strs[1] << "'" << std::endl;
						log_file << " - Action: CHANGED - File: '" << strs[1] << "'" << std::endl;
					}
					break;
				}
			case action_type::rename:
				{
					if ( strs.size() > 2 )
					{
						boost::filesystem::rename(strs[1], strs[2]);
						ptime now = microsec_clock::local_time();
						//log_file << to_iso_string(now) << " - Action: RENAMED - Source File: '" << strs[1] << "' - Target File: '" << strs[2] << "'" << std::endl;
						log_file << " - Action: RENAMED - Source File: '" << strs[1] << "' - Target File: '" << strs[2] << "'" << std::endl;
					}
					break;
				}
			case action_type::remove:
				{
					if ( strs.size() > 1 )
					{
						boost::filesystem::remove( strs[1] );
						ptime now = microsec_clock::local_time();
						//log_file << to_iso_string(now) << " - Action: REMOVED - File: '" << strs[1] << "'" << std::endl;
						log_file << " - Action: REMOVED - File: '" << strs[1] << "'" << std::endl;
					}
					break;
				}
			}
		}
	}


	log_file.close();
	instructions_file.close();

	std::cout << "END..." << std::endl;
}


int main(int argc, char* argv[] )
{
	std::string dir = default_dir;
	std::string instruction_file_path = default_instruction_file_path;

	if (argc > 1)
	{
		dir = argv[1];
	}

	if (argc > 2)
	{
		instruction_file_path = argv[2];
	}

	//boost::thread thrd( boost::bind(&stress_thread, dir, instruction_file_path) );
	//thrd.join();

	stress_thread(dir, instruction_file_path);

	std::cout << "Press Enter to Exit" << std::endl;
	std::cin.sync();
	std::cin.clear();
	std::cin.get();

	return 0;
}
