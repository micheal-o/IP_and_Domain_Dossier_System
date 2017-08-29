#include <stdio.h>      /* for printf() and fprintf() */
#include <sys/socket.h> /* for socket(), bind(), and connect() */
#include <arpa/inet.h>  /* for sockaddr_in and inet_ntoa() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <unistd.h>     /* for close() */
#include "linkedlist.c"
#include "log.c"
#include <errno.h> //For errno - the error number
#include <netdb.h> //hostent
#include <time.h>

#define MAXPENDING 5    /* Maximum outstanding connection requests */
#define RCVBUFSIZE 120   /* Size of receive buffer */
#define SNDBUFSIZE 300
#define SECCODE "@ENTS689N"
char ipList[200]; //for temporary storage for file to linkedlist loading
void HandleTCPClient(int clntSocket,char clntAddr[]);
void loadLinkedList(char *fileName);
int hostname_to_ip(char *  , char *);
char client_Response[300];
char log_Mssg[100];
double reqGap;
char *dnsFile;
//char clientaddtemp[20];

int main(int argc, char *argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */
    struct sockaddr_in ServAddr; /* Local address */
    struct sockaddr_in ClntAddr; /* Client address */
    unsigned short ServPort;     /* Server port */
    unsigned int clntLen;            /* Length of client address data structure */


    if (argc != 4)     /* Test for correct number of arguments */
    {
        fprintf(stderr, "Usage:  %s <Server Port>\n", argv[0]);
        exit(1);
    }
    
    ServPort = atoi(argv[1]);  /* First arg:  local port */
    dnsFile= argv[2];
    reqGap= (double) atoi(argv[3]);
    //load domain name file into linkedlist
    loadLinkedList(dnsFile);
    printf("i got before sock");
    /* Create socket for incoming connections */
    if ((servSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        perror("socket() failed");
      
    /* Construct local address structure */
    memset(&ServAddr, 0, sizeof(ServAddr));   /* Zero out structure */
    ServAddr.sin_family = AF_INET;                /* Internet address family */
    ServAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* Any incoming interface */
    ServAddr.sin_port = htons(ServPort);      /* Local port */
    printf("about to bind\n");
    /* Bind to the local address */
    if (bind(servSock, (struct sockaddr *) &ServAddr, sizeof(ServAddr)) < 0){
        perror("bind() failed");
    }
    printf("bind successful \n");
    /* Mark the socket so it will listen for incoming connections */
    printf("waiting for connection... \n");
    if (listen(servSock, MAXPENDING) < 0){
        perror("listen() failed");
    }

    for (;;) /* Run forever */
    {
        /* Set the size of the in-out parameter */
        clntLen = sizeof(ClntAddr);
        printf("waiting for connection...");
        /* Wait for a client to connect */
        if ((clntSock = accept(servSock, (struct sockaddr *) &ClntAddr, 
                               &clntLen)) < 0)
            perror("accept() failed");

        /* clntSock is connected to a client! */

        printf("Handling client %s\n", inet_ntoa(ClntAddr.sin_addr));
        //strcpy(clientaddtemp,inet_ntoa(ClntAddr.sin_addr));
        HandleTCPClient(clntSock,inet_ntoa(ClntAddr.sin_addr));
    }
    /* NOT REACHED */
}

void HandleTCPClient(int clntSocket, char clntAddr[])
{
	char logMssg[200];
	memset(logMssg,0,sizeof(logMssg));
	//char logStatus[10];
	printf("inside client handler for %s \n",clntAddr);
	sprintf(logMssg,"Connection to new client %s",clntAddr);
	createLog(logMssg, "success");
    char mssgBuffer[RCVBUFSIZE];        /* Buffer for echo string */
    int recvMsgSize;                    /* Size of received message */
    char reqCode;
    memset(mssgBuffer,0,sizeof(mssgBuffer));
    /* Receive message from client */
    if ((recvMsgSize = recv(clntSocket, mssgBuffer, RCVBUFSIZE, 0)) < 0){
        perror("recv() failed: ");
        sprintf(logMssg,"Receiving message from %s",clntAddr);
        createLog(logMssg, "failed");
    }
    const char s[2] = ",";
    char *token;
    //unsigned int temp=0;
    /* get the first token */
    reqCode = *(strtok(mssgBuffer, s));
    //reqCount = atoi(strtok(NULL, s));
    //printf("%c - domain to find ip is: %s \n",reqCode,strtok(NULL, s));
    char *domain;
    struct node *temp;
    switch(reqCode){
    case '1' :{ ;//empty statement
    	//Find IP for a domain
    	char ip[200];
    	memset(ip,0,sizeof(ip));
    	domain= strtok(NULL, s);
    	printf("domain= %s \n",domain);
    	temp= find(domain);
    	if(temp == NULL){
    		//domain not found in linkedlist
    		int err= hostname_to_ip(domain, ip);
    		if(strcmp(ip,"\0")==0 || err==0){
    			//unable to find domain in linkedlist and dns
    			strcpy(client_Response,"Unable to resolve domain, please check if its valid...");
    			//send log message to log file
    			sprintf(logMssg,"Name resolution request (%s) from %s",domain,clntAddr);
    			createLog(logMssg, "failed");
    			if (send(clntSocket, client_Response, SNDBUFSIZE, 0) < 0){
    				perror("Unable to send message to client: ");
    			}

    		}else{
    			//domain found via dns
    			time_t curtime;
    			time(&curtime);

    			insertFirst(domain, 1, ip, curtime);//add to linkedlist
    			//printf("domain add time: %s \n",(char *)curtime);
    			strcpy(client_Response,domain);
    			strcat(client_Response," ");
    			strcat(client_Response,ip);
    			//send log message to log file
    			sprintf(logMssg,"Name resolution request (%s) from %s",domain,clntAddr);
    			createLog(logMssg, "success");
    			if (send(clntSocket, client_Response, SNDBUFSIZE, 0) < 0){
    				perror("Unable to send message to client: ");
    			}
    		}
    	}
    	else{
    		//domain found in linkedlist
    		//Mon Aug 13 08:23:14 2012
    		(temp->reqCount) += 1;//increase request count
    		time_t curtime;
    		time(&curtime);

    		printf("about to find using request gap = %g \n",reqGap);
    		double secs= difftime(curtime,temp->lastReqTime);//date difference in seconds
    		printf("time diff between req: %g \n", secs);
    		//printf("times: %s / %s \n",ctime(&curtime),(temp->lastReqTime);
    		if(secs < reqGap){
    			//within request gap
    			//(temp->reqCount) += 1;
    			sprintf(client_Response,"Another inquiry had been made %g seconds ago, wait %g seconds before another submission...", secs,(reqGap-secs));
    			//send log message to log file
    			sprintf(logMssg,"Name resolution rejected (%s) from %s, wait %g secs",domain,clntAddr, (double)(reqGap-secs));
    			createLog(logMssg, "failed");
    			if (send(clntSocket, client_Response, SNDBUFSIZE, 0) < 0){
    				perror("Unable to send message to client: ");
    			}
    		}
    		else{
    			//after request gap
    			time_t curtime;
    			time(&curtime);
    			//strcpy(temp->lastReqTime,ctime(&curtime));//update last request time
    			temp->lastReqTime = time(&curtime);
    			strcpy(client_Response,temp->domName);
    			strcat(client_Response," ");
    			strcat(client_Response,temp->ipList);
    			//(temp->reqCount) += 1;
    			//send log message to log file
    			sprintf(logMssg,"Name resolution request (%s) from %s",domain,clntAddr);
    			createLog(logMssg, "success");
    			if (send(clntSocket, client_Response, SNDBUFSIZE, 0) < 0){
    				perror("Unable to send message to client: ");
    			}
    		}

    	}
    	break;}
    case '2' :{ ;//empty statement
    	   //printf("Current time = %s", ctime(&curtime));
    	//Add a record to the list
    	char ipadd[30];
    	memset(ipadd,0,sizeof(ipadd));
    	//char *domain= strtok(NULL, s);
    	domain= strtok(NULL, s);
    	strcpy(ipadd,strtok(NULL, s));
    	temp= find(domain);
    	if(temp == NULL){
    		//domain not in list
    		time_t curtime;
    		time(&curtime);
    		insertFirst(domain, 0, ipadd,curtime);//add to linkedlist
    		sprintf(client_Response,"%s successfully added",domain);
    		//send log message to log file
    		sprintf(logMssg,"(%s,%s) added by %s",domain,ipadd,clntAddr);
    		createLog(logMssg, "success");
    		if (send(clntSocket, client_Response, SNDBUFSIZE, 0) < 0){
    			perror("Unable to send message to client: ");
    		}
    	}else{
    		//domain in list
    		sprintf(client_Response,"%s information already exist in the system...",domain);
    		//send log message to log file
    		sprintf(logMssg,"(%s,%s) added by %s, already exist",domain,ipadd,clntAddr);
    		createLog(logMssg, "failed");
    		if (send(clntSocket, client_Response, SNDBUFSIZE, 0) < 0){
    			perror("Unable to send message to client: ");
    		}
    	}
    	break;}
    case '3':{ ;//empty statement
    	//Delete a record from the list
    	//char *domain= strtok(NULL, s);
    	domain= strtok(NULL, s);
    	printf("domain to delete: %s \n",domain);
    	temp= find(domain);
    	if(temp == NULL){
    		//record to delete doesn't exists
    		sprintf(client_Response,"Unable to delete...%s doesn't exist in the system...",domain);
    		//send log message to log file
    		sprintf(logMssg,"(%s) delete request by %s, record doesn't exist",domain,clntAddr);
    		createLog(logMssg, "failed");
    		if (send(clntSocket, client_Response, SNDBUFSIZE, 0) < 0){
    			perror("Unable to send message to client: ");
    		}
    	}else{
    		//record found
    		delete(temp);
    		sprintf(client_Response,"%s successfully deleted...",domain);
    		//send log message to log file
    		sprintf(logMssg,"(%s) delete request by %s",domain,clntAddr);
    		createLog(logMssg, "success");
    		if (send(clntSocket, client_Response, SNDBUFSIZE, 0) < 0){
    			perror("Unable to send message to client: ");
    		}

    	}
    	break;}
    case '4':{ ;//empty statement
    	//Report the most requested record(s)
    	temp= mostRequest();
    	sprintf(client_Response,"Most Requests: %d request(s) %s %s ",temp->reqCount,temp->domName,temp->ipList);
    	//send log message to log file
    	sprintf(logMssg,"(Most Requests) requested by %s",clntAddr);
    	createLog(logMssg, "success");
    	if (send(clntSocket, client_Response, SNDBUFSIZE, 0) < 0){
    		perror("Unable to send message to client: ");
    	}
    	break;}

    case '5':{ ;//empty statement
    	//Report the least requested record(s)
    	temp= leastRequest();
    	sprintf(client_Response,"Least Requests: %d request(s) %s %s ",temp->reqCount,temp->domName,temp->ipList);
    	//send log message to log file
    	sprintf(logMssg,"(Least Requests) requested by %s",clntAddr);
    	createLog(logMssg, "success");
    	if (send(clntSocket, client_Response, SNDBUFSIZE, 0) < 0){
    		perror("Unable to send message to client: ");
    	}
    	break;}
    case '6':{ ;//empty statement
    	//Shut down
    	//FILE *dnsfp;SECCODE

    	char *secCode= strtok(NULL, s);
    	//printf("sent password: %s \n",secCode);
    	if(strcmp(SECCODE,secCode)==0){
    		//security code match
    		FILE *dnsfp= fopen(dnsFile,"w");
    		printList(dnsfp);
    		fclose(dnsfp);
    		strcpy(client_Response,"Security check passed...Server shutting down...");
    		//send log message to log file
    		sprintf(logMssg,"Server shutdown request by %s",clntAddr);
    		createLog(logMssg, "success");
    		if (send(clntSocket, client_Response, SNDBUFSIZE, 0) < 0){
    			perror("Unable to send message to client: ");
    		}
    		printf("\nshutdown command received...server shutting down");
    		//printf("\%s sent shutdown request...security check passed...server sutting down...\n",clientaddtemp);
    		sprintf(logMssg,"Server is offline");
    		createLog(logMssg, "shutdown");
    		close(clntSocket);
    		exit(0);
    	}else{
    		//security code didn't match
    		strcpy(client_Response,"Invalid security code...Security check failed...");
    		//send log message to log file
    		sprintf(logMssg,"Server shutdown request by %s (Invalid code)",clntAddr);
    		createLog(logMssg, "failed");
    		if (send(clntSocket, client_Response, SNDBUFSIZE, 0) < 0){
    			perror("Unable to send message to client: ");
    		}
    	}
    	break;}
    default:{;}
    	//return error in request code
    }
    /* Send received string and receive again until end of transmission */
//while
    close(clntSocket);    /* Close client socket */
}

void loadLinkedList(char *fileName){
	memset(ipList,0,sizeof(ipList));
	char domName[72]; /*Max length of domain name label is 63 + 8+ 1*/
	memset(domName,0,sizeof(domName));
	unsigned int reqCount;
	unsigned int ipCount;
	struct tm date_1;//default date & time structure
	time_t def_date;//default date & time on file load
	//char ipList[30][16];
	date_1.tm_hour=0; date_1.tm_min=0; date_1.tm_sec=0; date_1.tm_mon=12; date_1.tm_mday=31; date_1.tm_year=15;
	def_date= mktime(&date_1);
	//printf("default date: %s \n", (char *) def_date);
	FILE *dnsfp = fopen(fileName,"r");//"testdnsfile.txt"
	if(dnsfp==NULL){
		perror("Dns File Error: ");
		exit(1);
	}
	char line[1024];
	memset(line,0,sizeof(line));
	while (fgets(line, 1024, dnsfp)){
		const char s[2] = " ";
		char *token;
		unsigned int temp=0;
		/* get the first token */
		strcpy(domName,strtok(line, s));
		reqCount = atoi(strtok(NULL, s));
		token= strtok(NULL, s);
		//copy first ip address
		memset(ipList,0,sizeof(ipList));
		while( token != NULL )
		{
			//strcpy(ipList[temp],token);
			strcat(ipList,token);
			strcat(ipList," ");
			token = strtok(NULL, s);
		}
		printf("%s %d %s \n",domName,reqCount, ipList);
		insertFirst(domName, reqCount, ipList, def_date );
		printf("1 \n");
	}
	fclose(dnsfp);
	printf("done loading");
}

int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ( (he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        herror("gethostbyname");
        return 0;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++)
    {
    	strcat(ip," ");
        strcat(ip , inet_ntoa(*addr_list[i]) );
    }

    return 1;
}
