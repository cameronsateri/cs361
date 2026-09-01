//---------------------------------------------------------------------
// Assignment : PA-04 Multi-Threaded UDP Server
// Date       : 12/03/2024
// Author     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//              Cameron Sateri satericj@dukes.jmu.edu
// File Name  : factory.c
//---------------------------------------------------------------------

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "wrappers.h"
#include "message.h"

#define MAXSTR     200
#define IPSTRLEN    50

typedef struct sockaddr SA ;

int minimum( int a , int b)
{
    return ( a <= b ? a : b ) ; 
}

void *subFactory( void * ptr ) ;

//int iterations;

void factLog( char *str )
{
    printf( "%s" , str );
    fflush( stdout ) ;
}

typedef struct {
    int factoryID;
    int capacity;
    int duration;
} params_t;

typedef struct {
    int factoryID;
    int totalPartsMade;
    int iterations;
} factoryResults_t;

int grandTotalPartsmade = 0;

/*-------------------------------------------------------*/

// Global Variable for Future Thread to Shared
pthread_mutex_t activeMutex = PTHREAD_MUTEX_INITIALIZER; //mutex to protect active threads

int   remainsToMake , // Must be protected by a Mutex
      actuallyMade ;  // Actually manufactured items

int   numActiveFactories = 1 , orderSize ;

volatile unsigned  activeThreads = 0;  // To be protected by a mutex

int   sd ;      // Server socket descriptor
struct sockaddr_in  
             srvrSkt,       /* the address of this server   */
             clntSkt;       /* remote client's socket       */

//------------------------------------------------------------
//  Handle Ctrl-C or KILL 
//------------------------------------------------------------
void goodbye(int sig) 
{
    /* Mission Accomplished */
    printf( "\n### I (%d) have been nicely asked to TERMINATE. "
           "goodbye\n\n" , getpid() );  
    msgBuf msg;
    msg.purpose = htonl(PROTOCOL_ERR);
    msg.facID = htonl(0);
    sendto(sd, (void *) &msg, sizeof(msgBuf), 0, (SA *) &clntSkt, sizeof(clntSkt));
    close(sig);
    exit(0);
}

/*-------------------------------------------------------*/
int main( int argc , char *argv[] )
{
    char  *myName = "Anna-Maria Lleshi and Cameron Sateri" ; 
    unsigned short port = 50015 ;      /* service port number  */
    int    N = 1 ;                     /* Num threads serving the client */

    printf("\nThis is the FACTORY server developed by %s\n\n" , myName ) ;
    char myUserName[30] ;
    getlogin_r ( myUserName , 30 ) ;
    time_t now_time;
    time(&now_time);
    struct timeval now, end;
    fprintf( stdout , "Logged in as user '%s' on %s\n\n" , myUserName ,  ctime( &now_time)  ) ;
    fflush( stdout ) ;

	switch (argc) 
	{
      case 1:
        break ;     // use default port with a single factory thread
      
      case 2:
        N = atoi( argv[1] ); // get from command line
        port = 50015;            // use this port by default
        break;

      case 3:
        N    = atoi( argv[1] ) ; // get from command line
        port = atoi( argv[2] ) ; // use port from command line
        break;

      default:
        printf( "FACTORY Usage: %s [numThreads] [port]\n" , argv[0] );
        exit( 1 ) ;
    }
    printf("I will attempt to accept orders at port %d and use %d sub-factories.\n\n", port, N);

    // handelers for signals
    sigactionWrapper(SIGINT, goodbye);
    sigactionWrapper(SIGTERM, goodbye);

    // create socket
    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0)
    {
        exit(1);
    }
    printf("Bound socket %d to IP 0.0.0.0 Port %d\n", sd, port);

    // set up socket
    srvrSkt.sin_family = AF_INET;
    srvrSkt.sin_addr.s_addr = htonl(INADDR_ANY);
    srvrSkt.sin_port = htons(port);
    
    if (bind( sd, (SA *) & srvrSkt , sizeof(srvrSkt) ) < 0 )
    {
        close(sd);
        exit(1);
    }

    factoryResults_t *results[N];

    int forever = 1;
    while ( forever )
    {
        

        printf( "\nFACTORY server ( by ANNA-MARIA LLESHI and CAMERON SATERI ) waiting for Order Requests\n\n" ) ; 

        msgBuf msg1;
        socklen_t clntLen = sizeof(clntSkt);

        recvfrom(sd, (void*) &msg1, sizeof(msgBuf), 0, (SA *) &clntSkt, &clntLen);

        char ipStr[IPSTRLEN];
        inet_ntop(AF_INET, (void*) &clntSkt.sin_addr.s_addr, ipStr, IPSTRLEN);

        orderSize = ntohl(msg1.orderSize);
        remainsToMake = orderSize;
        actuallyMade = 0;

        printf("\nFACTORY server received: " ) ;
        printMsg( & msg1 );  puts("");

        printf("\tFrom IP %s Port %d", ipStr, port);

        msg1.purpose = htonl(ORDR_CONFIRM);
        msg1.numFac = htonl(N);
        sendto(sd, (void*) &msg1, sizeof(msgBuf), 0, (SA*) &clntSkt, clntLen);

        printf("\n\nFACTORY sent this Order Confirmation to the client " );
        printMsg(  & msg1 );  puts("");

        pthread_t thrd[N]; //should it be N + 1? and should it be i <= N??
        params_t *args;

        gettimeofday( &now, NULL ) ;
        for (int i = 0; i < N; i++){ // should it start at 0 or 1???
            args = (params_t*) malloc (sizeof(params_t));
            if(!args){
                puts("Out of Memory");
                exit(-1);
            }

            args->factoryID = i + 1;
            args->capacity = (rand() % 41) + 10;
            args->duration = (rand() % 701) + 500;

            Pthread_create(&thrd[i], NULL, subFactory, (void*) args);
            
            pthread_mutex_lock(&activeMutex);
            activeThreads++;
            pthread_mutex_unlock(&activeMutex);

            printf("Created Factory Thread # %d with capacity =   %d parts in  %d mSec\n", args->factoryID, args->capacity, args->duration);
        }

        while(1)
        {
            gettimeofday (&end, NULL) ;
            pthread_mutex_lock(&activeMutex);
            if (activeThreads == 0)
            {
                pthread_mutex_unlock(&activeMutex);
                break;
            }
            pthread_mutex_unlock(&activeMutex);
            sleep(1);
        }
        
        for(int i = 0; i < N; i++){
            Pthread_join(thrd[i], (void **) &results[i]);
        }
        
        printf("\n****** FACTORY Server ( by ANNA-MARIA LLESHI and CAMERON SATERI ) Summary Report ******\n");
        printf("    Sub-Factory       Parts Made       Iterations\n");
        for (int i = 0; i < N; i++){
            pthread_mutex_lock(&activeMutex);
            grandTotalPartsmade += results[i]->totalPartsMade;
            pthread_mutex_unlock(&activeMutex);
            printf("              %3d              %3d              %3d\n", results[i]->factoryID, results[i]->totalPartsMade, results[i]->iterations);
        }
        printf("===================================================\n");
        printf("Grand total parts made   =    %3d   vs   order size of    %3d\n\n", grandTotalPartsmade, orderSize);
        
        printf("Order-to-Completion time =   %.1ld milliSeconds\n\n", (end.tv_sec - now.tv_sec) * 1000 + (end.tv_usec - now.tv_usec) / 1000);

        //subFactory( 1 , 50 , 350 ) ;  // Single factory, ID=1 , capacity=50, duration=350 ms
    }
    
    
    close(sd);
    return 0 ;
}

void *subFactory( void * ptr )
{
    params_t *args = (params_t*) ptr;
    int myFactoryID = args->factoryID;
    int myCapacity = args->capacity;
    int myDuration = args->duration;

    char    strBuff[ MAXSTR ] ;   // snprint buffer
    int     partsImade = 0 , myIterations = 0 ;
    msgBuf  msg;

    while ( 1 )
    {
        pthread_mutex_lock(&activeMutex);
        // See if there are still any parts to manufacture
        if ( remainsToMake <= 0 ){
            pthread_mutex_unlock(&activeMutex);
            break ;   // Not anymore, exit the loop
        }

        int toMake = minimum(remainsToMake, myCapacity);
        Usleep(myDuration * 1000);
        remainsToMake -= toMake;
        partsImade += toMake;
        myIterations++;
        
        // Send a Production Message to Supervisor
        msg.purpose = htonl(PRODUCTION_MSG);
        msg.facID = htonl(myFactoryID);
        msg.capacity = htonl(toMake);
        msg.partsMade = htonl(toMake);
        msg.duration = htonl(myDuration);

        pthread_mutex_unlock(&activeMutex);
        
        printf("Factory ( by ANNA-MARIA LLESHI and CAMERON SATERI )  #  %d: Going to make   %d parts in    %d mSec\n", 
        ntohl(msg.facID), ntohl(msg.partsMade), ntohl(msg.duration));
        
        sendto(sd, (void*) &msg, sizeof(msgBuf), 0, (SA*) &clntSkt, sizeof(clntSkt));
    }

    // Send a Completion Message to Supervisor
    msg.purpose = htonl(COMPLETION_MSG);
    msg.facID = htonl(myFactoryID);
    msg.capacity = htonl(myCapacity);
    msg.partsMade = htonl(partsImade);
    msg.duration = htonl(myDuration * myIterations); //or should it me * myDuration??
    
    pthread_mutex_lock(&activeMutex);
    activeThreads--;
    pthread_mutex_unlock(&activeMutex);
    
    sendto(sd, (void*) &msg, sizeof(msgBuf), 0, (SA*) &clntSkt, sizeof(clntSkt));

    snprintf( strBuff , MAXSTR , ">>> Factory # %-3d: Terminating after making total of %-5d parts in %-4d iterations\n" 
          , myFactoryID, partsImade, myIterations);
    factLog( strBuff ) ;

    //iterations = myIterations;

    factoryResults_t *result = malloc( sizeof (factoryResults_t));
    result->factoryID = myFactoryID;
    result->iterations = myIterations;
    result->totalPartsMade = partsImade;

    free(args);
    pthread_exit((void *) result);
}
