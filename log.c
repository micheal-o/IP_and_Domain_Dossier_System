#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*Log all activities Function. Accepts two string arguments and appends to a file along with the current local time*/

void createLog(char *message, char status[])
{
    
    FILE *fp; //Create a file pointer
    fp = fopen("Logfile.txt", "a"); //Open logfile.txt in append mode
    
    if (fp == NULL) { //Exit with error if file cannot be opened
        fprintf(stderr, "Error! Cannot open the Logfile! \n");
        exit(1);
    }
    
    time_t t = time(NULL); //Initialzing the time_t datatype
    struct tm tm = *localtime(&t); //Initialize the struct with the localtime
    
    fprintf(fp, "%d/%d/%d %02d:%02d:%02d\t", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec); //Printing to the file
    
    fprintf(fp, "%s\t%s\n", message, status); //Print the message and status
    
    fclose(fp);//Close the file pointer
    
}
