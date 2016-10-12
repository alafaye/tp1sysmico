/*------------------------------------------------------------------*-

   ta_32768.c (v1.00) 

  ------------------------------------------------------------------

   *** THIS IS A SCHEDULER FOR THE MSP430 ***

   *** Uses TA for timing with a 32768Hz crystal ***
   *** Tick will not occure each 1ms exactly, 33/32768 = 1.007ms ***

   COPYRIGHT
   ---------

   This code is from the book:

   PATTERNS FOR TIME-TRIGGERED EMBEDDED SYSTEMS by Michael J. Pont 
   [Pearson Education, 2001; ISBN: 0-201-33138-1].

   This code is copyright (c) 2001 by Michael J. Pont.
 
   See book for copyright details and other information.
   
   Modif  : Folladore Laurent
   Date   : 03.01.06
   Reason : SCH_Update has been modified so that it allows a task
            execution at each tick (added '-1' in delay reload).
            
            Modification of the timer reload value
            
            Debug signal have been added in timer isr (defined in port.h)


   Modif  : Brunner Nicolas
   Date   : 04.09.2008
   Reason : adapted for msp430, added error code when there is too many schedule task
            (SCH_tasks_G[Index].RunMe has overflow)
   Modif  : BHT
   Date   : 04.09.2014
   Reason : adapted for msp430f5529

-*------------------------------------------------------------------*/

//#include <msp430.h>
#include "ta_32768.h"
//#include <io430.h>
#include <intrinsics.h>

// ------ Public variable declarations -----------------------------

// The array of tasks (see Scheduler.c)
extern sTask SCH_tasks_G[SCH_MAX_TASKS];

// Used to display the error code
// See Port.H for details of the error port
extern uint8_t Error_code_G;

//extern unsigned int ms=0;
//extern unsigned int cs;

/*------------------------------------------------------------------*-

  SCH_Init_T0()

  Scheduler initialisation function.  Prepares scheduler
  data structures and sets up timer interrupts at required rate.
  Must call this function before using the scheduler.  

-*------------------------------------------------------------------*/
void SCH_Init_T0(void) 
   {
   uint8_t i;

   for (i = 0; i < SCH_MAX_TASKS; i++)
      {
      SCH_Delete_Task(i-1);
      }

   // Reset the global error variable
   // - SCH_Delete_Task() will generate an error code, 
   //   (because the task array is empty)
   Error_code_G = 0;

   // Sets up timer reload values for a tick period of about 1 ms
   TA0CCR0 = 32; // Clk = 32768Hz
   
   P7SEL |= BIT7;  // To use other P7.7 funcs (MCLK)
   P7DIR |= BIT7;  // Set P7.7 to out

   //TA0CTL = TACLR | MC_1 | ID_0 | TASSEL_1;   // ACLK, divide by 1, clear, continuous mode

   }

/*------------------------------------------------------------------*-

  SCH_Start()

  Starts the scheduler, by enabling interrupts.

  NOTE: Usually called after all regular tasks are added,
  to keep the tasks synchronised.

  NOTE: ONLY THE SCHEDULER INTERRUPT SHOULD BE ENABLED!!! 
 
-*------------------------------------------------------------------*/
void SCH_Start(void) 
   {
   TA0CCTL0 |= CCIE;
   __enable_interrupt();
   }



/*------------------------------------------------------------------*-

  SCH_Update

  This is the scheduler ISR.  It is called at a rate 
  determined by the timer settings in SCH_Init_T0().
  This version is triggered by Timer A interrupts.
 
-*------------------------------------------------------------------*/
#pragma vector=TIMER0_A0_VECTOR
__interrupt void SCH_Update(void)
   {
   uint8_t Index;

   ms+=10;
   cs+=1;

      // NOTE: calculations are in *TICKS* (not milliseconds)
      for (Index = 0; Index < SCH_MAX_TASKS; Index++)
         {
         // Check if there is a task at this location
         if (SCH_tasks_G[Index].pTask)
            {
            if (SCH_tasks_G[Index].Delay == 0)
               {
               // The task is due to run
               SCH_tasks_G[Index].RunMe += 1;  // Inc. the 'Run Me' flag
               if (SCH_tasks_G[Index].RunMe == 0)
                  {
                  Error_code_G = ERROR_SCH_TOO_MANY_SCHEDULE_EXECUTION;
                  }
       
               if (SCH_tasks_G[Index].Period)
                  {
                  // Schedule periodic tasks to run again
                  SCH_tasks_G[Index].Delay = SCH_tasks_G[Index].Period - 1;  // LFE "added -1" if not "-1" the highest frequency is 2 TICKs
                                                                             // (1 tick to see it's not due and next to execute)
                  }
               }
            else
               {
               // Not yet ready to run: just decrement the delay 
               SCH_tasks_G[Index].Delay -= 1;
               }
            }
         }
      __low_power_mode_off_on_exit(); // needed to prevent the CPU from going back to low-power mode
   }

/*------------------------------------------------------------------*-
  ---- END OF FILE -------------------------------------------------
-*------------------------------------------------------------------*/
