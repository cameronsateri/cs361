/*-------------------------------------------------------------------------------
Demo: IPC using Shared Memory
Written By: 
     1- Dr. Mohamed Aboutabl
     
     The P1 Process:    p1.c
     
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
#include <fcntl.h>

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
   shmflg      = IPC_CREAT | IPC_EXCL  | S_IRUSR | S_IWUSR ; // So, P1 must run before P2
   semflg      = O_CREAT | O_EXCL;
   semmode     = S_IRUSR | S_IWUSR;
   shmid       = shmget( shmkey , SHMEM_SIZE , shmflg ) ;

   mutex       = Sem_open("/lleshiax_satericj_mutex"        , semflg, semmode, 1);
   p1RdyToCnt  = Sem_open("/lleshiax_satericj_p1RdyToCnt"   , semflg, semmode, 0);
   p1Done      = Sem_open("/lleshiax_satericj_p1Done"       , semflg, semmode, 0);
   p2RdyToCnt  = Sem_open("/lleshiax_satericj_p2RdyToCnt"   , semflg, semmode, 0);
   p2Done      = Sem_open("/lleshiax_satericj_p2Done"       , semflg, semmode, 0);
   
   if ( shmid == -1 ) 
   {
      printf ( "\nP1 Failed to get shared memory id=%d\n" , shmid );
      perror( "Reason: " );
      exit( -1 ) ;
   }

   p  = (shmData *) shmat( shmid , NULL , 0 ); // Attach for R/W
   if ( p == (shmData *) -1 ) 
   {
      printf ("\nP1 Failed to attach shared memory id=%d\n" , shmid );
      perror( "Reason: " );
      exit( -1 ) ;
   }

   /* Initialize data in the shared memory  */ 
   p->counter = 0; 
   Sem_post(p1RdyToCnt);

   printf("P1 ready to count. LARGEINT = %10d\n", LARGEINT);
   printf("Waiting for P2 to become ready, too.\n");

   /* Wait for Other Process to start   */
   Sem_wait(p2RdyToCnt);

   printf("P1 now will increment the counter\n");
   for( unsigned i=1 ; i <= LARGEINT ; i++ )
   {
        Sem_wait(mutex);
        p->counter ++;
        Sem_post(mutex);
   }

   Sem_post(p1Done);
   printf("P1 is done. Waiting for P2 to finish, too.\n");
   Sem_wait(p2Done);

   unsigned long expected = LARGEINT << 1 ;  // 2*LARGEINT
   printf( "P1 reports final counter value = %10u  Expecting: %10lu" 
          , p->counter , expected );

   if ( p->counter == expected )    
        printf("    CORRECT\n") ;
   else  
        printf("    NOT CORRECT\n") ;
       
   shmdt( p ) ;
   shmctl( shmid , IPC_RMID , NULL );
   Sem_close(mutex);          sem_unlink("/lleshiax_satericj_mutex");
   Sem_close(p1RdyToCnt);     sem_unlink("/lleshiax_satericj_p1RdyToCnt");
   Sem_close(p1Done);         sem_unlink("/lleshiax_satericj_p1Done");
   Sem_close(p2RdyToCnt);     sem_unlink("/lleshiax_satericj_p2RdyToCnt");
   Sem_close(p2Done);         sem_unlink("/lleshiax_satericj_p2Done");

   return 0;
}

