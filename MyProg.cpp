#include <iostream>
#include <pthread.h>
#include <bitset>
#include <stdint.h>

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

Maze* traversedMaze;
int liveThreads = 0;
int threadNo = 0;

// path finding function
void* pathFinder (void*);
bool pointAlrExplored(Point p, VectorOfPointStructType traversed);
char getUDLRFrom4Bit(int num);
bool encounter(Point p, VectorOfPointStructType traversed, bool&);
void deadEnd(Point p, VectorOfPointStructType traversed);


int main()
{
    mazeObj->LoadMaze(DefaultMazeFilename);
    traversedMaze = new Maze(mazeObj->getLength(), mazeObj->getBreadth(), mazeObj->getStartLocation(), mazeObj->getEndLocation);
    
    while(!discoveredASolutionPath)
    {
        if(liveThreads < MAX_NO_OF_THREADS)
            for(;liveThreads < MAX_NO_OF_THREADS; ++liveThreads)
            {
                // pthread_create(pthread_t *thread, NULL for default, void *(*code to execute)(void *), void *(arg into code));
                pthread_create(gPFR.activeThreadArray[threadNo++], NULL, pathFinder, reinterpret_cast<void *>(threadNo));
                liveThreads++;
                
                cout << "Thread " << THREAD_NAMES[threadNo] << " has been created !!" << endl;
            }
    }
    
    for(;liveThreads < MAX_NO_OF_THREADS; +liveThreads)
        pthread_join(globalPathFinderResource.activeThreadArray[threadNo], NULL);
    
    submitMazeSolnObj->printSubmittedSolution("Stanley Teo", "5920668");
    submitMazeSolnObj->saveSubmittedSolution("Stanley Teo", "5920668");
}

void* pathFinder(void* arg)
{
    // thread No. is number of live threads
    int threadNo = static_cast<int>(reinterpret_cast<intptr_t>(arg));
    bool run = true;
    
    Point curr = mazeObj->getStartLocation();
    Point up, down, left, right;
    
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
            gPFR.usedThreadNameIndex--;
            pthread_exit();
        }
               
        int dir4Bit = 15;
        
        if(pointAlrExplored(up, traversed))
            dir4Bit -= UP_;
        if(pointAlrExplored(down, traversed))
            dir4Bit -= DOWN_;
        if(pointAlrExplored(left, traversed))
            dir4Bit -= LEFT_;
        if(pointAlrExplored(right, traversed))
            dir4Bit -= RIGHT_;
        
        // data soon to be written to global stuff so lock for for this thread for a bit
        pthread_mutex_lock (&thread_mutex);
        
        // here, dir4Bit can range from 0 to 15
        // the possible directions are Up Down Left and Right
        // dead end gotten only if all 4 adjacent points are already explored
        switch (getUDLRFrom4Bit(dir4Bit))
        {
            case 'U': // up
                if (encounter(up, traversed, run))
                    curr = up;
                break;
            case 'D': // down
                if (encounter(up, traversed, run))
                    curr = up;
                break;
            case 'L': // left
                if (encounter(up, traversed, run))
                    curr = up;
                break;
            case 'R': // right
                if (encounter(up, traversed, run))
                    curr = up;
                break;
            default: // dead end
        }
        
        sleep(1);
    }
    noActiveThread--;
    threadNo--;
    pthread_exit(NULL);
}

bool encounter(Point p, VectorOfPointStructType traversed, bool& run)
{
    bool canAdvance = false;
    if(mazeObj->IsThereBarrier(p)) // the encounter was a barrier
    {
        gPFR->discoveredDangerLocations.push_back(p);
        traversed.push_back(p); // add the barrier to traversed points;
        submitMazeSolnObj->submitPathToBarrier((pthread_t)threadNo, traversed);
        traversed.pop_back(); // since we don't actually want to be traversing barriers
        traversedMaze->updateMaze(p, BARRIER_INT);
        pthread_mutex_unlock(&thread_mutex);
    }
    else if(mazeObj->IsThereDanger(p)) // the encounter was a danger
    {
        cout << "Thread '" << THREAD_NAMES [threadNo] << "' stepped into DANGER at ";
	p.display(); 
        cout << " !!" << endl;
        
        gPFR->discoveredDangerLocations.push_back(p);
        traversed.push_back(p); // add the danger to traversed points;
        submitMazeSolnObj->submitPathToDangerArea((pthread_t)threadNo, traversed);
        traversed.pop_back(); // since we don't actually want to be traversing barriers
        traversedMaze->updateMaze(p, BARRIER_INT);
        pthread_mutex_unlock(&thread_mutex);
        run = false;
    }
    else if(p == mazeObj->getEndLocation()) // the encounter was the solution
    {
        cout << "Thread '" << THREAD_NAMES [threadNo] << "' just found a solution !! ";
        submitMazeSolnObj->submitSolutionPath ((pthread_t) threadNo, traversed);
        discoveredASolutionPath += 1;
        pthread_mutex_unlock(&thread_mutex);
        run = false;
    }
    else // the encounter is just another path and we can advance
        canAdvance = true;
        
    return canAdvance;
}

bool deadEnd(Point p, VectorOfPointStructType traversed)
{
    cout << "Thread " << THREAD_NAMES [threadNo] <<" hits a DEAD END at ";
    p.display();
    cout << " !!" << endl;
    
    gPFR->discoveredDangerLocations.push_back(p);
    traversed.pop_back();
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

bool pointAlrExplored(Point p, VectorOfPointStructType traversed)
{
    // if p is known danger or has been traversed before
    if(Path::isLocationInPath(p, gPFR.discoveredDangerAreas) || Path::isLocationInPath(p, traversed))
        return true;
    // otherwise point is unexplored
    return false;
}

