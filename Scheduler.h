/*------------------------------------------------------------------*-
  
   Scheduler.h (v1.00)

  ------------------------------------------------------------------

   - see Scheduler.c for details


   COPYRIGHT
   ---------

   This code is from the book:

   PATTERNS FOR TIME-TRIGGERED EMBEDDED SYSTEMS by Michael J. Pont 
   [Pearson Education, 2001; ISBN: 0-201-33138-1].

   This code is copyright (c) 2001 by Michael J. Pont.
 
   See book for copyright details and other information.
   
   Modif  : Folladore Laurent
   Date   : 03.01.06
   Reason : VFPTR (Virtual Function PointeR) type definition added
            to allow usage of function pointer as function parameter
            (which seems necessary with Raisonnnance Ide)
            
            Modification of the task pool size

   Modif  : Brunner Nicolas
   Date   : 04.09.2008
   Reason : adapted for msp430

-*------------------------------------------------------------------*/

#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include <stdint.h>

typedef void (*VFPTR)(void);  // _LFE_

// ------ Public data type declarations ----------------------------

// Store in DATA area, if possible, for rapid access  
// Total memory per task is 7 bytes
typedef struct 
   {
   // Pointer to the task (must be a 'void (void)' function)
   VFPTR pTask;  // _LFE_

   // Delay (ticks) until the function will (next) be run
   // - see SCH_Add_Task() for further details
   uint16_t Delay;       

   // Interval (ticks) between subsequent runs.
   // - see SCH_Add_Task() for further details
   uint16_t Period;       

   // Incremented (by scheduler) when task is due to execute
   uint8_t RunMe;       
   } sTask; 

// ------ Public function prototypes -------------------------------

// Core scheduler functions
void  SCH_Dispatch_Tasks(void);
uint8_t SCH_Add_Task(VFPTR, const uint16_t, const uint16_t);  
char  SCH_Delete_Task(const uint8_t);
void  SCH_Report_Status(void);

// ------ Public constants -----------------------------------------

#define RETURN_NORMAL 0
#define RETURN_ERROR  1

// The maximum number of tasks required at any one time
// during the execution of the program
//
// MUST BE ADJUSTED FOR EACH NEW PROJECT
#define SCH_MAX_TASKS   (10)

#define ERROR_SCH_TOO_MANY_TASKS (1)
#define ERROR_SCH_CANNOT_DELETE_TASK (2)
#define ERROR_SCH_TOO_MANY_SCHEDULE_EXECUTION (3)

 
#endif
                              
/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/
