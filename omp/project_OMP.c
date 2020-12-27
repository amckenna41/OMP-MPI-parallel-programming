//import all required header files and libraries
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <omp.h>

/////////////////////////////////////////////////////////////////////////////////////////////////////
// Final Project OMP Parallel Programming Implementation
////////////////////////////////////////////////////////////////////////////////////////////////////

//initialise control variables to store values from control file and total number of control lines
int controlInput[10000][3];

//create file pointer for results_OMP output file
FILE *fOutput;

#define INPUTSDIR "inputs"

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

/*readFromControl(controlCount, controlIndex)
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
     //Default execution behaviour if only one control file found in inputs directory - control.txt file used
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

    /*
     Optimisation ----
        Input/Output using parallel OMP constructs is possible but was avoided in my program. It would be possible to read the text/pattern/control files in parallel using pragma omp ordered, to ensure the order of the files was maintained. The ordered construct can be expensive and has very similar functionality as just reading the files in serially but with the downside of the added OMP overhead required, thus it was avoided. It may prove useful when reading in files where the order of the contents are not important for the program.
     */

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
        fclose (f);     //close file
    }
}

/*readPatterns(char **patternData, int *patternLength, int totalPattern)

Description:
 read in all pattern files and store data and length in patternData and patternLength arrays
Parameters:
    **patternData: pointer to patternData array where pattern in file will be stored
    *patternLength: pointer to patternLength array where length of pattern file will be stored
    totalPattern: total number of pattern files in inputs folder
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
        sprintf (fileName, "%s\\pattern%d.txt",INPUTSDIR, i);
    #else
        sprintf (fileName, "%s/pattern%d.txt",INPUTSDIR, i);
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
void countFiles(int *patCount, int* textCount, int*contCount) {

    //char array used to store the sought file name pattern
    char *startsWith = NULL;

    //initialise file counters
    int pCount = 0;
    int tCount = 0;
    int cCount = 0;
    int errnum;

    DIR * dir;      //open directory pointer
    struct dirent * entry; //dirent structure contains the character pointer d_name, which points to a string that gives the name of a file in the directory.

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

/*printControlData(int numContLines)

Description:
 function for printing contents of the control file
Parameters:
     numContLines: number of lines in control file
 Returns:
    void

 */
void printControlData(int numContLines)
{
    printf("Printing Control File\n\n");
    int i;

    //unable to parallise using OMP in order to maintain order when printing control data
    for (i = 0; i <numContLines; i++)
    {
        printf("%d %d %d",controlInput[i][0], controlInput[i][1], controlInput[i][2]);
        printf("\n");
    }
}

/*
 findOccurenance(int textLength, char* textData, int patternLength, char* patternData)

 Description:
    Finds if pattern occurs or not
 Parameters:
      textLength: length of the text being searched
      textData: pointer to text data being searched
      patternLength: length of the pattern file to be used
      patternData: pointer to pattern data being searched
 Returns:
    -1 - if pattern not found
    -2 - if pattern found

 */
int findOccurence(int textLength, char* textData, int patternLength, char* patternData)

{

    int i,j,k, lastI;
    j=0;
    k=0;
    lastI = textLength - patternLength;

    //initilaise patFound variable to textLength + 1
    int patFound = textLength + 1;

    //patFound is shared among OMP threads
    //j & k are private to each thread
    //all other variables are firstprivate meaning each thread has its own instance of the variable & should be initialized with the value of the variable, because it exists before the parallel construct.
    //schedule - dynamic schedule proved most efficient - for both small and large inputs
    //num_threads - 4 threads was best for small inputs, 4 threads was best for large inputs
    #pragma omp parallel for default(none) shared(patFound) private(j, k) firstprivate(lastI, textData, patternData, patternLength) schedule(dynamic) num_threads(8)

    for (i = 0; i <= lastI; i++)
    {
        if (i < patFound)
        {
            k=i;
            j=0;

            while( textData[k] == patternData[j] && j < patternLength)
            {
                k++;
                j++;
            }

            if (j == patternLength)
            {

                    /*
                     Optimisation ----
                        Avoiding critical regions where possible.
                        Critical regions are expensive to execute as the sections of code executed by the regions cannot be parallelised and must be executed by only one thread. Although their use highlights the tradeoff in OMP between correctness and performance. Critical sections are not ideal and may have a negative impact on performance but without them there can be a race condition present, whereby two threads access the same variable or peice of memroy without synchronisation, leading to non-deterministic results. In this case, and for the findAllOccurences function, correctness took priority over performance.
                         Ultimately, critical sections were required but were avoided as much as possible in the program and it was ensured that the critical regions are not too long.
                     */

                //critical section to ensure code only executed by 1 thread at a time
                //errors can occuur if not done by 1 thread at a time, as patFound is a shared var
                #pragma omp critical
                {
                    if (patFound > i)
                        patFound = i;
                }
            }

        }
    }

    //return if pattern found or not
    if (patFound < textLength + 1)
        return -2;
    else
        return -1;
}


/*
 findAllOccurances(int textNumber, int patternNumber)

 Description:
    Finds all instances of pattern in text
 Parameters:
      testNumber: index of text file to be used
      patternNumber: index of pattern file to be used
      textLength: length of the text being searched
      textData: pointer to text data being searched
      patternLength: length of the pattern file to be used
      patternData: pointer to pattern data being searched
Returns:
    void

 */
void findAllOccurances(int testNumber, int patternNumber, int textLength, char* textData, int patternLength, char* patternData)
{
    int i,j,k, lastI;

    i = 0;
    j = 0;
    k = 0;

    //lastI set to stop loop when all text data has been searched
    lastI = textLength - patternLength;
    int patFound = 0;

    //patFound and fOutputs are shared among OMP threads
    //j & k are private to each thread
    //all other variables are firstprivate meaning each thread has its own instance of the variable & should be initialized with the value of the variable, because it exists before the parallel construct.
    //schedule - dynamic schedule proved most efficient - for both small and large inputs
    //num_threads - 4 threads was best for small inputs, 4 threads was best for larger inputs

     #pragma omp parallel for default(none) shared(patFound, fOutput) private(j, k) firstprivate(lastI, textData, patternData, patternLength, testNumber, patternNumber) schedule(dynamic) num_threads(8)
    for (i = 0; i <= lastI; i++)
    {
        k=i;
        j=0;

        while( textData[k] == patternData[j] && j < patternLength )
        {
            k++;
            j++;
        }

        //if pattern found
        if (j == patternLength)
        {
            //critical section to ensure code only executed by 1 thread at a time
            //errors can occuur if not done by 1 thread at a time, as patFound is a shared var
            //optimisations around critical section discussed in previous search function
            #pragma omp critical
            {
                //set patFound to one indicating pattern has been found
                patFound = 1;

                //if pattern found, write index to file indicating where pattern was found
                fprintf (fOutput, "%d %d %d\n",testNumber,patternNumber,i);

            }
        }

    }

    //if pattern not found, write -1 to file indicating pattern not found
    if (!patFound)
        fprintf (fOutput, "%d %d %d\n",testNumber,patternNumber,-1);

}


/*processData(int controlNumber,int textNumber,int patternNumber, int textLength, char *textData, int patternLength, char * patternData)

 Description:
    process text and pattern files to try and find the pattern in the text or every occurence of pattern, depending on control number
 Parameters:
    controlNumber: 0,1 - find whether pattern occurs or find index of every occurence of pattern
    textNumber: number of text file to search
    patternNumber: number of pattern file to search in text
    textLength: length of the text being searched
    textData: pointer to text data being searched
    patternLength: length of the pattern file to be used
    patternData: pointer to pattern data being searched
 Returns:
    void

 */
void processData(int controlNumber,int textNumber,int patternNumber, int textLength, char *textData, int patternLength, char * patternData)

{
    int result;

    if (patternLength > textLength) {

        fprintf (fOutput, "%d %d %d\n",textNumber,patternNumber,-1);
        return;
    }

    //if only searching for pattern occurence in the text data
    if (controlNumber == 0) {

        //call search function
        result = findOccurence(textLength, textData, patternLength, patternData);

        //write result, -2 if found, -1 if not.
        fprintf (fOutput, "%d %d %d\n",textNumber,patternNumber,result);
    }

    //if searching for every occurence of pattern in text data
    else {

        //call search function
        findAllOccurances(textNumber, patternNumber, textLength, textData, patternLength, patternData);

        }

    //else if controlNUm! = 0 or 1
    }

/*main(int argc, char **argv)

Description:
    Main calling function for program

*/
int main(int argc, char **argv)
{

    //initialise clock and time
    clock_t c0, c1;
    time_t t0, t1;

    int i=0;
    int total_threads = omp_get_thread_num(); //get total number of threads
    int max_threads = omp_get_max_threads();  //get max number of threads
    int total_proc = omp_get_num_procs();     //get number of processors
    int controlLines = 0;

    printf("Total Threads: %d :\n",total_proc);
    printf("Max Threads: %d :\n",max_threads);
    printf("Number of procesors: %d :\n",total_proc);

    //initialise file counters
    int patCount = 0;
    int textCount = 0;
    int contCount = 0;

    //count the number of pattern, text and control files in inputs directory
    countFiles(&patCount, &textCount, &contCount);

    //set text and pattern arrays to the size of the number of text and pattern files found by countFiles function
    char *textData[textCount];
    int textLength[textCount];

    char *patternData[patCount];
    int patternLength[patCount];

    //read all text in inputs directory and store text in textData array and length in textLength array
    readTexts(textData, textLength, textCount);
    //read all patterns in inputs directory and store pattern in patternData array and length in patternLength array
    readPatterns(patternData, patternLength, patCount);

    // removing previous OMP output file
    remove("result_OMP.txt");

    //create file pointer to output file and open in append mode
    fOutput = fopen("result_OMP.txt","a");

    int defaultControlIndex = 0; //set this variable to the index of the control file you want to use instead of the default 'control.txt', the readFromControl function will read in the index of the control file specified by this var. If setting this index, the function assumes there is more than one control file in the inputs dir. E.g set this var to 2 if you want to read in control2.txt.

    //read control file, store contents in controlInput array and count number of lines
    controlLines = readFromControl(contCount, defaultControlIndex);

    //if no lines found in control file - exit
    if (controlLines == 0)
    {
        printf("Control file not found or empty");
        return -1;
    }
    printf("%d lines found in control file.\n", controlLines);

    //initialising clock before search
    c0 = clock(); t0 = time(NULL);

    //iterate through control file
    for (i =0; i <controlLines; i++)
    {
        //set control/text/pattern number to value pointed by i in controlInput array
        int controlNum = controlInput[i][0];
        int textNum = controlInput[i][1];
        int patternNum = controlInput[i][2];

        //process text and pattern data according to controlInput array at current iteration i
        processData(controlNum,textNum,patternNum, textLength[textNum], textData[textNum], patternLength[patternNum], patternData[patternNum]);

    }
    //close output file
    fclose(fOutput);

    //stopping clock
    c1 = clock(); t1 = time(NULL);

    printf("Elapsed wall clock time = %ld\n", (long) (t1 - t0));
    printf("Elapsed CPU time = %f\n\n", (float) (c1 - c0)/CLOCKS_PER_SEC);

}
