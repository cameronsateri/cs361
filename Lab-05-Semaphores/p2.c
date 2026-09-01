/*-------------------------------------------------------------------------------
Demo: IPC using Shared Memory
Assignment  : Lab-05 Semaphores
Date        : /26/2024

Written By  : 1- Dr. Mohamed Aboutabl

Edited By   : Team 2
              1- Anna-Maria Lleshi lleshiax@dukes.jmu.edu
              2- Cameron Sateri satericj@dukes.jmu.edu

The P2 Process:    p2.c
     
-------------------------------------------------------------------------------*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include "wrappers.h"
#include "shmSegment.h"
#include <semaphore.h>

int main( int argc , char *argv[] )
{
   int        shmid ;
   key_t      shmkey;
   int        shmflg;
   int        semflg;
   int        semmode;
   shmData    *p;
   sem_t      *mutex;
   sem_t      *p1RdyToCnt, *p1Done, *p2RdyToCnt, *p2Done;
   
   shmkey      = ftok( "shmSegment.h" , 5 ) ;
   shmflg      = S_IRUSR | S_IWUSR ;  // No IPC_CREAT, so P2 must run after P1
   semflg      = 0 ; //No O_CREAT, so use the sem_open with only two args.
   semmode     = S_IRUSR | S_IWUSR;
   
   shmid       = shmget( shmkey , SHMEM_SIZE , shmflg ) ;

   mutex       = Sem_open2("/lleshiax_satericj_mutex"        , semflg);
   p1RdyToCnt  = Sem_open2("/lleshiax_satericj_p1RdyToCnt"   , semflg);
   p1Done      = Sem_open2("/lleshiax_satericj_p1Done"       , semflg);
   p2RdyToCnt  = Sem_open2("/lleshiax_satericj_p2RdyToCnt"   , semflg);
   p2Done      = Sem_open2("/lleshiax_satericj_p2Done"       , semflg);
   
   if ( shmid == -1 ) 
   {
      printf ( "\nP2 Failed to get shared memory id=%d\n" , shmid );
      perror( "Reason: " );      exit( -1 ) ;
   }

   p  = (shmData *) shmat( shmid , NULL , 0 ); // Attach for R/W
   if ( p == (shmData *) -1 ) 
   {
      printf ("\nP2 Failed to attach shared memory id=%d\n" , shmid );
      perror( "Reason: " );      exit( -1 ) ;
   }

   /* Initialize data in the shared memory  */ 

   Sem_post(p2RdyToCnt);

   printf("P2 ready to count. LARGEINT = %10d\n" , LARGEINT );
   printf("Waiting for P1  to start, too.\n");

   /* Wait for Other Process to start   */
   Sem_wait(p1RdyToCnt);

   printf("P2 now will increment the counter\n");
   for( unsigned i=1 ; i <= LARGEINT ; i++ ){
        Sem_wait(mutex);
        p->counter ++;
        Sem_post(mutex);
   }
 
   Sem_post(p2Done);
   
   printf("P2 is done. Waiting for P1 to finish, too.\n");

   sem_wait(p1Done);

   unsigned long expected = LARGEINT << 1 ;  // 2*LARGEINT
   printf("P2 reports final counter value = %10u  Expecting: %10lu" 
          , p->counter , expected );

   if ( p->counter == expected )    
        printf("    CORRECT\n") ;
   else
        printf("    NOT CORRECT\n") ;
       
   shmdt( p ) ;

   Sem_close( mutex      );
   Sem_close( p1RdyToCnt );
   Sem_close( p1Done     );
   Sem_close( p2RdyToCnt );
   Sem_close( p2Done     );

   return 0;
}
