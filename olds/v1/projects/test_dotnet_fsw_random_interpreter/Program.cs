using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace test_dotnet_fsw_random_interpreter
{
    enum ActionType
    {
        Create = 0,
        Change = 1,
        Rename = 2,
        Remove = 3
    }


    class Program
    {

        const string defaultDir = ".\\test_dir\\temp1\\";
        const string defaultInstructionFilePath = "instructions_file.txt";
        const string fileName = "a";
        const string fileExt = ".txt";


        static void editFile(string fileName)
        {
            TextWriter fileStream = new StreamWriter(fileName);
            fileStream.WriteLine(".");
            fileStream.Close();
        }


        static void stressThread( string dir, string instructionsFilePath )
        {
            Console.WriteLine("Press Enter to begin with the process");
            Console.ReadLine();

	        string sourceFilePath = dir + fileName + fileExt; //TODO: definir

            TextWriter logFile = new StreamWriter("log_file.txt"); //TODO: esta hardcodeado
            StreamReader instructionsFile = new StreamReader( instructionsFilePath );

            string line;
            

            while((line = instructionsFile.ReadLine()) != null)
	        {
                string[] strs = line.Split('|');
                

		        if ( strs.Length > 0 )
		        {
			        ActionType action = (ActionType) Convert.ToInt32( strs[0] );
        			

			        switch (action)
			        {
			            case ActionType.Create:
				            {
					            if ( strs.Length > 1 )
					            {
                                    File.Copy( sourceFilePath, strs[1] );
                                    logFile.WriteLine(DateTime.Now + " - Action: CREATED - File: '" + strs[1] + "'");
        			            }

					            break;
				            }
			            case ActionType.Change:
				            {
					            if ( strs.Length > 1 )
					            {
						            editFile (strs[1]);

						            logFile.WriteLine( DateTime.Now + " - Action: CHANGED - File: '" + strs[1] + "'");
					            }
					            break;
				            }
			            case ActionType.Rename:
				            {
					            if ( strs.Length > 2 )
					            {
						            File.Move( strs[1], strs[2] );
						            logFile.WriteLine( DateTime.Now + " - Action: RENAMED - Source File: '" + strs[1] + "' - Target File: '" + strs[2] + "'");
					            }

					            break;
				            }
			            case ActionType.Remove:
				            {
					            if ( strs.Length > 1 )
					            {
						            File.Delete( strs[1] );
						            logFile.WriteLine( DateTime.Now + " - Action: REMOVED - File: '" + strs[1] + "'");
					            }

					            break;
				            }

			        }

		        }

	        }


	        logFile.Close();
	        instructionsFile.Close();

            Console.WriteLine("END...");
        }

        static void Main(string[] args)
        {

            string dir = defaultDir;
            string instructionFilePath = defaultInstructionFilePath;

            if (args.Length > 1)
	        {
                dir = args[1];
	        }

            if (args.Length > 2)
	        {
                instructionFilePath = args[2];
	        }

            stressThread(dir, instructionFilePath);

	        Console.WriteLine("Press Enter to Exit");
            Console.ReadLine();
        }
    }
}
