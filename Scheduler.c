/*------------------------------------------------------------------*-

   Scheduler.c (v1.00) 

  ------------------------------------------------------------------

   *** THESE ARE THE CORE SCHEDULER FUNCTIONS ***
   --- These functions may be used with all 8051 devices ---

   *** SCH_MAX_TASKS *must* be set by the user ***
   --- see "Scheduler.h" ---

   *** Includes (optional) power-saving mode ***
   --- You must ensure that the power-down mode is adapted ---
   --- to match your chosen device (or disabled altogether) ---


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

   Modif  : Brunner Nicolas
   Date   : 04.09.2008
   Reason : adapted for msp430, only go to sleep when no waiting task left

-*------------------------------------------------------------------*/

#include <intrinsics.h>
#include "Scheduler.h"

// ------ Public variable definitions ------------------------------

// The array of tasks
sTask SCH_tasks_G[SCH_MAX_TASKS];

// Used to display the error code
// See Port.h for details of the error port
uint8_t Error_code_G = 0;

// ------ Private function prototypes ------------------------------

static void SCH_Go_To_Sleep(void);

// ------ Private variables ----------------------------------------

// Keeps track of time since last error was recorded (see below)
static uint16_t Error_tick_count_G;

// The code of the last error (reset after ~1 minute)
static uint8_t Last_error_code_G;


/*------------------------------------------------------------------*-

  SCH_Dispatch_Tasks()

  This is the 'dispatcher' function.  When a task (function)
  is due to run, SCH_Dispatch_Tasks() will run it.
  This function must be called (repeatedly) from the main loop.

-*------------------------------------------------------------------*/
void SCH_Dispatch_Tasks(void) 
   {
   uint8_t Index;
   uint8_t goToSleep = 1;

   // Dispatches (runs) the next task (if one is ready)
   for (Index = 0; Index < SCH_MAX_TASKS; Index++)
      {
      if (SCH_tasks_G[Index].RunMe > 0)
         {
         (*SCH_tasks_G[Index].pTask)();  // Run the task

         SCH_tasks_G[Index].RunMe -= 1;   // Reset / reduce RunMe flag

         // Periodic tasks will automatically run again
         // - if this is a 'one shot' task, remove it from the array
         if (SCH_tasks_G[Index].Period == 0)
            {
            SCH_Delete_Task(Index);
            }
         }
      }

   // Report system status
   SCH_Report_Status();

   // The scheduler enters idle mode if no task is waiting execution
   Index = 0;
   while (goToSleep && Index < SCH_MAX_TASKS)
      {
      goToSleep = !SCH_tasks_G[Index].RunMe;
      Index++;
      }
   if (goToSleep) SCH_Go_To_Sleep();
   }

/*------------------------------------------------------------------*-

  SCH_Add_Task()

  Causes a task (function) to be executed at regular intervals 
  or after a user-defined delay

  Fn_P   - The name of the function which is to be scheduled.
           NOTE: All scheduled functions must be 'void, void' -
           that is, they must take no parameters, and have 
           a void return type. 
                   
  DELAY  - The interval (TICKS) before the task is first executed

  PERIOD - If 'PERIOD' is 0, the function is only called once,
           at the time determined by 'DELAY'.  If PERIOD is non-zero,
           then the function is called repeatedly at an interval
           determined by the value of PERIOD (see below for examples
           which should help clarify this).


  RETURN VALUE:  

  Returns the position in the task array at which the task has been 
  added.  If the return value is SCH_MAX_TASKS then the task could 
  not be added to the array (there was insufficient space).  If the
  return value is < SCH_MAX_TASKS, then the task was added 
  successfully.  

  Note: this return value may be required, if a task is
  to be subsequently deleted - see SCH_Delete_Task().

  EXAMPLES:

  Task_ID = SCH_Add_Task(Do_X,1000,0);
  Causes the function Do_X() to be executed once after 1000 sch ticks.            

  Task_ID = SCH_Add_Task(Do_X,0,1000);
  Causes the function Do_X() to be executed regularly, every 1000 sch ticks.            

  Task_ID = SCH_Add_Task(Do_X,300,1000);
  Causes the function Do_X() to be executed regularly, every 1000 ticks.
  Task will be first executed at T = 300 ticks, then 1300, 2300, etc.            
 
-*------------------------------------------------------------------*/
uint8_t SCH_Add_Task(VFPTR pFunction,     // _LFE_
                   const uint16_t DELAY, 
                   const uint16_t PERIOD)    
   {
   uint8_t Index = 0;
   
   // First find a gap in the array (if there is one)
   while ((SCH_tasks_G[Index].pTask != 0) && (Index < SCH_MAX_TASKS))
      {
      Index++;
      } 
   
   // Have we reached the end of the list?   
   if (Index == SCH_MAX_TASKS)
      {
      // Task list is full
      //
      // Set the global error variable
      Error_code_G = ERROR_SCH_TOO_MANY_TASKS;

      // Also return an error code
      return SCH_MAX_TASKS;  
      }
      
   // If we're here, there is a space in the task array
   SCH_tasks_G[Index].Delay  = DELAY;
   SCH_tasks_G[Index].Period = PERIOD;

   SCH_tasks_G[Index].RunMe  = 0;
   
   // update the pTask at the end because it's tell the scheduler there is a task 
   SCH_tasks_G[Index].pTask  = pFunction;

   return Index; // return position of task (to allow later deletion)
   }

/*------------------------------------------------------------------*-

  SCH_Delete_Task()

  Removes a task from the scheduler.  Note that this does
  *not* delete the associated function from memory: 
  it simply means that it is no longer called by the scheduler. 
 
  TASK_INDEX - The task index.  Provided by SCH_Add_Task(). 

  RETURN VALUE:  RETURN_ERROR or RETURN_NORMAL

-*------------------------------------------------------------------*/
char SCH_Delete_Task(const uint8_t TASK_INDEX)
   {
//   bit Return_code;
   char Return_code;


   if (SCH_tasks_G[TASK_INDEX].pTask == 0)
      {
      // No task at this location...
      //
      // Set the global error variable
      Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;

      // ...also return an error code
      Return_code = RETURN_ERROR;
      }
   else
      {
      Return_code = RETURN_NORMAL;
      }      
   
   SCH_tasks_G[TASK_INDEX].pTask   = 0x0000;
   SCH_tasks_G[TASK_INDEX].Delay   = 0;
   SCH_tasks_G[TASK_INDEX].Period  = 0;

   SCH_tasks_G[TASK_INDEX].RunMe   = 0;

   return Return_code;       // return status
   }


/*------------------------------------------------------------------*-

  SCH_Report_Status()

  Simple function to display error codes.

  This version displays code on a port with attached LEDs:
  adapt, if required, to report errors over serial link, etc.

  Errors are only displayed for a limited period 
  (60000 ticks = 1 minute at 1ms tick interval).
  After this the the error code is reset to 0. 

  This code may be easily adapted to display the last
  error 'for ever': this may be appropriate in your
  application.

  See Chapter 10 for further information.

-*------------------------------------------------------------------*/
void SCH_Report_Status(void)
   {
#ifdef SCH_REPORT_ERRORS
   // ONLY APPLIES IF WE ARE REPORTING ERRORS
   // Check for a new error code
   if (Error_code_G != Last_error_code_G)
      {
      Error_port = (Error_port & ~Error_mask) | (Error_code_G << Error_shift);
      
      Last_error_code_G = Error_code_G;

      if (Error_code_G != 0)
         {
         Error_tick_count_G = 60000;
         }
      else
         {
         Error_tick_count_G = 0;
         }
      }
   else
      {
      if (Error_tick_count_G != 0)
         {
         if (--Error_tick_count_G == 0)
            {
            Error_code_G = 0; // Reset error code
            }
         }
      }
#endif
   }


/*------------------------------------------------------------------*-

  SCH_Go_To_Sleep()

  This scheduler enters 'idle mode' between clock ticks
  to save power.  The next clock tick will return the processor
  to the normal operating state.

  Note: a slight performance improvement is possible if this
  function is implemented as a macro, or if the code here is simply 
  pasted into the 'dispatch' function.  

  However, by making this a function call, it becomes easier 
  - during development - to assess the performance of the 
  scheduler, using the 'performance analyser' in the Keil 
  hardware simulator. See Chapter 14 for examples for this. 

  *** May wish to disable this if using a watchdog ***

  *** ADAPT AS REQUIRED FOR YOUR HARDWARE ***

-*------------------------------------------------------------------*/
void SCH_Go_To_Sleep()
   {
   //__low_power_mode_3();
   }

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/
