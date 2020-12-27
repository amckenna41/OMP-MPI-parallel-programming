#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>


////////////////////////////////////////////////////////////////////////////////
// Parellel Searching Using MPI - Test 1 & 2
////////////////////////////////////////////////////////////////////////////////

/*searching_MPI_0.c
Description:
  The purpose of this program is to implement a basic and unoptimised search algorithm using the OpenMPI library for the parallel searching of patterns in text.
*/

//initialise text and pattern data char arrays and lengths
char *textData;
int textLength;

char *patternData;
int patternLength;

//initialise clock and time
clock_t c0, c1;
time_t t0, t1;


//execute if program runs out of memory
void outOfMemory()
{
    fprintf (stderr, "Out of memory\n");
    exit (0);
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
    //store read characters from file in data pointer & length of file in length pointer
    *data = result;
    *length = resultLength;
}

/*readText()
Description:
 read from text file
Parameters:
    None
Returns:
    void
 */
int readText ()
{
    //initialise file pointer for text file
    FILE *f;
    char fileName[1000];
#ifdef DOS
        sprintf (fileName, "inputs\\text.txt");
#else
    sprintf (fileName, "inputs/text.txt");
#endif

    //open text file - if error then return 0
    f = fopen (fileName, "r");
    if (f == NULL)
        return 0;

    //read contents of text file
    readFromFile (f, &textData, &textLength);

    //close file
    fclose (f);

    return 1;

}

/*readText()
Description:
 read from pattern file
Parameters:
    testNumber: number of pattern file
Returns:
    void
 */
int readPattern(int testNumber)
{
    //initialise file pointer for pattern file
    FILE *f;
    char fileName[1000];
#ifdef DOS
        sprintf (fileName, "inputs\\pattern%d.txt", testNumber);
#else
    sprintf (fileName, "inputs/pattern%d.txt", testNumber);
#endif

    //open pattern file - if error return 0
    f = fopen (fileName, "r");
    if (f == NULL)
        return 0;

    //read contents of pattern file
    readFromFile (f, &patternData, &patternLength);

    //close file
    fclose (f);

    return 1;
}

/*hostMatch(long *comparisons)
Description:
 iterate through text file and search for pattern, counting the number of comparisons and returning if the pattern was found or not
Parameters:
    comparisons: used to count the number of comparisons done by search algorithm
Returns:
    number of comparisons
 */
int hostMatch(long *comparisons)
{
    int i,j,k, lastI;

    i=0;
    j=0;
    k=0;
    lastI = textLength-patternLength;
        *comparisons=0;

    while (i<=lastI && j<patternLength)
    {
        (*comparisons)++;
        if (textData[k] == patternData[j])
        {
            k++;
            j++;
        }
        else
        {
            i++;
            k=i;
            j=0;
        }
    }
    if (j == patternLength)
        return i;
    else
        return -1;
}

/*processData()
Description:
 process text and pattern data to find pattern in text or not
Parameters:
    None
Returns:
    void
*/
void processData()
{
    unsigned int result;
        long comparisons;

    result = hostMatch(&comparisons);

    //print if pattern found or not in text
    if (result == -1)
        printf ("Pattern not found\n");
    else
        printf ("Pattern found at position %d\n", result);
        printf ("Number of comparisons = %ld\n", comparisons);

}

/*start_patttern_search(int patternNumber, int rank)
 Description:
      Called by each process that calls all other relevant functions to read and process the text and pattern data
 Parameters:
      patternNumber - Number for pattern file to read and search for in the text
      rank - the world_rank of the process calling the function
Returns:
    void
*/
void start_pattern_search(int patternNumber, int rank)
{
    //read pattern file according to patterNumber
    readPattern(patternNumber);

    //process pattern searching and measure time elapsed wall clock and CPU time
    c0 = clock(); t0 = time(NULL);
        processData();
    c1 = clock(); t1 = time(NULL);

    printf("Pattern %d run by process %d\n", patternNumber, rank);
    printf("Test %d elapsed wall clock time = %ld\n", patternNumber, (long) (t1 - t0));
    printf("Test %d elapsed CPU time = %f\n\n", patternNumber, (float) (c1 - c0)/CLOCKS_PER_SEC);
    printf ("-------------------------------------\n");
    printf ("-------------------------------------\n\n");


}
/*count_pattern_files()
 Description:
        Iterates through each file in inputs folder & counts number of pattern files.
        Allows for dynamic addition/removal of pattern files to inputs dir, if needed.
 Parameters:
        None
Returns:
        int pat_count - returns the number of pattern files in inputs folder
*/
int count_pattern_files() {

    int errnum;
    int pat_count = 0; //initialised to 0
    DIR * dir;      //open directory pointer
    struct dirent * entry;

    //open inputs dir
    dir = opendir("inputs");

    //error handling if directory doesn't exist or error opening
    if (dir == NULL) {
        fprintf(stderr, "Error opening inputs directory, ending program...%s\n", strerror(errnum));
        return 1;
    }

    //iterate through all files in dir, incrementing pattern file counter
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
             pat_count++;
        }
    }
    //decrementing count by 1 to account for 1 text file counted
    pat_count--;

    //close dir
    closedir(dir);

    return pat_count;
}

/*main(int argc, char **argv)
Description:
    Main calling function for program
*/
int main(int argc, char **argv)
{
    int testNumber, i;

    //counting the number of pattern files in inputs folder, initiaise to 0 before function so processes don't overwrite value
    int pattern_file_count = 0;
    pattern_file_count = count_pattern_files();

    //Initialise MPI environment
    MPI_Init(NULL, NULL);

    //get the rank (id) of the calling MPI process within specified communicator
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    //return the total number of MPI processes in specified communicator
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    //return processor name
    int myProcNameLength;
    char myProcName[MPI_MAX_PROCESSOR_NAME];
    MPI_Get_processor_name(myProcName, &myProcNameLength);

    //read text data
    readText();

    //If there are fewer than 2 processes, abort the execution.
    if (world_size < 2)
           {
            fprintf(stderr, "World size must be greater than 1. Aborting MPI environment. \n");
            MPI_Abort(MPI_COMM_WORLD, 1);
      }

    //following executed by process with rank 0 - 1st process
    //only executed by 1st process, otherwise all processes would execute it
    if(world_rank == 0)
    {
        printf("Executing using %d pattern files\n", pattern_file_count);
        printf("Pattern search using %d processes (World_Size)\n", world_size);
        printf ("-------------------------------------\n\n");

    }

    //following executed if number of processes is 2
    if(world_size == 2)
    {
        //executed by 1st process out of 2
        if (world_rank == 0)
        {
            //looping through all pattern files and only searching for odd numbered patterns
            for (i = 1; i<=pattern_file_count;i++)
            {
                if (i % 2 ==1)
                {
                    start_pattern_search(i, world_rank);

                }
            }
        } else {        //executed by 2nd process out of 2

            //looping through all pattern files and only searching for even numbered patterns
            for (i = 1; i<=pattern_file_count;i++)
                     {
                         if (i % 2 ==0)
                         {
                             start_pattern_search(i, world_rank);

                         }
                     }
                }
    }

    //following executed if number of processes is 4
    if(world_size == 4)
    {
        //executed by 1st process out of 4
        if (world_rank == 0)
        {
            start_pattern_search(1, world_rank);
            start_pattern_search(5, world_rank);
        }

       //executed by 2nd process out of 4
       if (world_rank == 1)
        {
            start_pattern_search(2, world_rank);
            start_pattern_search(6, world_rank);
        }

        //executed by 3rd process out of 4
        if (world_rank == 2)
        {
            start_pattern_search(3, world_rank);
            start_pattern_search(7, world_rank);
        }

       //executed by 4th process out of 4
       if (world_rank == 3)
        {
            start_pattern_search(4, world_rank);
            start_pattern_search(8, world_rank);
        }
    }

    //following executed if number of processes is 8
    if (world_size == 8)
    {
        start_pattern_search(world_rank+1, world_rank);

    }

    //remove textData from heap
    free(textData);

    //Terminate MPI execution environment
    MPI_Finalize();


}
