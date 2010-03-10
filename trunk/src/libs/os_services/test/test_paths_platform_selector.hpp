#include <boost/date_time/posix_time/posix_time.hpp>

using namespace boost::posix_time;


#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__) || defined(__FreeBSD__) // || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)

//const std::string default_dir = "/home/fernando/temp1/";
const std::string default_dir = "./temp1/";

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)

//const std::string default_dir = "C:\\temp1\\";
const std::string default_dir = ".\\temp1\\";

#elif defined(__CYGWIN__)
#  error Platform not supported
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#  error No test for the moment
#else
#  error Platform not supported
#endif


const int default_max_files = 10000;

const std::string file_name = "a";
const std::string file_ext = ".txt";


void stress_file_removal( const std::string& dir, int max_files )
{
	ptime now = second_clock::local_time();
	//std::cout << to_simple_string(now) << std::endl;
	std::cout << now << std::endl;

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
		//TODO: thread.sleep

	}

	now = second_clock::local_time();
	//std::cout << to_simple_string(now) << std::endl;
	std::cout << now << std::endl;

}

void stress_file_creation(	const std::string& dir, int max_files )
{
	ptime now = second_clock::local_time();
	//std::cout << to_simple_string(now) << std::endl;
	std::cout << now << std::endl;


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


	now = second_clock::local_time();
	//std::cout << to_simple_string(now) << std::endl;
	std::cout << now << std::endl;
}

void stress_thread( const std::string& dir, int max_files )
{
	std::cout << "Press Enter to begin with file creation process" << std::endl;
	std::cin.get();

	std::cout << "CREATING " << max_files << " files, please wait..." << std::endl;
	stress_file_creation(dir, max_files);


	std::cout << "Press Enter to begin with file removal process" << std::endl;
	std::cin.get();


	std::cout << "DELETING " << max_files << " files, please wait..." << std::endl;
	stress_file_removal(dir, max_files);
	std::cout << "END..." << std::endl;
}
