//Client application that sends requests to the server.

#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), connect(), send(), and recv() */
#include <sys/types.h>  /* for get hostbyname() */
#include <netdb.h>
#include <arpa/inet.h>  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */

#include "DieWithError.c" /*Includes the standard error function source file*/

#define RCVBUFSIZE 300   /* Size of receive buffer */

void DieWithError(char *errorMessage);  /* Error handling function */

//Function prints the menu for the IP Domain Dossier system

void printMenu()
{
    printf("\n------WELCOME TO IP DOMAIN DOSSIER CLIENT------\n");
    printf("\nRequest Code\tAction\n");
    printf("1\t\tFind IP for a domain e.g. client 129.2.133.250 1070 1 www.yahoo.com\n");
    printf("2\t\tAdd a record to the list e.g. client 129.2.133.250 1070 2 www.ebay.com 10.125.6.138\n");
    printf("3\t\tDelete a record from the list e.g. client 129.2.133.250 1070 3 www.ebay.com\n");
    printf("4\t\tReport the most requested record(s) e.g. client 129.2.133.250 1070 4\n");
    printf("5\t\tReport the least requested record(s) e.g. client 129.2.133.250 1070 5\n");
    printf("6\t\tShut Down e.g. client 129.2.133.250 1070 6 <SECURITYCODE>\n\n");
    
}

//Function to check if a given string is a valid IPv4 address

int isValidIp(char *st)
{
    int num, i, len;
    char *ch;
    
    // Counting number of octets present in a given IP address
    int octCnt = 0;
    len = strlen(st);
    
    //  Check if the entered string is valid
    if (len<7 || len>15)
        return 0;
    
    // Using strtok() to split the IP with the delimiter as "."
    ch = strtok(st, ".");
    int length = strlen(ch);
    
    while (ch != NULL)
    {
        octCnt++;
        num = 0;
        i = 0;
        char *ip_str = ch;
        
        // Loop to check whether current token is a number or not
        while (*ip_str)
        {
            if (*ip_str >= '0' && *ip_str <= '9')
                ++ip_str;
            else
            {
                printf("Please enter numbers only for IP addresses! \n");
                return 0;
            }
        }
        
        // Get the current token and convert to an integer value
        while (ch[i] != '\0')
        {
            num = num * 10;
            num = num + (ch[i] - '0');
            i++;
        }
        
        // Checking if the integer<0 or integer>255
        if (num < 0 || num > 255)
        {
            printf("IP Address octet is not in the range of 0 - 255\n");
            return 0;
        }
        
        ch = strtok(NULL, ".");
        
    }
    
    //  Checking for the format of the IP address (X.X.X.X)
    if (octCnt != 4)
    {
        printf("IP Address format is X.X.X.X");
        return 0;
    }
    
    //  Entered string is a valid IP address
    return 1;
}

int main(int argc, char *argv[])
{
    int sock; /* Socket descriptor */
    struct sockaddr_in ServAddr; /* server address */
    unsigned short ServPort; /* server port */
    char *servIP; /* Server IP address (dotted quad) */
    //char *SECURITYCODE = "ENTS_689N";
    unsigned int sendStringLen; /* Length of string to send */
    int bytesRcvd; /* Bytes read in single recv() and total bytes read */
    unsigned int len1, len2, len3;
    
    char recvString[RCVBUFSIZE];
    char reqCode;
    
    if ((argc < 4) || (argc > 6))    /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage: Filename <Server IP> <Port Number> <Request Code> <Optional arguments...>");
        exit(1);
  
    }
    
    /* Check if Server IP is valid */
    if (isValidIp(argv[1]) == 0)
        DieWithError("Server IP address is invalid. Exiting...");
    
    //servIP = argv[1];
    servIP = "127.0.0.1";
    ServPort = atoi(argv[2]);
    
    /* Create a reliable, stream socket using TCP */
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        DieWithError("socket() failed");
    
    /* Construct the server address structure */
    memset(&ServAddr, 0, sizeof(ServAddr)); /* Zero out structure */
    ServAddr.sin_family = AF_INET; /* Internet address family */
    ServAddr.sin_addr.s_addr = inet_addr(servIP); /* Server IP address */
    ServAddr.sin_port = htons(ServPort); /* Server port */
    
    /* Establish the connection to the server */
    if (connect(sock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0)
        DieWithError("connect() failed");
    
    /* Print User Menu */
    printMenu();
    
    if (strlen(argv[3]) != 1)
        DieWithError("Error! Invalid Request Code. Exiting...");
        
    reqCode = argv[3][0];
    switch(reqCode)
    {
        case '1':{ //Request Code 1, Look up IP adress of the given Domain name
            
            if (argc == 5)
            {
                if (gethostbyname(argv[4]) == NULL)
                {   printf("Error! Invalid Domain Name entered. Exiting...");
                    exit(1);
                }
                
                len1 = strlen(argv[3]);
                len2 = strlen(argv[4]);
                
                //sendString = argv[3];
                sendStringLen = len1 + len2 + 1; //Only the request code will be sent
                char sendString[sendStringLen];
                snprintf(sendString, sizeof sendString+1, "%s,%s",argv[3],argv[4]);
                
                
                /* Send the string to the server */
                if (send(sock, sendString, sendStringLen, 0) != sendStringLen)
                    DieWithError("send() sent a different number of bytes than expected");
            
                /* Receive response from the server */
                if ((bytesRcvd = recv(sock, recvString, RCVBUFSIZE - 1, 0)) <= 0)
                    DieWithError("recv() failed or connection closed prematurely");
            
                recvString[bytesRcvd] = '\0';
                printf("Bytes recvd = %d",bytesRcvd);
        
                /* Printing server response */
                printf("\n%s", recvString);
            }
        
            else
                DieWithError("Error! Invalid No. of arguments for Request Code 1. Please check the syntax given above.\n\n");
        } break;
            
        case '2':{
            
            if (argc == 6)
            {
                if (isValidIp(argv[5]) == 0)
                    DieWithError("IP address for the specified domain is invalid. Exiting...");
                
                if (gethostbyname(argv[4]) == NULL)
                {   printf("Error! Invalid Domain Name entered. Exiting...");
                    exit(1);
                }
                
                len1 = strlen(argv[3]);
                len2 = strlen(argv[4]);
                len3 = strlen(argv[5]);
                
                sendStringLen = len1 + len2 + len3 + 1;
                char sendString[sendStringLen];
                snprintf(sendString, sizeof sendString+1, "%s,%s,%s",argv[3],argv[4],argv[5]);
                
                /* Send the string to the server */
                if (send(sock, sendString, sendStringLen, 0) != sendStringLen)
                    DieWithError("send() sent a different number of bytes than expected");
                
                /* Receive response from the server */
                if ((bytesRcvd = recv(sock, recvString, RCVBUFSIZE - 1, 0)) <= 0)
                    DieWithError("recv() failed or connection closed prematurely");
                
                recvString[bytesRcvd] = '\0';
                
                /* Printing server response */
                printf("\n%s", recvString);
                
            }
            
            else
                DieWithError("Error! Invalid No. of arguments for Request Code 2. Please check the syntax given above.\n\n");
            
        } break;
            
        case '3':{
            
            if (argc == 5){
                
                if (gethostbyname(argv[4]) == NULL)
                {   printf("Error! Invalid Domain Name entered. Exiting...");
                    exit(1);
                }
                
                len1 = strlen(argv[3]);
                len2 = strlen(argv[4]);
                
                //sendString = argv[3];
                sendStringLen = len1 + len2 + 1; //Only the request code will be sent
                char sendString[sendStringLen];
                snprintf(sendString, sizeof sendString+1, "%s,%s",argv[3],argv[4]);
                
                
                /* Send the string to the server */
                if (send(sock, sendString, sendStringLen, 0) != sendStringLen)
                    DieWithError("send() sent a different number of bytes than expected");
                
                /* Receive response from the server */
                if ((bytesRcvd = recv(sock, recvString, RCVBUFSIZE - 1, 0)) <= 0)
                    DieWithError("recv() failed or connection closed prematurely");
                
                recvString[bytesRcvd] = '\0';
                
                /* Printing server response */
                printf("\n%s", recvString);
                
            }
            
            else
                DieWithError("Error! Invalid No. of arguments for Request Code 3. Please check the syntax given above.\n\n");
            
        } break;
            
        case '4':
        case '5':{
            
            if (argc == 4){
                
                /* Send the string to the server */
                if (send(sock, &reqCode, 1, 0) != 1)
                    DieWithError("send() sent a different number of bytes than expected");
                
                /* Receive response from the server */
                if ((bytesRcvd = recv(sock, recvString, RCVBUFSIZE - 1, 0)) <= 0)
                    DieWithError("recv() failed or connection closed prematurely");
                
                recvString[bytesRcvd] = '\0';
                
                if (reqCode == '4'){
                /* Printing server response */
                printf("\n%s\n", recvString);
                
                }
                else if (reqCode == '5'){
                    
                /* Printing server response */
                printf("\n%s\n", recvString);
                    
                }
            
            }
            else
                DieWithError("Error! Invalid No. of arguments for Request Code 4/5. Please check the syntax given above.\n\n");
            
        } break;
    
        case '6':{
            
            if (argc == 5){
                
                len1 = strlen(argv[4]);
                sendStringLen = len1 + 2;
                char sendString[sendStringLen];
                snprintf(sendString, sizeof sendString+1, "%C,%s",reqCode,argv[4]);
                printf("sec code: %s \n",sendString);
                printf("ARGV4: %s \n",argv[4]);


                
                /* Send the string to the server */
                if (send(sock, sendString, sendStringLen, 0) != sendStringLen)
                    DieWithError("send() sent a different number of bytes than expected");
                
                /* Receive response from the server */
                if ((bytesRcvd = recv(sock, recvString, RCVBUFSIZE - 1, 0)) <= 0)
                    DieWithError("recv() failed or connection closed prematurely");
                
                recvString[bytesRcvd] = '\0';
                
                /* Printing server response */
                printf("\n%s", recvString);
                
            }
            else
                DieWithError("Error! Invalid No. of arguments for Request Code 6. Please check the syntax given above.\n\n");
                
        } break;
    }
    
    printf("\n"); /* Print a final linefeed */
    
    close(sock);
    exit(0);

}
