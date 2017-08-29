#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
//#include "linkedlist.c"

struct node {
	char domName[72]; /*Max length of domain name label is 63 + 8+ 1*/
	unsigned int reqCount;
	//unsigned int ipCount;
	//char ipList[30][16];
	char ipList[200];
	//char lastReqTime[30];
	time_t lastReqTime;
	//int data;
	//char key[100];
	struct node *next;
	struct node *prev;
	struct node *left;
	struct node *right;
};


struct node * createTree(struct node *parent,struct node *newNode){
	
	if(parent == NULL){
		printf("tree: %s \n",newNode->domName);
		return newNode;

	}
	
	char parlwr[72], newlwr[72];
	memset(parlwr,0,sizeof(parlwr));
	memset(newlwr,0,sizeof(newlwr));
	for(int i = 0; parent->domName[i]; i++){ //convert string to lower
		parlwr[i] = tolower(parent->domName[i]);
	}
	for(int i = 0; newNode->domName[i]; i++){ //convert string to lower
		newlwr[i] = tolower(newNode->domName[i]);
	}
	printf("%s,%s \n",parlwr, newlwr);
	if(strcmp(newlwr,parlwr) < 0){//strcmp(newNode->domName,parent->domName) < 0
		parent->left= createTree(parent->left,newNode);
		printf("lower \n");
		//return parent;
	}
	else if(strcmp(newlwr,parlwr) > 0){//strcmp(newNode->domName,parent->domName) > 0
		parent->right= createTree(parent->right,newNode);
		printf("greater \n");
		//return parent;
	}
	else{}
	printf("tree: %s \n",parent->domName);
	return parent;
}

struct node * findDomainNode(struct node *parent,char domName[72]){
	if(parent == NULL){
		return NULL;
	}
	else{
		//convert to lowercase for comparison (for case insensitivity)
		char parlwr[72], domlwr[72];
		memset(parlwr,0,sizeof(parlwr));
		memset(domlwr,0,sizeof(domlwr));
		for(int i = 0; parent->domName[i]; i++){ //convert string to lower
			parlwr[i] = tolower(parent->domName[i]);
		}
		for(int i = 0; domName[i]; i++){ //convert string to lower
			domlwr[i] = tolower(domName[i]);
		}
		
		if(strcmp(parlwr,domlwr)==0){//strcmp(parent->domName,domName)==0
			return parent;
		}
		else if(strcmp(domlwr,parlwr) < 0){//strcmp(domName,parent->domName) < 0
			return findDomainNode(parent->left,domName);
		}
		else if(strcmp(domlwr,parlwr) > 0){//strcmp(domName,parent->domName) > 0
			return findDomainNode(parent->right,domName);
		}
		else{
			return NULL;
		}
	}
	//return parent;
}

void printbst(struct node *parent, FILE *fp){
	/*if(parent->left == NULL && parent->right == NULL){
		
		fprintf(fp,"%s %d %s \n",parent->domName,parent->reqCount,parent->ipList);
		return;
	}*/
	
	if(parent->left != NULL){
		printbst(parent->left,fp);
	}
	
	fprintf(fp,"%s %d %s \n",parent->domName,parent->reqCount,parent->ipList);
	
	if(parent->right != NULL){
		printbst(parent->right,fp);
	}
	//else if(parent->left == NULL && parent->right == NULL){
	
	
	
	//return;
	//}
	//else{}
	return;
}
