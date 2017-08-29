#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include "bst.c" //struct node defined here


struct node *head = NULL;
struct node *current = NULL;
struct node *tail= NULL;//used with the bst
//char ipList[30][16];
char ipList[200];
//display the list
void printList(FILE *fp) {
   struct node *ptr = head;
   
   //start from the beginning
   while(ptr != NULL) {
      fprintf(fp,"%s %d %s \n",ptr->domName,ptr->reqCount,ptr->ipList);
     /* unsigned int temp= 0;
      while(temp < (ptr->ipCount)){
    	  fprintf(fp,"%s ",ptr->ipList[temp]);
    	  temp++;
      }*/
      //fprintf(fp,"\n");
      ptr = ptr->next;
   }

}

/*void clearList(){

}*/

//insert link at the first location
void insertFirst(char *domName, unsigned int reqCount, char ipList[], time_t lastReqTime ) { //char *lastReqTime, int data
   //create a link
	//printf("i got here: %s ",ipList);
   struct node *link = (struct node*) malloc(sizeof(struct node));
   //printf("node memory created \n");
   strcpy(link->domName,domName);
   link->reqCount = reqCount;
   //link->ipCount = ipCount;
   //strcpy(link->lastReqTime,lastReqTime);
   link->lastReqTime = lastReqTime;
   strcpy(link->ipList,ipList);
   link->left = NULL;
   link->right = NULL;
   //link->data = data;
   /*for(int i=0; i< ipCount; i++){
	   strcpy(link->ipList[i],ipList[i]);
   }*/
   //point it to old first node
   link->next = head;
   if(head != NULL){
  	   head->prev = link;
   }
   else{
	   tail= link;

   }
   //point first to new first node
   head = link;
   //add node to bst
   printf("tail: %s \n",tail->domName);
   (void)createTree(tail,link);
}

//is list empty
bool isEmpty() {
   return head == NULL;
}

int length() {
   int length = 0;
   struct node *current;
	
   for(current = head; current != NULL; current = current->next) {
      length++;
   }
	
   return length;
}

//find a link with given key
struct node* find(char domName[]) {
	//if list is empty
   if(tail == NULL) {
      return NULL;
   }
   return findDomainNode(tail,domName);//using bst search

}

void delete(struct node* current){
	if(current->prev == NULL && current->next == NULL){ //only element
		free(current);
		head= NULL;
		tail= NULL;
	}
	else if (current->prev == NULL && current->next != NULL){ //first element
		current->next->prev = current->prev;
		head= current->next;
		free(current);
	}
	else {
		current->next->prev = current->prev;
		current->prev->next= current->next;
		if(tail==current){//if the node to delete is the tail
			tail= current->prev;
		}
		free(current);
	}
}

struct node* mostRequest() {

   //start from the first link
   struct node* current = head;
   unsigned int maxVal=0;
   struct node *maxNode;
   //if list is empty
   if(head == NULL) {
      return NULL;
   }
   maxVal= current->reqCount;
   maxNode= current;
   //navigate through list
   while(current->next != NULL) {

      if((current->next->reqCount) > maxVal) {
         maxVal= current->next->reqCount;
         maxNode= current->next;
      }
      current = current->next;
   }

   return maxNode;
}

struct node* leastRequest() {

   //start from the first link
   struct node* current = head;
   unsigned int minVal=0;
   struct node *minNode;
   //if list is empty
   if(head == NULL) {
      return NULL;
   }
   minVal= current->reqCount;
   minNode= current;
   //navigate through list
   while(current->next != NULL) {

      if((current->next->reqCount) < minVal) {
         minVal= current->next->reqCount;
         minNode= current->next;
      }
      current = current->next;
   }

   return minNode;
}
