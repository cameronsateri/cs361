//---------------------------------------------------------------------
// Assignment : PA-04 Multi-Threaded UDP Server
// Date       : 12/03/2024
// Author     : Anna-Maria Lleshi lleshiax@dukes.jmu.edu
//              Cameron Sateri satericj@dukes.jmu.edu
// File Name  : procurement.c
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

#include "wrappers.h"
#include "message.h"

#define MAXFACTORIES    20

typedef struct sockaddr SA ;

/*-------------------------------------------------------*/
int main( int argc , char *argv[] )
{
    struct timeval now, end;
    gettimeofday( &now, NULL ) ;
    
    int     numFactories ,      // Total Number of Factory Threads
            activeFactories ,   // How many are still alive and manufacturing parts
            iters[ MAXFACTORIES+1 ] = {0} ,  // num Iterations completed by each Factory
            partsMade[ MAXFACTORIES+1 ] = {0} , totalItems = 0;

    char  *myName = "Anna-Maria Lleshi and Cameron Sateri" ; 
    printf("\nThis is PROCUREMENT.  ( by %s )\n\n" , myName );    

    char myUserName[30] ;
    getlogin_r ( myUserName , 30 ) ;
    
    // fprintf( stdout , "Logged in as user '%s' on %s\n" , myUserName ,  ctime( &now)  ) ;
    fflush( stdout ) ;
    
    if ( argc < 4 )
    {
        printf("PROCUREMENT Usage: %s  <order_size> <FactoryServerIP>  <port>\n" , argv[0] );
        exit( -1 ) ;  
    }

    unsigned        orderSize  = atoi( argv[1] ) ;
    char	       *serverIP   = argv[2] ;
    unsigned short  port       = (unsigned short) atoi( argv[3] ) ;
 

    /* Set up local and remote sockets */
    int sd = socket( AF_INET, SOCK_DGRAM, 0);

    // Prepare the server's socket address structure
    struct sockaddr_in srvrSkt;
    memset(&srvrSkt, 0, sizeof(srvrSkt));

    srvrSkt.sin_family = AF_INET;
    srvrSkt.sin_port = htons(port);

    printf("Attempting Factory server at '%s' : %d\n", serverIP, port);

    if(inet_pton(AF_INET, serverIP, (void *) &srvrSkt.sin_addr.s_addr) != 1){
        printf("\nInvalid server IP address");
        exit(2);
    }

    // Send the initial request to the Factory Server
    msgBuf msg1;

    msg1.purpose = htonl(REQUEST_MSG);
    msg1.orderSize = htonl(orderSize);

    sendto(sd, (void *) &msg1, sizeof(msgBuf), 0, (SA*)&srvrSkt, sizeof(srvrSkt));

    printf("\nPROCUREMENT Sent this message to the FACTORY server: "  );
    printMsg( &msg1 );  puts("");

    /* Now, wait for oreder confirmation from the Factory server */
    msgBuf  msg2;
    printf ("\nPROCUREMENT is now waiting for order confirmation ...\n" );

    unsigned srvrSktLen = sizeof(srvrSkt);
    if(recvfrom(sd, (void *) &msg2, 1 + sizeof(msgBuf), 0, (SA*)&srvrSkt, &srvrSktLen) < 0){
        printf("\nFailed to recieve order confirmation.");
        exit(EXIT_FAILURE);
    }

    printf("PROCUREMENT received this from the FACTORY server: "  );
    printMsg( & msg2 );  puts("\n");

    activeFactories = ntohl(msg2.numFac);
    msgBuf msg, msgs[activeFactories];

    // Monitor all Active Factory Lines & Collect Production Reports
    while ( activeFactories > 0 ) // wait for messages from sub-factories
    {   

        //My code starts here
        recvfrom(sd, (void *) &msg, sizeof(msgBuf), 0, (SA*)&srvrSkt, &srvrSktLen);

       // Inspect the incoming message
        if(ntohl(msg.purpose) == PRODUCTION_MSG){
            partsMade[ntohl(msg.facID)] += ntohl(msg.partsMade);
            iters[ntohl(msg.facID)]++;
            totalItems += ntohl(msg.partsMade);
            printf("PROCUREMENT: Factory #%d   produced %d   parts in %d   milliSecs\n", 
            ntohl(msg.facID), ntohl(msg.partsMade), ntohl(msg.duration));
        }else if(ntohl(msg.purpose) == COMPLETION_MSG){
            activeFactories--;
            printf("PROCUREMENT: FACTORY #%d        COMPLETED its task\n", ntohl(msg.facID));
            msgs[ntohl(msg.facID)] = msg;
        }else{
            printf("PROCUREMENT: Received invalid msg ");
            printMsg(&msg);
            printf("\n\n");
            exit(EXIT_FAILURE);
        }
       
    } 

    // Print the summary report
    printf("\n\n****** PROCUREMENT ( by ANNA-MARIA LLESHI and CAMERON SATERI ) Summary Report ******\n");
    printf("Sub-Factory\tParts Made\tIterations\n");
    for(int i = 1; i <= ntohl(msg2.numFac); i++)
    {
        printf("\t%d\t\t%d\t\t%d\n", i, ntohl(msgs[i].partsMade), iters[ntohl(msgs[i].facID)]);
    }
    // printf("Factory #  %d made a total of   %d parts in     %d iterations\n", 
    // ntohl(msg.facID), ntohl(msg.partsMade), iters[ntohl(msg.facID)]);

    printf("==============================\n") ;

    printf("Grand total parts made =   %d   vs   order size of   %d\n", 
    totalItems, ntohl(msg1.orderSize));

    gettimeofday (&end, NULL) ;

    printf("Order-to-Completion time =   %.1ld milliSeconds\n\n", (end.tv_sec - now.tv_sec) * 1000 + (end.tv_usec - now.tv_usec) / 1000);

    close(sd);
    return 0 ;
}
