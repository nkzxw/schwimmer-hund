//TODO: comentarios

#include <iostream>
#include <sstream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/os_services/file_system_monitor.hpp>

#include "test_paths_platform_selector.hpp" //definition of the paths

using namespace boost::os_services;


typedef std::list<std::string> collection_type;
//typedef std::vector<std::string> collection_type;
collection_type file_collection;



// Event Handlers
//static void OnChanged( filesystem_event_args e ) // object source,
//{
//	std::cout << "Changed: '" << e.full_path << "'" << std::endl;
//}

static void on_created( filesystem_event_args e ) // object source,
{
	file_collection.push_back(e.name);
	//std::cout << "Created: '" << e.full_path << "'" << std::endl;
}

//static void OnDeleted( filesystem_event_args e ) // object source,
//{
//	std::cout << "Deleted: '" << e.full_path << "'" << std::endl;
//}

//static void OnRenamed( renamed_event_args e ) // object source,
//{
//	std::cout << "File: '" << e.old_full_path << "' renamed to: '" << e.full_path  << "'" << std::endl;
//}




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


	{
		//file_collection.reserve(max_files);


		boost::shared_ptr<file_system_monitor> monitor;

		try
		{
			monitor.reset(new file_system_monitor);

			monitor->add_directory( dir );
			

			//TODO: mapear los notify filters de Windows con otras plataformas...

			monitor->set_notify_filters( notify_filters::last_access | notify_filters::last_write | notify_filters::file_name | notify_filters::directory_name );
			monitor->set_filter("*.txt"); //TODO: implementar este filtro
			//monitor->set_changed_event_handler( OnChanged );
			monitor->set_created_event_handler( on_created );
			//monitor->set_deleted_event_handler( OnDeleted );
			//monitor->set_renamed_event_handler( OnRenamed );

			monitor->start();

			//std::cout << "Press Enter to Stop Monitoring..." << std::endl;
			//std::cin.sync();
			//std::cin.clear();
			//std::cin.get();

		}
		catch ( const std::runtime_error& e )
		{
			std::cout << "EXCEPTION: " << e.what() << std::endl;
		}
		catch ( const std::invalid_argument& e )
		{
			std::cout << "EXCEPTION: " << e.what() << std::endl;
		}


		//boost::thread thrd( boost::bind(&stress_thread, dir, max_files) );
		//thrd.join();

		stress_thread(dir, max_files);

	}


	std::cout << "Files: " << file_collection.size() << std::endl;

	int previous_file_index = -1;
	for (collection_type::const_iterator it = file_collection.begin(); it!=file_collection.end(); ++it)
	{
		//int tempPos = (*it).find('.')-1;
		std::string temp = (*it).substr( 1, (*it).find('.')-1 );
		int index = boost::lexical_cast<int>(temp);

		if ( previous_file_index == index-1 )
		{
			std::cout << *it << std::endl;
		}
		else
		{
			std::cout << "------------------------" << std::endl;
		}

		previous_file_index  = index;
	}



	std::cout << "Press Enter to Exit" << std::endl;
	std::cin.sync();
	std::cin.clear();
	std::cin.get();

	return 0;
}
