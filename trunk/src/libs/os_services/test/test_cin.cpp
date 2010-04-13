//TODO: comentarios

#include <fstream>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>



int main(int argc, char* argv[] )
{

	//std::cout << "Press Enter to Stop Monitoring... XXXXXX ......" << std::endl;
	//std::cin.sync();
	//std::cin.clear();
	////std::cin.get();
	//std::cin.peek();
	////std::cin.getline();
	////std::cin.good();
	////std::cin.sync_with_stdio();

	// -------------> OK



	try
	{

		std::cout << "Press Enter to Stop Monitoring... XXXXXX ......" << std::endl;
		std::cin.sync();
		std::cin.clear();
		//std::cin.get();
		std::cin.peek();
		//std::cin.getline();
		//std::cin.good();
		//std::cin.sync_with_stdio();

	}
	catch (std::runtime_error& e)
	{
		std::cout << "EXCEPTION: " << e.what() << std::endl;
	}
	catch (std::invalid_argument& e)
	{
		std::cout << "EXCEPTION: " << e.what() << std::endl;
	}




	std::cout << "Press Enter to Exit" << std::endl;
	std::cin.sync();
	std::cin.get();

	return 0;
}
