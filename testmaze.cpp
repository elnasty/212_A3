
//
// To compile this program into an executable named 'testmaze', ...
//
// 1) Ensure all downloaded files are in the SAME folder
// 
// 2) Use the following command : 
//    g++ testmaze.cpp Maze.o -o testmaze
//

#include "Maze.h"
#include "Assignm3_Utils.h"

const std::string filename = "mazedata.txt";

int main (void)
{

	Assignm3::Maze *maze = new Assignm3::Maze ();

	maze->LoadMaze (filename);

	std::cout << std::endl;
	std::cout << "Length  of maze in '" << filename << "' : " << maze->getLength();
	std::cout << std::endl;
	std::cout << "Breadth of maze in '" << filename << "' : " << maze->getBreadth();
	std::cout << std::endl;

	Point startLoc = maze->getStartLocation();
	Point endLoc   = maze->getEndLocation();

	std::cout << "Start Location of maze in '" << filename << "' : ";
	startLoc.display();
	std::cout << std::endl;

	std::cout << "End   Location of maze in '" << filename << "' : ";
	endLoc.display();
	std::cout << std::endl;

	std::cout << std::endl;
	std::cout << "Displaying Maze contents ...";
	std::cout << std::endl;
	maze->DisplayMaze ();

	std::cout << std::endl;
	std::cout << std::endl;

}


