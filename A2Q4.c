#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>
#include "boundedBuffer.h"
//-------------------------------------------------------------------------------------
// Course:      COMP3430
// Section:     A01
// Assignment:  2, question 4
// Name:        Eddie Torres
// UM-ID:       umtorre8
// Semester:    Winter, 2018
// Description- Using pthreads, I created a C program that implements
//              the key functionality of a multi-threaded print managager
//              this is ofcourse an implemen 
//              
//              
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
// CONSTANTS and TYPES
//-------------------------------------------------------------------------------------
#define MAX_PRINTREQUESTS 6
#define MAX_BUFFERSIZE 4
#define MIN_FILESIZE 500
#define MAX_FILESIZE 40000
#define checking(string, value){if (value) { printf("Failed with %ld @ %s", value, string); exit(1);} }



PrintRequest BoundedBuffer[MAX_BUFFERSIZE];
//-------------------------------------------------------------------------------------
// PROTOTYPES
//-------------------------------------------------------------------------------------
void * printClientCode (void * tid);
void * printServerCode (void * tid);
char * concatenateTwo  (const char *a, const char *b);
char * concatenateThree(const char *a, const char *b, const char *c);

//-------------------------------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------------------------------
int main(int argc, const char * const argv[]) 
{
    int  numPrintClients;
    int  numPrinters;
    long rc;

    assert(argc == 3);
    assert(argv != NULL);


    if(argc == 3 && argv != NULL)
    {
        numPrintClients = atoi(argv[1]);
        numPrinters     = atoi(argv[2]);
    }
    else
    {
        exit(1);
    }

    //playing the role of the OS code that is managing each printer
    pthread_t printServers[numPrinters];
    //applications requesting printing
    pthread_t printClients[numPrintClients];


    for(int i = 0; i < numPrinters; i ++)
    {
        rc = pthread_create(&printServers[i], NULL, printServerCode, NULL);
        checking("Failed at creating print server with value: ", rc);
    }
    for(int j = 0; j < numPrintClients; j++)
    {
        rc = pthread_create(&printClients[j], NULL, printClientCode, NULL);
        checking("Failed at creating print client with value: ", rc);
    }
    
    for(int k = 0; k < numPrinters; k++)
    {
		pthread_join(printServers[k],NULL);
    }
    for(int s = 0; s < numPrintClients; s++)
    {
		pthread_join(printClients[s],NULL);
    }
    pthread_exit(NULL);
    return EXIT_SUCCESS;
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
void * printClientCode(void * tid)
{
    char * string;
    char * string2;
    int    fileSize   = 0;
    long   pthreadId  = (long)pthread_self();
    char   buff  [sizeof(unsigned long)*8+1];
    char   buff2 [sizeof(unsigned long)*8+1];

    //long to char and storing in buff
    sprintf(buff, "%lu", pthreadId); 

    for(int i = 0; i < 6; i ++)
    {
        sprintf(buff2, "%lu", (long)i);

        //"random" size(in bytes) of the file between min and max
        fileSize = (rand() % (MIN_FILESIZE - MAX_FILESIZE)) + MAX_FILESIZE;

        string2  = concatenateTwo("-", buff2);
        string   = concatenateThree("File-", buff,  string2);

        printf("%s\n",string);

        if(bufferInsert(BoundedBuffer, string, fileSize) == 0)
        {
            printf("File size: %d\n", fileSize);
            printf("****INSERTING****\n%d: Hi, I am thread:'%ld'\n****END of INSERTING****\n", i, (long)pthread_self());


            sleep((rand() % (0 - 3)) + 3);
        }
        else
        {
            printf("Error, could not insert: '%s' into the buffer", string);
        }
        free(string);
        free(string2);
    }
    pthread_exit(NULL);
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
void * printServerCode(void * tid)
{
    char * result = NULL;
    while(true)
    {
        if(buffertDelete(BoundedBuffer, &result) == 0)
        {
            printf("****DELETED****\n: Hi, I am thread:'%ld'\nThis is the info from the printRequest %s\n****END of DELETED****\n",
             (long)pthread_self(), result);

        }
    }
    pthread_exit(NULL);
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
char * concatenateThree(const char * a, const char * b, const char * c) 
{
    size_t one      = strlen(a);
    size_t two      = strlen(b);
    size_t three    = strlen(c);
    char * result   = malloc(one + two + three + 1);

    memcpy(result, a, one);
    memcpy(result + one, b, two);
    memcpy(result + one + two, c, three + 1);
    return result;
}

char * concatenateTwo(const char * a, const char * b)
{
    size_t one      = strlen(a);
    size_t two      = strlen(b);
    char * result   = malloc(one + two + 1);

    memcpy(result, a, one);
    memcpy(result + one, b, two + 1);
    return result;
}