using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace test_dotnet_fsw_stress_watcher
{
    class Program
    {

        TextWriter logFile;

        // Event Handlers
        private static void OnChanged(object source, FileSystemEventArgs e)
        {
            
            logFile.WriteLine( DateTime.Now + " - Action: CHANGED - File: '" + e.FullPath + "'" );
        }

        private static void OnCreated(object source, FileSystemEventArgs e)
        {
	        logFile.WriteLine( DateTime.Now + " - Action: CREATED - File: '" + e.FullPath + "'" );
        }

        private static void OnDeleted(object source, FileSystemEventArgs e)
        {
	        logFile.WriteLine( DateTime.Now + " - Action: REMOVED - File: '" + e.FullPath + "'" );

        }

        private static void OnRenamed(object source, RenamedEventArgs e)
        {
            logFile.WriteLine( DateTime.Now + " - Action: RENAMED - Source File: '" + e.OldFullPath + "' - Target File: '" + e.FullPath + "'" );
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



            logFile = new StreamWriter("log_file_watcher.txt"); //TODO: esta hardcodeado


            
            
            FileSystemWatcher watcher = new FileSystemWatcher();

            watcher.Path = dir;
            
	        watcher.NotifyFilter = NotifyFilters.LastAccess | NotifyFilters.LastWrite | NotifyFilters.FileName | NotifyFilters.DirectoryName;
			
	        //watcher.set_filter("*.txt"); //TODO: implementar este filtro
			
            watcher.Changed += new FileSystemEventHandler(OnChanged);
            watcher.Created += new FileSystemEventHandler(OnChanged);
            watcher.Deleted += new FileSystemEventHandler(OnChanged);
            watcher.Renamed += new RenamedEventHandler(OnRenamed);

	        watcher.EnableRaisingEvents = true;




            // Wait for the user to quit the program.
            Console.WriteLine("Press \'q\' to quit the sample.");
            while (Console.Read() != 'q') ;



	        logFile.Close();

	        Console.WriteLine("Press Enter to Exit" );
	        std::cin.get();

	        return 0;
        }

        static void Main(string[] args)
        {
        }
    }
}
