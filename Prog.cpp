#include <iostream>
#include <pthread.h>
#include "Path.h"
#include "Maze.h"
#include "SubmitMazeSoln.h"
#include "Assignm3_Utils.h"
#include "Assignm3.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <ctime>

/* utilize namespace to reduce stuff to type */
using namespace std;
using namespace Assignm3;

/* Global shared resources to be used */

const int MAX_NO_THREAD = 2;
const char fileName [] = "mazedata.txt";

SubmitMazeSoln sms;
Maze maze;
Maze *discoveredMaze;

Path path;
VectorOfPointStructType invalidLoc;

bool solutionFound = false;
int noActiveThread = 0;
int threadNo = 0;

/* Function declaration */
void *mazeCrawler (void*);

int main ()
{
	/* Performs administrative tasks in preparation of navigation of maze */
	maze.LoadMaze (fileName);
	PathFinderResource finder;
	discoveredMaze = new Maze (maze.getLength(), maze.getBreadth(), maze.getStartLocation(), maze.getEndLocation());

	/* Attempts to randomize the start direction */
	srand (time (NULL));
	
	/* Loop is done till a solution is found and number of active threads is incremented to prevent */
	/* too much threads from being spawned */
	while (!solutionFound)
	{
		if (noActiveThread < MAX_NO_THREAD)
		{
			for (int i =0; i < MAX_NO_THREAD; i++)
			{
				pthread_create (&finder.activeThreadArray [threadNo++], NULL, mazeCrawler, (void *) threadNo);
			        noActiveThread++;

			        cout << "Thread " << THREAD_NAMES [threadNo] << " has been spawn!" << endl;
			}
		}
    
	}

	threadNo = 0;

	/* Goes through the pthread_join and waits for the thread to quit */
	for (int i = 0; i < MAX_NO_THREAD; i++)
	{
		pthread_join (finder.activeThreadArray [threadNo], NULL);
	}

	/* Prints and save the submited solution to predefined name and student ID */
	sms.printSubmittedSolution ("Malgene Teo", "5563215");
	sms.saveSubmittedSolution ("Malgene Teo", "5563215");
}

void *mazeCrawler (void *arg)
{
	/* Adminstrative stuff such as variable declarations to be used later */
	bool run = true, invalidPath = false, goUp = true, goDown = true, goLeft = true, goRight = true;
	int threadNo = (int) arg, toGo = rand () % 4, deadEndCount = 0;

	Point curLoc = maze.getStartLocation ();
	Point up, down, left, right;
	VectorOfPointStructType traversedPath;

	/* Logs first location to the traversed path */
	traversedPath.push_back (curLoc);

	while (run)
	{
		invalidPath = false, goUp = true, goDown = true, goLeft = true, goRight = true;
		deadEndCount = 0;
		Point up (curLoc.x, curLoc.y - 1), down (curLoc.x, curLoc.y + 1), left (curLoc.x - 1, curLoc.y), right (curLoc.x + 1, curLoc.y);

		/* Checks to see if solution is found and quits if it is*/
		if (solutionFound)
		{
			noActiveThread--;
			pthread_exit(NULL);
			threadNo--;
		}
		
		/* About to start utilizing global resources, locks it */
		pthread_mutex_lock (&thread_mutex);

		/* Checks to see if direction has been explored before and increment a counter to determine if should be backtracked */
		/* checks if location is a barrier and checks if direction is the end */
		if (path.isLocationInPath (up, invalidLoc) || path.isLocationInPath (up, traversedPath))
		{
			goUp = false;
			deadEndCount++;
		}

		else
		{
			if(maze.IsThereBarrier(up))
			{
				invalidLoc.push_back (up);

				traversedPath.push_back(up);        
				sms.submitPathToBarrier((pthread_t)threadNo, traversedPath);
				traversedPath.pop_back();
				discoveredMaze->updateMaze(up, BARRIER_INT);

				goUp = false;
				deadEndCount++;
			}

			if(up == maze.getEndLocation ())
		   		toGo = 0;
        	}

		/* Checks to see if direction has been explored before and increment a counter to determine if should be backtracked */
		/* checks if location is a barrier and checks if direction is the end */
		if (path.isLocationInPath (down, invalidLoc) || path.isLocationInPath (down, traversedPath))
		{
			goDown = false;
			deadEndCount++;
		}

		else
		{
			if (maze.IsThereBarrier(down))
			{
				invalidLoc.push_back (down);
				traversedPath.push_back(down);    
				sms.submitPathToBarrier((pthread_t)threadNo, traversedPath);   
				traversedPath.pop_back();
				discoveredMaze->updateMaze(down, BARRIER_INT);

				goDown = false;
				deadEndCount++;
			}
			
			if (down == maze.getEndLocation ())
		    		toGo = 1;
		}

		/* Checks to see if direction has been explored before and increment a counter to determine if should be backtracked */
		/* checks if location is a barrier and checks if direction is the end */
		if (path.isLocationInPath (left, invalidLoc) || path.isLocationInPath (left, traversedPath))
		{
			goLeft = false;      
			deadEndCount++;
		}
	      
		else
		{
			if (maze.IsThereBarrier(left))
			{
				invalidLoc.push_back (left);
				traversedPath.push_back(left);   
				sms.submitPathToBarrier((pthread_t)threadNo, traversedPath);
				traversedPath.pop_back();
				discoveredMaze->updateMaze(left, BARRIER_INT);

				goLeft = false;
				deadEndCount++;
			}

			if (left == maze.getEndLocation ())
		   		 toGo = 2;
		}

		/* Checks to see if direction has been explored before and increment a counter to determine if should be backtracked */
		/* checks if location is a barrier and checks if direction is the end */
		if (path.isLocationInPath (right, invalidLoc) || path.isLocationInPath (right, traversedPath))
		{
			goRight = false;
			deadEndCount++;
		}
		
		else
		{
			if(maze.IsThereBarrier(right))
			{
				invalidLoc.push_back (right);
				traversedPath.push_back(right);
				sms.submitPathToBarrier((pthread_t)threadNo, traversedPath);
				traversedPath.pop_back();
				discoveredMaze->updateMaze(right, BARRIER_INT);

				goRight = false;
				deadEndCount++;
			}

			if (right == maze.getEndLocation ())
		    		toGo = 3;
		}

		pthread_mutex_unlock(&thread_mutex);

		/* if surrounded by dead end, store location and backtraces to previous location */
		if(deadEndCount > 3)
		{
			cout << "Thread " << THREAD_NAMES [threadNo] <<" hits a DEAD END at ";
			curLoc.display();
			cout << "!" << endl;

			pthread_mutex_lock(&thread_mutex);
			invalidLoc.push_back(curLoc);
			pthread_mutex_unlock(&thread_mutex);
			traversedPath.pop_back();
		}

		/* Otherwise, determine next course of action, till path is in the invalid vector */
		else
		{
			do
			{
				invalidPath = false;

				switch (toGo)
				{
					case 0:
						if(!goUp)
							invalidPath = true;
						break;

					case 1:
						if (!goDown)
							invalidPath = true;
						break;

					case 2:
						if(!goLeft)
							invalidPath = true;
						break;

					case 3:
						if(!goRight)
							invalidPath = true;
						break;

				}

					if(invalidPath)
						toGo++;
                    
               
       					if(toGo > 3)
                    				toGo = 0;

   			}
			while (invalidPath);

		/* moves the thread to the new location that has been set above */
		if (toGo == 0)
		{
			curLoc = up;
			traversedPath.push_back(curLoc);
			pthread_mutex_lock(&thread_mutex);
			discoveredMaze->ShowPathGraphically(traversedPath);	
			pthread_mutex_unlock(&thread_mutex);
		}
        
                else if (toGo == 1)
		{
			curLoc = down;
			traversedPath.push_back(curLoc);
			pthread_mutex_lock(&thread_mutex);
			discoveredMaze->ShowPathGraphically(traversedPath);	
			pthread_mutex_unlock(&thread_mutex);
		}

                else if (toGo == 2)
		{
			curLoc = left;
			traversedPath.push_back(curLoc);
			pthread_mutex_lock(&thread_mutex);
			discoveredMaze->ShowPathGraphically(traversedPath);	
			pthread_mutex_unlock(&thread_mutex);
		}

		else if (toGo == 3)
		{
			curLoc = right;
			traversedPath.push_back(curLoc);
			pthread_mutex_lock(&thread_mutex);
			discoveredMaze->ShowPathGraphically(traversedPath);	
			pthread_mutex_unlock(&thread_mutex);
		}

		/* checks to see if the new location is a danger area, and kills the thread if it is */
		if (maze.IsThereDanger (curLoc))
		{
			cout << "Thread '" << THREAD_NAMES [threadNo] << "' is dead at ";
			curLoc.display(); 
			cout << "\nIt's sacrifice shall not be in vain!\n" << endl;

			pthread_mutex_lock(&thread_mutex);

			sms.submitPathToDangerArea((pthread_t)threadNo, traversedPath);
			invalidLoc.push_back (curLoc);
			discoveredMaze->updateMaze(curLoc, DANGER_INT);

			pthread_mutex_unlock(&thread_mutex);

			run = false;
		}

        }

	/* checks to see if the new location is the end goal, and reports the traversed path and kills the thread */
        if(curLoc == maze.getEndLocation ())
        {
		cout << "Thread " << THREAD_NAMES [threadNo] <<" has found a solution! Well done!!." << endl;
		pthread_mutex_lock(&thread_mutex);
		sms.submitSolutionPath ((pthread_t) threadNo, traversedPath);
		solutionFound = true;
		pthread_mutex_unlock(&thread_mutex);
		run = false;
        }

	/* Otherwise, thread loops with a delay of 1 second */
        sleep (1);
    }

	/* Upon quitting the loop, total num of active threads is decreased to allow for more threads to spawn if need be */
	noActiveThread--;
	threadNo--;
	pthread_exit(NULL);
}
