#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //for exit(0);
#include<sys/socket.h>
#include<errno.h> //For errno - the error number
#include<netdb.h> //hostent
#include<arpa/inet.h>

int hostname_to_ip(char *  , char *);

int main()
{
    char *hostname = "www.yahoo.com";
    char ip[200];

    hostname_to_ip(hostname , ip);
    printf("%s resolved to %s" , hostname , ip);

    printf("\n");

}
/*
    Get ip from domain name
 */

int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *he;
    struct in_addr **addr_list;
    int i;

    if ( (he = gethostbyname( hostname ) ) == NULL)
    {
        // get the host info
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **) he->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++)
    {
        strcat(ip , inet_ntoa(*addr_list[i]) );
        strcat(ip," ");
    }

    return 1;
}

//insert link at the first location
void insertFirst(char *domName, unsigned int reqCount, char ipList[], char *lastReqTime ) { //char key[100], int data
   //create a link
	//printf("i got here: %s ",ipList);
   struct node *link = (struct node*) malloc(sizeof(struct node));

   strcpy(link->domName,domName);
   link->reqCount = reqCount;
   //link->ipCount = ipCount;
   strcpy(link->lastReqTime,lastReqTime);
   strcpy(link->ipList,ipList);
   //link->data = data;
   /*for(int i=0; i< ipCount; i++){
	   strcpy(link->ipList[i],ipList[i]);
   }*/
   //point it to old first node
   link->next = head;
   if(head != NULL){
  	   head->prev = link;
     }
   //point first to new first node
   head = link;
}

//find a link with given key
struct node* find(char domName[]) {
	//mode= 1 if its new request so request count can be increased, mode= 0 if its find before add or find before delete
   //start from the first link
   struct node* current = head;

   //if list is empty
   if(head == NULL) {
      return NULL;
   }

   //convert to lowercase for comparison (for case insensitivity)
   char lwrstr1[72], lwrstr2[72];
   for(int i = 0; current->domName[i]; i++){ //convert string to lower
	   lwrstr1[i] = tolower(current->domName[i]);
   }
   for(int i = 0; domName[i]; i++){ //convert string to lower
	   lwrstr2[i] = tolower(domName[i]);
   }

   //navigate through list
   while(strcmp(lwrstr1,lwrstr2)!=0) {

      //if it is last node
      if(current->next == NULL) {
         return NULL;
      } else {
         //go to next link
         current = current->next;
         strcpy(lwrstr1,"");//clear string
         for(int i = 0; current->domName[i]; i++){ //convert string to lower
        	 lwrstr1[i] = tolower(current->domName[i]);
         }
      }
   }
   //increasing request count
   /*if(mode == '1'){
	   //new request
	   (current->reqCount) += 1;
   }*/
   //if data found, return the current Link
   return current;
}

void delete(struct node* current){
	if(current->prev == NULL && current->next == NULL){ //only element
		free(current);
		head= NULL;
	}
	else if (current->prev == NULL && current->next != NULL){ //first element
		current->next->prev = current->prev;
		head= current->next;
		free(current);
	}
	else {
		current->next->prev = current->prev;
		current->prev->next= current->next;
		free(current);
	}
}

void main(){
	char domName[72]; /*Max length of domain name label is 63 + 8+ 1*/
	unsigned int reqCount;
	unsigned int ipCount;
	//char ipList[30][16];

	FILE *dnsfp = fopen("testdnsfile.txt","a+");
	if(dnsfp==NULL){
		perror("Dns File Error: ");
		exit(1);
	}
	char line[1024];
	while (fgets(line, 1024, dnsfp)){
		const char s[2] = " ";
		char *token;
		unsigned int temp=0;
		/* get the first token */
		strcpy(domName,strtok(line, s));
		reqCount = atoi(strtok(NULL, s));
		token= strtok(NULL, s);
		//copy first ip address
		strcpy(ipList,"");
		while( token != NULL )
		{
			//strcpy(ipList[temp],token);
			strcat(ipList,token);
			strcat(ipList," ");
			token = strtok(NULL, s);
		}
		/* walk through other tokens */
		/*while( token != NULL )
		{
			//strcpy(ipList[temp],token);
			strcat(ipList[temp],token);
		    token = strtok(NULL, s);
		    temp++;
		}*/
		//printing file line
		//printf("number of ip: %d \n",temp);
		printf("%s %d %s \n",domName,reqCount, ipList);
		//unsigned int temp2= 0;
		/*while(temp2 < temp){
			printf("%s ",ipList[temp2]);
			temp2++;
		}*/
		//printf("\n");
		//list
		insertFirst(domName, reqCount, ipList, "" );
	}
	struct node *p= find("www.GoogLe.com");//to search
	if(p==NULL){
		printf("ip address of google.com is: NULL \n");
	}else{ printf("ip address of google.com is: %s \n",p->ipList);}
	//printf("ip address of google.com is: %s \n",p->ipList);
	p= mostRequest();
	printf("The most is: %s %d \n",p->domName,p->reqCount);
	p= leastRequest();
	printf("The least is: %s %d \n",p->domName,p->reqCount);
	//delete(p); to delete
	printf("Tail is %s \n",tail->domName);
	//FILE *fpp= fopen("bstout.txt","a");
	//printbst(tail,fpp);
	//fclose(fpp);
	FILE *fp= fopen("list.txt","w+");
	printList(fp);//to print to file
	printf("Length of list: %d \n", length());

	exit(0);
}
