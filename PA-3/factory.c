//---------------------------------------------------------------------
// Assignment : PA-03 UDP Single-Threaded Server
// Date       : 11/19/2024
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

void subFactory( int factoryID , int myCapacity , int myDuration ) ;

void factLog( char *str )
{
    printf( "%s" , str );
    fflush( stdout ) ;
}

/*-------------------------------------------------------*/

// Global Variable for Future Thread to Shared
int   remainsToMake , // Must be protected by a Mutex
      actuallyMade ;  // Actually manufactured items

int   numActiveFactories = 1 , orderSize ;

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
    time_t  now;
    time( &now ) ;
    fprintf( stdout , "Logged in as user '%s' on %s\n\n" , myUserName ,  ctime( &now)  ) ;
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


    // our code starts here

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

    // our code ends here


    int forever = 1;
    while ( forever )
    {
        printf( "\nFACTORY server waiting for Order Requests\n\n" ) ; 

        // our code starts here
        
        msgBuf msg1;
        socklen_t clntLen = sizeof(clntSkt);

        recvfrom(sd, (void*) &msg1, sizeof(msgBuf), 0, (SA *) &clntSkt, &clntLen);
        // our code end here

        char ipStr[IPSTRLEN];
        inet_ntop(AF_INET, (void*) &clntSkt.sin_addr.s_addr, ipStr, IPSTRLEN);

        orderSize = ntohl(msg1.orderSize);
        remainsToMake = orderSize;
        actuallyMade = 0;
        //printf("orderSize: %d\n", ntohl(msg1.orderSize));
        //printf("orderSize: %d\n", htonl(msg1.orderSize));
        //printf("orderSize: %d\n", msg1.orderSize);
        //printf("orderSize: %d\n", orderSize);
        //printf("remainsToMake: %d\n", ntohl(remainsToMake));
        //printf("remainsToMake: %d\n", htonl(remainsToMake));
        //printf("remainsToMake: %d\n", remainsToMake);

        printf("\nFACTORY server received: " ) ;
        printMsg( & msg1 );  puts("");

        // our code starts here
        printf("\tFrom IP %s Port %d", ipStr, port);


        msg1.purpose = htonl(ORDR_CONFIRM);
        sendto(sd, (void*) &msg1, sizeof(msgBuf), 0, (SA*) &clntSkt, clntLen);

        // our code end here


        printf("\n\nFACTORY sent this Order Confirmation to the client " );
        printMsg(  & msg1 );  puts("");
        
        subFactory( 1 , 50 , 350 ) ;  // Single factory, ID=1 , capacity=50, duration=350 ms

    }
    printf("I am getting to this part in my code!!!!\n");
    close(sd);
    return 0 ;
}

void subFactory( int factoryID , int myCapacity , int myDuration )
{
    char    strBuff[ MAXSTR ] ;   // snprint buffer
    int     partsImade = 0 , myIterations = 0 ;
    msgBuf  msg;

    while ( 1 )
    {
        // See if there are still any parts to manufacture
        if ( remainsToMake <= 0 ){
            break ;   // Not anymore, exit the loop
        }
        // missing code goes here
        int toMake = minimum(remainsToMake, myCapacity);
        Usleep(myDuration * 1000);
        remainsToMake -= toMake;
        partsImade += toMake;
        myIterations++;
        
        // Send a Production Message to Supervisor
        msg.purpose = htonl(PRODUCTION_MSG);
        msg.facID = htonl(factoryID);
        msg.capacity = htonl(toMake);
        msg.partsMade = htonl(toMake);
        msg.duration = htonl(myDuration);

        printf("Factory #  %d: Going to make   %d parts in    %d mSec\n", 
        ntohl(msg.facID), ntohl(msg.partsMade), ntohl(msg.duration));
        
        sendto(sd, (void*) &msg, sizeof(msgBuf), 0, (SA*) &clntSkt, sizeof(clntSkt));

        // missing code goes here


    }

    // Send a Completion Message to Supervisor
    msg.purpose = htonl(COMPLETION_MSG);
    msg.facID = htonl(factoryID);
    msg.capacity = htonl(myCapacity);
    msg.partsMade = htonl(partsImade);
    msg.duration = htonl(myDuration * myDuration);

    sendto(sd, (void*) &msg, sizeof(msgBuf), 0, (SA*) &clntSkt, sizeof(clntSkt));

    // missing code goes here



    snprintf( strBuff , MAXSTR , ">>> Factory # %-3d: Terminating after making total of %-5d parts in %-4d iterations\n" 
          , factoryID, partsImade, myIterations);
    factLog( strBuff ) ;
    
    //close(sd);

}

