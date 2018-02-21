
#include <pthread.h>

#include "Path.h"
#include "Maze.h"
#include "SubmitMazeSoln.h"
#include "Assignm3_Utils.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// To compile, 
// 1) Ensure all the necessary header files (see above) are in the same directory
// 2) g++ Path.o Maze.o SubmitMazeSoln.o TestSubmitMazeSoln.cpp -o TestSubmitMazeSoln.exe -lpthread


// #######################################################################################

main ()
{
	Assignm3::Path path;
	Assignm3::SubmitMazeSoln sms;

	VectorOfPointStructType pathToBarrier1 = VectorOfPointStructType ();
	VectorOfPointStructType pathToBarrier2 = VectorOfPointStructType ();
	VectorOfPointStructType pathToBarrier3 = VectorOfPointStructType ();
	VectorOfPointStructType pathToBarrier4 = VectorOfPointStructType ();
	VectorOfPointStructType pathToBarrier5 = VectorOfPointStructType ();

	pathToBarrier1.push_back (Point (1,1));
	pathToBarrier1.push_back (Point (0,1));

	pathToBarrier2.push_back (Point (1,1));
	pathToBarrier2.push_back (Point (1,0));

	pathToBarrier3.push_back (Point (1,1));
	pathToBarrier3.push_back (Point (1,2));
	pathToBarrier3.push_back (Point (0,2));

	pathToBarrier4.push_back (Point (1,1));
	pathToBarrier4.push_back (Point (1,2));
	pathToBarrier4.push_back (Point (1,3));
	pathToBarrier4.push_back (Point (1,4));

	pathToBarrier5.push_back (Point (1,1));
	pathToBarrier5.push_back (Point (1,2));
	pathToBarrier5.push_back (Point (1,4));

//	path.displayPath (pathToBarrier1);
//	path.displayPath (pathToBarrier2);
//	path.displayPath (pathToBarrier3);
//	path.displayPath (pathToBarrier4);
//	path.displayPath (pathToBarrier5);

	sms.submitPathToBarrier ((pthread_t) 1234567, pathToBarrier1);
	sms.submitPathToBarrier ((pthread_t) 1234567, pathToBarrier2);
	sms.submitPathToBarrier ((pthread_t) 1234567, pathToBarrier3);
	sms.submitPathToBarrier ((pthread_t) 1234567, pathToBarrier4);
	sms.submitPathToBarrier ((pthread_t) 1234567, pathToBarrier5);
	

	VectorOfPointStructType pathToDangerArea1 = VectorOfPointStructType ();
	VectorOfPointStructType pathToDangerArea2 = VectorOfPointStructType ();
	VectorOfPointStructType pathToDangerArea3 = VectorOfPointStructType ();
	VectorOfPointStructType pathToDangerArea4 = VectorOfPointStructType ();
	VectorOfPointStructType pathToDangerArea5 = VectorOfPointStructType ();

	pathToDangerArea1.push_back (Point (1,1));
	pathToDangerArea1.push_back (Point (2,1));
	pathToDangerArea1.push_back (Point (3,1));
	pathToDangerArea1.push_back (Point (4,1));

	pathToDangerArea2.push_back (Point (1,1));
	pathToDangerArea2.push_back (Point (2,1));
	pathToDangerArea2.push_back (Point (2,2));
	pathToDangerArea2.push_back (Point (2,3));
	pathToDangerArea2.push_back (Point (3,3));
	pathToDangerArea2.push_back (Point (4,3));
	pathToDangerArea2.push_back (Point (4,2));
	pathToDangerArea2.push_back (Point (4,1));
//	pathToDangerArea2.push_back (Point (4,0));


	sms.submitPathToDangerArea ((pthread_t) 1234567, pathToDangerArea1);
	sms.submitPathToDangerArea ((pthread_t) 1234567, pathToDangerArea2);



	VectorOfPointStructType solutionPath = VectorOfPointStructType ();
	solutionPath.push_back (Point (1,1));
	solutionPath.push_back (Point (1,2));
	solutionPath.push_back (Point (1,3));
	solutionPath.push_back (Point (2,3));
	solutionPath.push_back (Point (2,4));
	solutionPath.push_back (Point (2,5));
	solutionPath.push_back (Point (3,5));
	solutionPath.push_back (Point (4,5));
	solutionPath.push_back (Point (4,4));
	solutionPath.push_back (Point (4,3));

	sms.submitSolutionPath ((pthread_t) 1234567, solutionPath);	


	sms.printSubmittedSolution ("Tan Ah Beng", "1001001");

	sms.saveSubmittedSolution ("Tan Ah Beng", "1001001");


}	// end main () ...

// #######################################################################################

/*
TESTED!! -> Point struct '==' and '=' operator overloading ...

	Point p (3, 8);

	Point p1 (3, 8);

	Point p2 (8, 8);


	if (p == p1)
	{
		std::cout << "p ";	p.display ();
		std::cout << " is EQUAL to p1 ";	p1.display ();
		std::cout << " !!" << std::endl;
	}
	else
	{
		std::cout << "p ";	p.display ();
		std::cout << " is NOT EQUAL to p1 ";	p1.display ();
		std::cout << " !!" << std::endl;
	}


	if (p == p2)
	{
		std::cout << "p ";	p.display ();
		std::cout << " is EQUAL to p2 ";	p2.display ();
		std::cout << " !!" << std::endl;
	}
	else
	{
		std::cout << "p ";	p.display ();
		std::cout << " is NOT EQUAL to p2 ";	p2.display ();
		std::cout << " !!" << std::endl;
	}


	Point p3 (8, 8);

	Point p4, p5;

	p4 = p5 = p3;
//	p5 = p4;

	std::cout << "p3 is "; p3.display (); std::cout << std::endl;
	std::cout << "p4 is "; p4.display (); std::cout << std::endl;
	std::cout << "p5 is "; p5.display (); std::cout << std::endl;



*/

// #######################################################################################

/*
TESTED !! -> isConnected() in struct Point ...

	Point p (3, 8);
	Point pLeft (2, 8);
	Point pRight (4, 8);
	Point pUp (3, 9);
	Point pDown (3, 7);

	if (p.isConnected (pLeft))
		std::cout << "p is connected to pLeft !!" << std::endl;

	if (p.isConnected (pRight))
		std::cout << "p is connected to pRight !!" << std::endl;

	if (p.isConnected (pUp))
		std::cout << "p is connected to pUp !!" << std::endl;

	if (p.isConnected (pDown))
		std::cout << "p is connected to pDown !!" << std::endl;

	if (pLeft.isConnected (pRight))
		std::cout << "pLeft is connected to pRight !!" << std::endl;
	else
		std::cout << "pLeft is NOT connected to pRight !!" << std::endl;
	
	if (pUp.isConnected (pDown))
		std::cout << "pUp is connected to pDown !!" << std::endl;
	else
		std::cout << "pUp is NOT connected to pDown !!" << std::endl;
*/

// #######################################################################################

/*
TESTED!! -> new method in Path.h 'arePathsIdentical ()' ...
	Point p (3, 8);
	Point p1 (3, 8);
	Point p2 (8, 8);

	VectorOfPointStructType path1;
	path1.push_back (p);	path1.push_back (p1);	path1.push_back (p2);

	VectorOfPointStructType path2;
	path2.push_back (p);	path2.push_back (p1);	path2.push_back (p2);


	Assignm3::Path path;

	if (path.arePathsIdentical (path1, path2))
		std::cout << "path1 is identical to path2 !!" << std::endl;
	else
		std::cout << "path1 is different from path2 !!" << std::endl;


*/

// #######################################################################################

/*
TESTED !! -> Maze constructor, and new methods () ...

	Assignm3::Maze * probMaze = new Assignm3::Maze ();
	probMaze->LoadMaze ();

	Assignm3::Maze * solnMaze = new Assignm3::Maze (probMaze->getLength(), probMaze->getBreadth(),
													probMaze->getStartLocation(), probMaze->getEndLocation());

	probMaze->DisplayMaze ();
	probMaze->DisplayInfo ();

	solnMaze->updateMaze (Point (0,0), Assignm3::BARRIER_INT);
	solnMaze->updateMaze (Point (0,1), Assignm3::BARRIER_INT);
	solnMaze->updateMaze (Point (1,0), Assignm3::BARRIER_INT);

	solnMaze->DisplayMaze ();
	solnMaze->DisplayInfo ();


*/

// #######################################################################################

/*
TESTED !! -> 


*/

// #######################################################################################





