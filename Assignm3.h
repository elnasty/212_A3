
#ifndef Assignm3_H
#define Assignm3_H

// ------------------------------------------------------------------------------------

#include <vector>
#include <string>
#include <pthread.h>

#include "Path.h"
#include "Maze.h"
#include "ProgramLog.h"
#include "Assignm3_Utils.h"

#include "SubmitMazeSoln.h"

// ------------------------------------------------------------------------------------

namespace Assignm3
{

	const int MAX_NO_OF_THREADS			= 3;
	const std::string THREAD_NAMES []	= {"POOH", "TIGGER", "ROO", "GOLPHER", "KANGA", "LUMPY", "OWL", "RABBIT", "PIGLET",
										   "POOH0", "TIGGER0", "ROO0", "GOLPHER0", "KANGA0", "LUMPY0", "OWL0", "RABBIT0", "PIGLET0",
										   "POOH1", "TIGGER1", "ROO1", "GOLPHER1", "KANGA1", "LUMPY1", "OWL1", "RABBIT1", "PIGLET1",
										   "POOH2", "TIGGER2", "ROO2", "GOLPHER2", "KANGA2", "LUMPY2", "OWL2", "RABBIT2", "PIGLET2",
										   "POOH3", "TIGGER3", "ROO3", "GOLPHER3", "KANGA3", "LUMPY3", "OWL3", "RABBIT3", "PIGLET3",
										   "POOH4", "TIGGER4", "ROO4", "GOLPHER4", "KANGA4", "LUMPY4", "OWL4", "RABBIT4", "PIGLET4",
										   "POOH5", "TIGGER5", "ROO5", "GOLPHER5", "KANGA5", "LUMPY5", "OWL5", "RABBIT5", "PIGLET5"
										  };

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

	struct PathFinderParameterInfo
	{
		int						threadIDArrayIndex;
		bool					exitThisThreadNow;
		Point					currentLocation;
		std::string				threadName;
		VectorOfPointStructType	travelledPath;

		PathFinderParameterInfo (void)
		{
			currentLocation.x	= -1;
			currentLocation.y	= -1;
			threadIDArrayIndex	= -1;
			exitThisThreadNow	= false;
			travelledPath		= VectorOfPointStructType ();
		}

		~PathFinderParameterInfo (void)
		{
			travelledPath.clear ();
		}
	};

// ------------------------------------------------------------------------------------

    struct PathFinderResource
    {
		pthread_t						activeThreadArray 		[MAX_NO_OF_THREADS];
		PathFinderParameterInfo *		activeThreadParamArray	[MAX_NO_OF_THREADS];

		VectorOfPointStructType			solutionPath;
		VectorOfPointStructType			discoveredDangerAreas;
		int								usedThreadNameIndex;
		int								noOfDeadEndPathsFound;
		int								noOfBarriersDiscovered;
		int								noOfDangerAreaDiscovered;
		
		PathFinderResource (void)
		{
			usedThreadNameIndex			= 0;
			noOfDeadEndPathsFound		= 0;
			noOfBarriersDiscovered		= 0;
			noOfDangerAreaDiscovered	= 0;
			solutionPath				= VectorOfPointStructType ();
			discoveredDangerAreas		= VectorOfPointStructType ();
		}

		~PathFinderResource (void)
		{
			solutionPath.clear ();
			discoveredDangerAreas.clear ();
		}
	};
	
	PathFinderResource globalPathFinderResource;
	
// ------------------------------------------------------------------------------------

	static Maze * mazeObj;
	static Path * pathObj;
	static SubmitMazeSoln * submitMazeSolnObj;

	static std::fstream logFileStream;
	static std::string DefaultLogFilename = "Assignm3Log.txt";

	static pthread_mutex_t	thread_mutex		= PTHREAD_MUTEX_INITIALIZER;
	static pthread_cond_t   thread_condition	= PTHREAD_COND_INITIALIZER;

	static bool mainThreadReportUpdateNow		= false;
	static bool discoveredASolutionPath			= false;

// ------------------------------------------------------------------------------------

	static void AllocateProgramsVariableMemory (void)
	{
		mazeObj				= new Maze ();
		pathObj				= new Path ();
		submitMazeSolnObj	= new SubmitMazeSoln ();
		
		logFileStream.open (DefaultLogFilename.c_str(), std::fstream::out);

	}	// end allocateProgramsVariableMemory () ...

// ------------------------------------------------------------------------------------

	static void DeallocateProgramsVariableMemory (void)
	{
		delete mazeObj;
		delete pathObj;
		delete submitMazeSolnObj;
		
		logFileStream.close ();
		pthread_mutex_destroy ( &thread_mutex );
		pthread_cond_destroy  ( &thread_condition );

	}	// end deallocateProgramsVariableMemory () ...

// ------------------------------------------------------------------------------------

	static void HandleThreadOperationResult (std::ostream & outputStream, const std::string message, const int status)
	{
		if (status)
		{
			std::string msg = "Error on : " + message + ", ERROR CODE = " + IntToString (status) + "\n";

			// below function 'WriteLogMessage' is defined in 'ProgramLog.h' ...
			WriteLogMessage (std::cout, msg);
			WriteLogMessage (outputStream, msg);
	
			DeallocateProgramsVariableMemory ();
			exit (EXIT_FAILURE);
		}
	
	}	// end handleThreadOperationResult () ...

// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------

}	// end namespace Assignm3

#endif // Assignm3_H


