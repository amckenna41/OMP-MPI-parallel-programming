//import all required header files and libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <mpi.h>


/////////////////////////////////////////////////////////////////////////////////////////////////////
// Final Project MPI Parallel Programming Implementation
////////////////////////////////////////////////////////////////////////////////////////////////////

/*project_MPI.c
Description:
  The purpose of this program is to search for certain patterns within text using the OpenMPI library for parallel searching.
*/

//initialise control variables to store values from control file and total number of control lines
int controlInput[10000][3];

//create file pointer for results_MPI output file
FILE *fOutput;

//initialise request handles for non-blocking asynchronous receive and broadcast operations
MPI_Request patternFoundReq;
MPI_Request patternFoundBroadcast;

#define MASTER 0
#define INPUTSDIR "inputs"
#define processFoundTag 20      //arbitary value for found tag


/*outOfMemory()
Description:
    execute if the program runs out of memory
Parameters:
    None
Returns:
    void
 */
void outOfMemory()
{
    fprintf (stderr, "Out of memory\n");
    exit (0);
}


/*readFromControl(controlCount,  controlIndex)
Description:
 read from control file, iterate through and store each line of file into 3 dimensional int controlInput array. This function also allows for the user to set what index of control file to open, in the case that there may be multiple control files in the inputs directory. The default behaviour for the function is to read in the control file called control.txt. If the number of control files counted by the countFiles function is greater than 1 then the controlIndex parameter is used as the index of the control file to open, the default for this parameter is 0 meaning the control.txt file is opened and read. This function is useful if one wants to test the program on different control files.
Parameters:
    controlCount: number of control files counted in inputs directory, value obtained from countFiles function.
    controlIndex: index of the control file to open and read, default is 0 meaning control.txt file is read by function.
Returns:
    totalLines: total number of lines found in control file
 */
int readFromControl(int controlCount, int controlIndex)
{

        //create file pointer
        FILE *f;
        char fileName[100];
        int totalLines = -1;

    //the following executes if more than one control file found in inputs directory according to countFiles function AND if the index of the control file to use is not at its default value of 0 for the control.txt file
     if (controlCount > 1 && controlIndex !=0) {
#ifdef DOS
        sprintf (fileName, "%s\\control%d.txt", INPUTSDIR, controlIndex);
#else
        sprintf (fileName, "%s/control%d.txt", INPUTSDIR, controlIndex);
#endif
     }
     //Default execution behaviour if only one control file found in inputs directory - control.txt file used //
     else {

#ifdef DOS
        sprintf (fileName, "%s\\control.txt", INPUTSDIR);
#else
        sprintf (fileName, "%s/control.txt", INPUTSDIR);
#endif
     }
        //open control file
        f = fopen (fileName, "r");
        if (f == NULL)
                return(0);

        //iterate through lines in control file, store into controlInput array and count number of lines
        //1st Dimension - 0 = find whether pattern occurs, 1 = find every pattern occurrence
        //2nd Dimension - Which text file to use
        //3rd Dimension - Which pattern file to use
        while(!feof(f)){
            totalLines ++; //incrementing line count at start of loop so fscanf doesn't read a line after all have been read in leading to 0 0 0 being stored in array
            fscanf(f,"%d %d %d",&controlInput[totalLines][0],&controlInput[totalLines][1],&controlInput[totalLines][2]);
        }

    //return total number of lines found in control file
    return totalLines;
}

/*readFromFile(File *f, char **data, int *length)
Description:
 iterate through and read contents of file
Parameters:
    *f: pointer to file being read
    **data: pointer to data that is read from file
    *length: length of data being read
Returns:
    void
 */
void readFromFile (FILE *f, char **data, int *length)
{
    int ch;
    int allocatedLength;
    char *result;
    int resultLength = 0;

    allocatedLength = 0;
    result = NULL;

    ch = fgetc (f);
    while (ch >= 0)
    {
        resultLength++;
        if (resultLength > allocatedLength)
        {
            allocatedLength += 10000;
            result = (char *) realloc (result, sizeof(char)*allocatedLength);
            if (result == NULL)
                outOfMemory();
        }
        result[resultLength-1] = ch;
        ch = fgetc(f);
    }
    *data = result;
    *length = resultLength;
}


/*readTexts(char **textData, int *textLength, int totalText)
Description:
 read in all text files and store data and length in textData and textLength arrays
Parameters:
    **textData: pointer to textData array where text in file will be stored
    *textLength: pointer to textLength array where length of text file will be stored
    totalText: total number of text files in inputs folder
 Returns:
    void
 */
void readTexts(char **textData, int *textLength, int totalText)
{
    int i;

    //iterate through all text files in inputs folder and store text and length in arrays
    for (i = 0; i < totalText; i++)
    {
        //create file pointer for text file
        FILE *f;
        char fileName[1000];
    #ifdef DOS
        sprintf (fileName, "%s\\text%d.txt", INPUTSDIR, i);
    #else
        sprintf (fileName, "%s/text%d.txt", INPUTSDIR, i);
    #endif
        f = fopen(fileName, "r");
        if (f == NULL)
                exit (0);

        //read contents of file storing contents in textData array at current iteration i
        readFromFile (f, &textData[i], &textLength[i]);
        fclose (f); //close file
    }
}


/*readPatterns(char **patternData, int *patternLength, int patCount)
Description:
 read in all pattern files and store data and length in patternData and patternLength arrays
Parameters:
    **patternData: pointer to patternData array where pattern in file will be stored
    *patternLength: pointer to patternLength array where length of pattern file will be stored
    patCount: total number of pattern files in inputs folder
 Returns:
    void
*/
void readPatterns(char **patternData, int *patternLength, int totalPattern)
{
    int i;

    //iterate through all pattern files in inputs folder and store pattern and length in arrays
    for (i = 0; i < totalPattern; i++)
    {
        //create file pointer for pattern file
        FILE *f;
        char fileName[1000];

    #ifdef DOS
        sprintf (fileName, "%s\\pattern%d.txt", INPUTSDIR, i);
    #else
        sprintf (fileName, "%s/pattern%d.txt", INPUTSDIR, i);
    #endif
        f = fopen (fileName, "r");
        if (f == NULL)
            exit (0);

        //read contents of file storing contents in patternData array at current iteration i
        readFromFile (f, &patternData[i], &patternLength[i]);
        fclose (f);     //close file
    }
}

/*countFiles()
Description:
 Counts the number of pattern, text and control files in inputs directory - used for setting the size of the text and pattern arrays to make them no bigger than the max number of pattern/text files in directory - Optimisation.
Parameters:
    *patCount: pointer to variable used to count the number of pattern files
    *textCount: pointer to variable used to count the number of text files
    *contCount: pointer to variable used to count the number of control files
    None
 Returns:
    void
*/
void countFiles(int *patCount, int* textCount, int*contCount)
{

    //char array used to store the sought file name pattern
    char *startsWith = NULL;

    //initialise file counters
    int pCount = 0;
    int tCount = 0;
    int cCount = 0;
    int errnum;

    DIR * dir;      //open directory pointer
    struct dirent * entry;  //dirent structure contains the character pointer d_name, which points to a string that gives the name of a file in the directory.

    //open inputs dir
    dir = opendir(INPUTSDIR);

    //error handling if directory doesn't exist or error opening it
    if (dir == NULL) {
        fprintf(stderr, "Error opening inputs directory, ending program...%s\n", strerror(errnum));
        exit(0);
    }

    //iterate through all files in dir, incrementing file counters if respective file found
    //d_name is set to the name of the current file read by readdir(dir) operation, if the d_name starts with 'text'/'pattern'/'control' then the respective counters are incremented
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            startsWith = strstr(entry->d_name, "pattern");
            if (startsWith)
            {
                pCount++;            //increment pattern file counter

            }
            startsWith = strstr(entry->d_name, "text");
             if (startsWith)
             {
                tCount++;           //increment text file counter

             }

            startsWith = strstr(entry->d_name, "control");
            if (startsWith)
            {
                cCount++;           //increment control file counter

            }
        }
    }

    //close dir
    closedir(dir);

    //set counter pointers equal to local counters
    *patCount = pCount;
    *textCount = tCount;
    *contCount = cCount;


}

/*printControlInput(int numContLines)
Description:
 function for printing contents of the control file
Parameters:
     numContLines: number of lines in control file
 Returns:
    void
*/
void printControlInput(int numContLines)
{
    printf("Printing Control File\n\n");
    int i;

    //iterate through contents of control file and print each line as found in controlInput array
    for (i = 0; i <numContLines; i++)
    {
        printf("%d %d %d",controlInput[i][0], controlInput[i][1], controlInput[i][2]);
        printf("\n");
    }
}

/*
findOccurence(int textLength, int patternLength, char* patternData,  char* textData, int rank)
Description:
   Finds if pattern occurs or not
Parameters:
     textLength: length of the text being searched
     patternLength: length of the pattern file to be used
     patternData: pointer to pattern data being searched
     textData: pointer to text data being searched
     rank: rank of calling process
Returns:
   result: -1 if not found, 1 if found
*/
int findOccurence(int textLength, int patternLength, char * patternData, char* textData, int rank) {

        int i,j,k, lastI;
        int patFound = -1;
        int patFoundTest = 0;
        int patternCheckInterval = 0;

        //initialise variables used in search function
        i=0;
        j=0;
        k=0;
        lastI = textLength-patternLength;

        //set patternCheckInterval which determines the interval at which the pattern found verification is done - for larger inputs it is done every 1000'th iteration, for small inputs it is done around every 100'th iteration.
        if (textLength >= 100000){
            patternCheckInterval = 1000;
        } else {
            patternCheckInterval = 100;
        }

        //iterate through text data while lastI has not been reached
        while (i<=lastI)
        {

           //below code used to check if pattern found, if so the search is stopped for all processes. It is unnessary and expensive to carry out this check each iteration, especially for large inputs and large values of i.
           if(i % patternCheckInterval == 0) {

               //executed by master process
               if (rank == MASTER)
               {
                   //patFoundTest initialised to 0 to indicate pattern not found by default
                   patFoundTest = 0;

                   //test if asynchronous receive initialised in main func has received anything from any source process that the pattern has been found; sent from MPI_Send command when pattern found. patternFoundReq request is the request handle on the non-blocking routine. Set patFoundTest to value returned from MPI_Test.
                   MPI_Test(&patternFoundReq, &patFoundTest, MPI_STATUS_IGNORE);

                   //if a slave process has indicated to master that pattern has been found then asynchronous broadcast communicates to slaves to stop searching for pattern, uses broadcast request handle.
                   if (patFoundTest != 0)
                   {

                       MPI_Ibcast(&patFoundTest, 1, MPI_INT, MASTER, MPI_COMM_WORLD, &patternFoundBroadcast);

                       //MPI_wait waits for the non-blocking operation pointed at by request handle to complete. This ensures that all processes receive the broadcast message that the pattern has been found
                       MPI_Wait(&patternFoundBroadcast, MPI_STATUS_IGNORE);

                       //break from loop
                       break;

                   }
               }

               //executed by slave processes
               else
               {
                   //patFoundTest initialised to default 0 to indicate pattern not found
                   patFoundTest = 0;

                   //test if aynchronous broadcast initialised in main func has been sent from master, sent using MPI_Ibcast with the patternFoundBroadcast request handle. Set patFoundTest to value returned from MPI_Test.
                   MPI_Test(&patternFoundBroadcast, &patFoundTest, MPI_STATUS_IGNORE);

                   //if a broadcast message has been received from the master process indicating the pattern has been found then stop searching and break from current iteration.
                   if (patFoundTest != 0)
                   {
                       break;

                   }

               }
       }

            //if text and pattern at current iteration k & j, respectively, match then increment counters
            if (textData[k] == patternData[j])
            {
                k++;
                j++;
            }

            //if text and pattern at current iteration isn't a match, increment i, set k to value of i and j to 0
            else
            {
                i++;
                k=i;
                j=0;
            }

            //if j == length of the pattern then match found
            if (j == patternLength){

                patFound = 1;

                //send patFound variable to Master with the found tag to indicate to master that patttern has been found, the master can then inform the rest of the processes to stop searching
                MPI_Send(&patFound, 1, MPI_INT, MASTER, processFoundTag, MPI_COMM_WORLD);
            }

    }
        return patFound;

    }


/*
findAllOccurences(int textNumber,int patternNumber, int textLength, int patternLength, char * patternData, char* textData)
Description:
    Finds all instances of pattern in text
Parameters:
      textNumber: index of text file to be used
      patternNumber: index of pattern file to be used
      textLength: length of the text being searched
      patternLength: length of the pattern file to be used
      patternData: pointer to pattern data being searched
      textData: pointer to text data being searched
Returns:
    result: -1 if not found or index of found pattern if it is found
*/
int findAllOccurences(int textNumber,int patternNumber, int textLength, int patternLength, char * patternData, char* textData)
        {
            int i,j,k, lastI;
            int patFound = -1;

            //initialise variables used in search function
            i=0;
            j=0;
            k=0;
            lastI = textLength-patternLength;

          //iterate through text data while lastI has not been reached
            while (i<=lastI)
            {

                //if text and pattern at current iteration k & j, respectively, match then increment counters
                if (textData[k] == patternData[j])
                {
                    k++;
                    j++;
                }

               //if text and pattern at current iteration isn't a match, increment i, set k to value of i and j to 0
                else
                {
                    i++;
                    k=i;
                    j=0;
                }

                //if j == length of the pattern then match found
                if (j == patternLength){

                    patFound = i;       //set patFound = current index where pattern has been found

                    //print value of i to file indicating pattern found
                    fprintf (fOutput, "%d %d %d\n",textNumber,patternNumber,i);

                }

            }

            return patFound;

        }

/*processData(int controlNumber,int textNumber,int patternNumber, int textLength, char *textData, int patternLength, char * patternData, int rank)
Description:
 process text and pattern files to try and find the pattern in the text or every occurence of pattern, depending on control number
Parameters:
    controlNumber: 0,1 - find whether pattern occurs or find every occurence of pattern
    textNumber: number of text file to search
    patternNumber: number of pattern file to search in text
    textLength: length of the text being searched
    textData: pointer to text data being searched
    patternLength: length of the pattern file to be used
    patternData: pointer to pattern data being searched
    rank: rank of current process
Returns:
    void
*/
void processData(int controlNumber,int textNumber,int patternNumber, int textLength, char *textData, int patternLength, char * patternData, int rank)

{
    //initialise local vars
    int result = 0;
    int resultAll = 0;
    int indexFound = 0;
    int indexFoundAll = 0;

    //searching only that pattern occurs in the text
    if (controlNumber == 0) {

        //get result obtained from findOccurence function that only searches if pattern occurs in text
        result = findOccurence(textLength, patternLength, patternData, textData, rank);

        //reduction operation to get the minimum value returned from the findOccurence function from all the processes.
        MPI_Reduce(&result, &indexFound, 1, MPI_INT, MPI_MIN, MASTER, MPI_COMM_WORLD);

    if (rank == MASTER) {
        if (indexFound==1) {

            //print -2 to file, indicating pattern was found
            fprintf (fOutput, "%d %d %d\n",textNumber,patternNumber,-2);
            }

        else {

            //print -1 to file, indicating pattern was not found
            fprintf (fOutput, "%d %d %d\n",textNumber,patternNumber,-1);

            }
        }
    }

    //searching for all occurences of pattern in the text
    else {

        //get result obtained from findAllOccurences function that searches for all instances of the pattern in the text
        result = findAllOccurences(textNumber, patternNumber, textLength, patternLength, patternData, textData);

        //reduction operation to get the minimum value returned from the findAllOccurences function from all the processes.
        MPI_Reduce(&result, &indexFoundAll, 1, MPI_INT, MPI_MIN, MASTER, MPI_COMM_WORLD);

        //master writes to file if pattern not found
        if (rank == MASTER) {

            //if pattern not found
            if (indexFoundAll==-1) {

                //print -1 to file, indicating pattern was not found
                fprintf (fOutput, "%d %d %d\n",textNumber,patternNumber,-1);
            }


            }
        }
    }


/*main(int argc, char **argv)
Description:
    Main calling function for program
*/
int main(int argc, char **argv)
{
    int i;
    int world_rank;
    int world_size;
    int controlLines;

    //initialise clock and time
    clock_t c0, c1;
    time_t t0, t1;

    //initialise MPI environment
    if(MPI_Init(NULL, NULL)!= MPI_SUCCESS) {
        fprintf(stderr, "Error creating MPI Environment %s\n", argv[0]);

    }

    //return the total number of MPI processes in specified communicator
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    //get the rank (id) of the calling MPI process within specified communicator
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    //terminate MPI environment if less than 2 MPI processes called, master-slave model requries at least 2 processes to function
    if (world_size < 2)
    {
            fprintf(stderr, "Master Slave process requires at least 2 processes %s\n", argv[0]);
            MPI_Abort(MPI_COMM_WORLD, 1);
    }

    //initialise file counters
    int patCount = 0;
    int textCount = 0;
    int contCount = 0;

//    master process counts number of files in inputs directory, removes old MPI output file, reads the control data from the control file and intialises the clock. Master was also used to execute printControlInput function which prints out contents of control file read - used for verification that readFromControl function read control file correctly.
     if (world_rank == 0) {

         int defaultControlIndex = 0; //set this variable to the index of the control file you want to use instead of the default 'control.txt', the readFromControl function will read in the index of the control file specified by this var. If setting this index, the function assumes there is more than one control file in the inputs dir. E.g set this var to 2 if you want to read in control2.txt.
         countFiles(&patCount, &textCount, &contCount);
         remove("result_MPI.txt");
         controlLines = readFromControl(contCount, defaultControlIndex);
         //if no lines found in control file - exit
          if (controlLines == 0)
          {
              printf("Control file not found or empty\n");
              return -1;
          }

         c0 = clock(); t0 = time(NULL);
         //printControlInput(controlLines); // print control file contents - used to check if control file was read in properly
     }

    //Broadcast file counters and the number of lines found in the control file to every process.
    MPI_Bcast(&patCount, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&textCount, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&contCount, 1, MPI_INT, MASTER, MPI_COMM_WORLD);
    MPI_Bcast(&controlLines, 1, MPI_INT, MASTER, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    // Read in the text pattern and control for each thread (aim of keeping bcasts etc to as few as possible)
    char *textData[textCount];
    int textLength[textCount];

    char *patternData[patCount];
    int patternLength[patCount];

    //create file pointer to output file and open in append mode
    fOutput = fopen("result_MPI.txt","a");

    //each process reads in all the text files and stores contents in textData array
    readTexts(textData, textLength, textCount);
    //each process reads in all the pattern files and stores contents in patternData array
    readPatterns(patternData, patternLength, patCount);

    //Barrier used to ensure each process has read in all the pattern and text files before proceeding with the search process. Synchronises processes.
    MPI_Barrier(MPI_COMM_WORLD);

    //iterate over all lines in the control file
        for (i = 0; i < controlLines; i++)
          {

          //get values for control number, text file to use and pattern file to use from the controlInput array at the current iteration i
          int controlNum = controlInput[i][0];
          int textNum = controlInput[i][1];
          int patternNum = controlInput[i][2];

              if (textLength[textNum] < patternLength[patternNum]) {
                    if (world_rank == 0)
                    {
                        //write -1 to file indicating pattern not found
                        fprintf(fOutput, "%d %d %d\n", textNum, patternNum, -1);
                        continue;

                    }
                    else {
                        continue;
                    }
                            }

          //initialise request handles used for non-blocking operations to NULL request
          patternFoundReq = MPI_REQUEST_NULL;
          patternFoundBroadcast = MPI_REQUEST_NULL;

           //create integer flags used with the non-blocking operations to indicate pattern found
           int patFoundFlag = 0;
           int patFlag;

           //master process creates asyncrhonous receive from any of the slave processes which indicate - using the pattern found tag - that pattern has been found. MPI_ANY_SOURCE means that any process can send to master and master can receive from any process. patternFoundReq is request handle for Irecv operation
           if (world_rank == 0)
           {
               MPI_Irecv(&patFlag, 1, MPI_INT, MPI_ANY_SOURCE, processFoundTag, MPI_COMM_WORLD, &patternFoundReq);
           }

           //non-blocking broadcast that broadcasts a message from master to all other processes in communicator. patternFoundBroadcast is request handle for Ibcast operation.
           else
           {
               MPI_Ibcast(&patFoundFlag, 1, MPI_INT, MASTER, MPI_COMM_WORLD, &patternFoundBroadcast);
           }

     //process text and pattern data according to controlInput array at current iteration i
     processData(controlNum, textNum, patternNum, textLength[textNum], textData[textNum], patternLength[patternNum], patternData[patternNum], world_rank);

        }

    //only master used to time the program
     if (world_rank == MASTER) {

          //stopping clock
           c1 = clock(); t1 = time(NULL);

           printf("Elapsed wall clock time = %ld\n", (long) (t1 - t0));
           printf("Elapsed CPU time = %f\n\n", (float) (c1 - c0)/CLOCKS_PER_SEC);
         }

    //terminate MPI environment
    MPI_Finalize();

    //close output file
    fclose(fOutput);
}
