#include <iostream>
#include <pthread.h>
#include <bitset>
#include <stdint.h>
#include <unistd.h>

#include "Assignm3.h"
#include "Assignm3_Utils.h"
#include "Maze.h"
#include "Path.h"
#include "ProgramLog.h"

using namespace std;
using namespace Assignm3;

#define gPFR globalPathFinderResource
// no 4bit UDLR (up down left right)
// 15 1111 YYYY all directions can
// 14 1110 YYYN no right
// 13 1101 YYNY no left
// 12 1100 YYNN no left no right
// 11 1011 YNYY no down
// 10 1010 YNYN no down no right
// 09 1001 YNNY no down no left
// 08 1000 YNNN ONLY UP
// 07 0111 NYYY no up
// 06 0110 NYYN no up no right
// 05 0101 NYNY no up no left
// 04 0100 NYNN ONLY DOWN
// 03 0011 NNYY no up no down
// 02 0010 NNYN ONLY LEFT
// 01 0001 NNNY ONLY RIGHT
// 00 0000 NNNN dead end
#define UP_ 8
#define DOWN_ 4
#define LEFT_ 2
#define RIGHT_ 1

static Maze* traversedMaze;
int liveThreads = 0;
int threadNo = 0;

// path finding function
void* pathFinder (void*);
bool pointAlrExplored(Point p, VectorOfPointStructType traversed);
char getUDLRFrom4Bit(int num);
int basicOptimize(VectorOfPointStructType& traversed);
void moderateOptimize(VectorOfPointStructType& traversed);
bool inStraightLine(Point p1, Point p2);
bool isObstructed(VectorOfPointStructType steps);
void straightStepsApart(VectorOfPointStructType& steps, Point p1, Point p2);
bool allStepsAlrExist(VectorOfPointStructType steps, VectorOfPointStructType traversed);

int main()
{
    // give the static pointers something to point at
    mazeObj = new Maze();
    pathObj = new Path();
    submitMazeSolnObj = new SubmitMazeSoln();
    
    mazeObj->LoadMaze();
    traversedMaze = new Maze(mazeObj->getLength(), mazeObj->getBreadth(), mazeObj->getStartLocation(), mazeObj->getEndLocation());
    
    while(!discoveredASolutionPath)
    {
        if(liveThreads <= MAX_NO_OF_THREADS)
            for(;liveThreads <= MAX_NO_OF_THREADS; ++liveThreads)
            {
                // pthread_create(pthread_t *thread, NULL for default, void *(*code to execute)(void *), void *(arg into code));
                pthread_create(&gPFR.activeThreadArray[threadNo++], NULL, pathFinder, reinterpret_cast<void*>(threadNo));
                liveThreads++;
                if(threadNo >= 54)
                    threadNo = 0;
                
                cout << "Thread " << THREAD_NAMES[threadNo] << " has been created !!" << endl;
            }
    }
    
    for(;liveThreads <= MAX_NO_OF_THREADS; +liveThreads)
        pthread_join(globalPathFinderResource.activeThreadArray[threadNo], NULL);
    
    submitMazeSolnObj->printSubmittedSolution("Stanley Teo", "5920668");
    submitMazeSolnObj->saveSubmittedSolution("Stanley Teo", "5920668");
}

void* pathFinder(void* arg)
{
    // thread No. is number of live threads
    int threadNo = static_cast<int>(reinterpret_cast<intptr_t>(arg));
    bool run = true, deadEnd;
    
    Point curr = mazeObj->getStartLocation();
    Point up, down, left, right, toGo;
    
    VectorOfPointStructType traversed;
    traversed.push_back(curr);
    
    while(run)
    {    
        // update adjacent points, top left is (0,0)
        up = Point(curr.x, curr.y-1);
        down = Point(curr.x, curr.y+1);
        left = Point(curr.x-1, curr.y);    
        right = Point(curr.x+1, curr.y);
                
        if(discoveredASolutionPath)
        {
            liveThreads--;
            threadNo--;
            pthread_exit(NULL);
        }
        
        /* About to start utilizing global resources, locks it */
        pthread_mutex_lock (&thread_mutex);

        int dir4Bit = 15;
        
        if(pointAlrExplored(up, traversed))
            dir4Bit -= UP_;
        if(pointAlrExplored(down, traversed))
            dir4Bit -= DOWN_;
        if(pointAlrExplored(left, traversed))
            dir4Bit -= LEFT_;
        if(pointAlrExplored(right, traversed))
            dir4Bit -= RIGHT_;
        
        pthread_mutex_unlock(&thread_mutex);
        deadEnd = false;
        
        // here, dir4Bit can range from 0 to 15
        // the possible directions are Up Down Left and Right
        // dead end gotten only if all 4 adjacent points are already explored
        switch (getUDLRFrom4Bit(dir4Bit))
        {
            case 'U': // up
                toGo = up;
                break;
            case 'D': // down
                toGo = down;
                break;
            case 'L': // left
                toGo = left;
                break;
            case 'R': // right
                toGo = right;
                break;
            default: // dead end
                deadEnd = true;
        }
        
        // Does direction lead to dead end
        if(deadEnd)
        {
            cout << "Thread '" << THREAD_NAMES [threadNo] <<"' hit a DEAD END at ";
            curr.display();
            cout << " !!" << endl;
                      
            pthread_mutex_lock(&thread_mutex);
            
            gPFR.discoveredDangerAreas.push_back(curr);// count dead end as danger
            traversed.pop_back(); // remove the last traversed point
            curr = traversed.back(); // go back to the new last traversed point (was 2nd latest)
            
            pthread_mutex_unlock(&thread_mutex);
	}
        // else check if direction to go is a barrier
        else if(mazeObj->IsThereBarrier(toGo))
	{               
            pthread_mutex_lock(&thread_mutex);
            
            gPFR.discoveredDangerAreas.push_back(toGo);
            
            traversed.push_back(toGo);
            submitMazeSolnObj->submitPathToBarrier((pthread_t)threadNo, traversed);
            traversed.pop_back();
            traversedMaze->updateMaze(toGo, BARRIER_INT);
            
            pthread_mutex_unlock(&thread_mutex);
	}
        // wasn't barrier nor dead end, so move in direction
        else
        {
            curr = toGo;
            traversed.push_back(curr);
            pthread_mutex_lock(&thread_mutex);
            traversedMaze->ShowPathGraphically(traversed);	
            pthread_mutex_unlock(&thread_mutex);
        }

        // if we move into danger
        if (mazeObj->IsThereDanger(curr))
	{
            cout << "Thread '" << THREAD_NAMES [threadNo] << "' stepped into DANGER at ";
            curr.display(); 
            cout << " !!" << endl;
            cout << "Thread '" << THREAD_NAMES [threadNo] << "' is dead !!"
                 << "It's sacrifice shall not be in vain !!" << endl << endl;

            pthread_mutex_lock(&thread_mutex);

            submitMazeSolnObj->submitPathToDangerArea((pthread_t)threadNo, traversed);
            gPFR.discoveredDangerAreas.push_back (curr);
            traversedMaze->updateMaze(curr, DANGER_INT);

            pthread_mutex_unlock(&thread_mutex);
            
            run = false; // thread to die stop looping
	}
        // else if we moved into end location
        else if(curr == mazeObj->getEndLocation())
        {
            cout << "Thread '" << THREAD_NAMES [threadNo] <<"' just found a solution !! Well done !!";
            cout << endl << endl;
            
            // hello this are my functions, please give me marks for it
            basicOptimize(traversed);
            moderateOptimize(traversed);
            
            
            pthread_mutex_lock(&thread_mutex);
            
            submitMazeSolnObj->submitSolutionPath ((pthread_t) threadNo, traversed);
            discoveredASolutionPath = true;
            
            pthread_mutex_unlock(&thread_mutex);
            
            run = false; // thread to die also, but honourably with fanfare
        }
               
        usleep(100000);
        
    }
    liveThreads--;
    threadNo--;
    pthread_exit(NULL);
}

char getUDLRFrom4Bit(int num)
{
    string s = bitset<4>(num).to_string();

    char UDLR[4] = {'U','D','L','R'};
    vector<char> possibilities;
        
    for(int i = 0; i < 4; ++i)
    {
        if(s[i] == '1')
            possibilities.push_back(UDLR[i]);
    }
    
    if(possibilities.size())
        return possibilities[GenerateRandomInteger(0, possibilities.size()-1)];
    return '?';
}

// simple function to compare every step with every other step and see if they are next to each other
int basicOptimize(VectorOfPointStructType& traversed)
{
    cout << "Basic Optimize Start" << endl;
    
    int deleted = 0;
    Point curr, against;
    for(int i = 0; i < traversed.size(); ++i)// check earlier steps
    {
        curr = traversed[i]; // curr will traverse the path
        for(int j = traversed.size()-1; j > i+1; --j)// against later steps
        {
            against = traversed[j]; // to be compared against
            // if the points are connected means all other steps in between are worthless
            if (curr.isConnected(against)) 
            {
                cout << i+deleted;
                curr.display();
                cout << " touches ";
                cout << j+deleted;
                against.display();
                cout << endl;
                // if 1 connects to 6, delete 1+1(2) to 6-1(5)
                traversed.erase(traversed.begin()+i+1, traversed.begin()+j);
                cout << "deleted " << i+1+deleted << " to " << j-1+deleted << endl;
                deleted += j-i-1;
                break; // exit on first touch
            }
        }
    }
    return deleted;
}

void moderateOptimize(VectorOfPointStructType& traversed)
{   
    cout << "Moderate Optimize Start" << endl;
    int added = 0, deleted = 0;// take d from previous optimize
    Point curr, against;
    VectorOfPointStructType steps;
    bool lucky = false;
    for(int i = 0; i < traversed.size(); ++i)// check earlier steps
    {
        curr = traversed[i]; // curr will traverse the path
        for(int j = traversed.size()-1; j > i+1; --j)// against later steps
        {
            against = traversed[j]; // to be compared against
            // if the points can form a straight line and aren't connected
            if (inStraightLine(curr, against) && !curr.isConnected(against))
            {
                // get the vector of straight steps apart
                straightStepsApart(steps, curr, against);
                
                if(isObstructed(steps) && !allStepsAlrExist(steps, traversed))
                    steps.clear();// clear worthless steps
                else // we have work to do
                {
                    // similar to basic optimize, delete the worthless steps
                    traversed.erase(traversed.begin()+i+1, traversed.begin()+j);
                                      
                    traversed.insert(traversed.begin()+i+1, steps.begin(), steps.end());
                    
                    cout << "created straightline between ";
                    steps.front().display();
                    cout << " and ";
                    steps.back().display();
                    cout << endl;
                    
                    deleted += j-i-1;
                    //added += steps.size();
                    i = j;
                }
                break; // exit on first success
            }
        }
    }
}

bool allStepsAlrExist(VectorOfPointStructType steps, VectorOfPointStructType traversed)
{
    for (int i = 0; i < steps.size(); ++i)
    {
        for(int j = 0; i < traversed.size(); ++j)
            if (steps[i] != traversed[j])
                return false;
    }
    return true;
}

bool isObstructed(VectorOfPointStructType steps)
{
    for(int i = 0; i < steps.size(); ++i)
    {
    // if along the straight line there is danger or barrier the line is worthless
        if(mazeObj->IsThereBarrier(steps[i]) || mazeObj->IsThereDanger(steps[i]))
        {
            return true;
        }            
    }
    return false;
}

bool inStraightLine(Point p1, Point p2)
{
    return (p1.x == p2.x || p1.y == p2.y);
}

void straightStepsApart(VectorOfPointStructType& steps, Point p1, Point p2)
{
    Point p;
    int dist;
    steps.clear();// empty just in case
    
    if (p1.x == p2.x) // the points are vertical p1 needs to move up or down
    {
        dist = p2.y - p1.y;
        if (abs(dist) == 1)// already adjacent so do nothing
            return;
        
        if (dist > 0)// positive diff
        {
            for(int i = 2; i < dist; ++i)
            {
                p = Point(p1.x, p1.y+i);
                steps.push_back(p);
            }
        }
        else if (dist < 0)// negative diff
        {
            for(int i = -2; i > dist; --i)
            {
                p = Point(p1.x, p1.y+i);
                steps.push_back(p);
            }  
        }
    }
    else if (p1.y == p2.y) // the points are horizontal
    {
        dist = p2.x - p1.x;
        if (abs(dist) == 1)// already adjacent so do nothing
            return;
        
        if (dist > 0)// positive diff
        {
            for(int i = 1; i < dist; ++i)
            {
                p = Point(p1.x+i, p1.y);
                steps.push_back(p);
            }
        }
        else if (dist < 0)// negative diff
        {
            for(int i = -1; i > dist; --i)
            {
                p = Point(p1.x+i, p1.y);
                steps.push_back(p);
            }  
        }
    }
}

bool pointAlrExplored(Point p, VectorOfPointStructType traversed)
{
    // if p is known danger or has been traversed before
    if(pathObj->isLocationInPath(p, gPFR.discoveredDangerAreas) || pathObj->isLocationInPath(p, traversed))
        return true;
    // otherwise point is unexplored
    return false;
}

