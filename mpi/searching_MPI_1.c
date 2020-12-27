//importing all required libraries and modules
#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Parallel Searching using MPI - Test 3: Master Slave Model
////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Message tag sent from slave to master if slave process has found pattern
const int foundTag = 1;
//Master process is always rank 0, constant used for conveinence
const int master_proc = 0;

//create pointers to char arrays used to store text data
char *textData;
char *scatterTextData;
int textLength;
//create pointers to char arrays used to store text data
char *patternData;
int patternLength;

//scatter size holds the size of the chunks of the text array data
int scatterSize;
int found;

//used for reduction operation on total number of comparisons
long comparisonSum;

//initialise clock and time
clock_t c0, c1;
time_t t0, t1;

//function called if the program runs out of memory
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
    lastI = scatterSize-patternLength;
        *comparisons=0;

    while (i<=lastI && j<patternLength)
    {
        //possibly introduce some optimisation by checking if pattern if found and then breaking from loop

        (*comparisons)++;
        if (scatterTextData[k] == patternData[j])
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

    //if pattern found, send message to master process with foundTag to indicate it as found
    if (j == patternLength)
    {
        int patternSend = 1;

        MPI_Send(&patternSend, 1, MPI_INT, master_proc, foundTag, MPI_COMM_WORLD);

        return i;   //return index of found pattern

    //else pattern not found - return -1
    } else {

        return -1;
    }

}

/*processData(int rank)
  Description:
    process text and pattern data to find pattern in text or not
  Parameters:
      rank: rank of calling process
  Returns:
      void
*/
void processData(int rank)
{
    int result;
    long comparisons;   //count number of comparisons
    int patternIndex = 0;
    int patternFound;   //set if pattern found at index

    //search for pattern and return if found or not
    result = hostMatch(&comparisons);

    //index from result will be the index of the pattern found within processes' portion of text data so index needs to be adjusted depdnign on process rank and size of portioned text data (scatterSize)
    if (result != -1)
        patternIndex = result + (rank*scatterSize);

    //reduction operation on number of comparisons collected by all slave processes
    MPI_Reduce(&comparisons, &comparisonSum, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
    //reduction operation on index of pattern found on processes
    MPI_Reduce(&patternIndex, &patternFound, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);

    //return if pattern found or not & number of comparisons - run by master process 0
    if(rank == master_proc)
    {
        if (patternFound == -1)
        {
            printf ("Pattern not found\n");
        }
        else
        {
            printf ("Pattern found at index = %d\n", patternFound);
            printf ("Number of comparisons = %ld\n", comparisonSum);
            printf ("-------------------------------------\n");
            printf ("-------------------------------------\n\n");

        }
    }
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
    int testNumber, i, j, patternNumber = 1, numPatternFiles;

    //global flags used for asynchronous receive and broadcasts between master and slaves
    int flagPatternFound;
    int flagPattern;

    //Initialise the MPI environment.
    MPI_Init(NULL, NULL);

    //MPI Request used to know when non-blocking operation has completed
    MPI_Request req_found;
    MPI_Request req_bcast;

    //start clock
    c0 = clock(); t0 = time(NULL);

    //get the rank (id) of the calling MPI process within specified communicator
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    //return the total number of MPI processes in specified communicator
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    //Abort program if less than 2 processes as master-slave requries at least 2
    if (world_size < 2)
    {
            fprintf(stderr, "World size must be >=2. Aborting MPI environment. \n");
            MPI_Abort(MPI_COMM_WORLD, 1);
      }

    //Master process reads text file
    if(world_rank == master_proc)

    {
        //read text file
        readText();

        printf("\nSearching MPI using %d processes\n", world_size);
        printf("Total text length being searched = %d\n", textLength);
        printf ("---------------------------------\n\n");

        numPatternFiles = count_pattern_files();

        //Master broadcasts the size of the text file to the slaves
        MPI_Bcast(&textLength, 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
        else {

            //memory allocated for textData char for all processes
            MPI_Bcast(&textLength, 1, MPI_INT, 0, MPI_COMM_WORLD);
            textData = (char *)malloc(sizeof(char)*textLength);
        }

    //Calculate the size of the scattered arrays according to the text lengh and number of processes
    scatterSize = textLength/world_size;
    scatterTextData = (char *)malloc(sizeof(char)*scatterSize);

    //MPI Scatter scatters the text data in textData between the different proceses
    MPI_Scatter(textData, scatterSize, MPI_CHAR, scatterTextData, scatterSize, MPI_CHAR, 0, MPI_COMM_WORLD);

    //each process iterates through all 8 patterns
    for (i = patternNumber; i <=8; i++)
    {

            //initialise found pattern flag to 0 at start of loop iteration
            flagPatternFound = 0;

           //Master process reads in the next pattern file to be searched for.
           //Master prints out all pattern details otherwise all processes would print it
           if(world_rank == master_proc)
           {
               printf("Pattern number = %d\n", i);
               printf ("Pattern length = %d\n", patternLength);

               //master reads pattern number i
               readPattern(i);

               //broadcast length of pattern to slaves
               MPI_Bcast(&patternLength, 1, MPI_INT, master_proc, MPI_COMM_WORLD);
           }
           else
           {
               //slaves broadcast length to master
               MPI_Bcast(&patternLength, 1, MPI_INT, master_proc, MPI_COMM_WORLD);
               patternData = (char *)malloc(sizeof(char)*patternLength);

           }

            //master broadcasts patterndata to all slaves
           MPI_Bcast(patternData, patternLength, MPI_CHAR, master_proc, MPI_COMM_WORLD);


           //master process sets up asynchronous receive for all slaves
           if (world_rank == master_proc)
           {
               //aysnhcronous receive
               MPI_Irecv(&flagPattern, 1, MPI_INT, MPI_ANY_SOURCE, foundTag, MPI_COMM_WORLD, &req_found);
           }

           //slave processes set up aychronous broadcast to master
           else
           {
               //asychronous broadcast
               MPI_Ibcast(&flagPatternFound, 1, MPI_INT, master_proc, MPI_COMM_WORLD, &req_bcast);
           }

            //process text and pattern data
            processData(world_rank);

        }

    //master process prints out total comparisons and elapsed time of search
    if(world_rank == master_proc)
    {
        printf("Total Number of Comparisons: %ld\n", comparisonSum);

        //stop clock
        c1 = clock(); t1 = time(NULL);

        printf("Total Elapsed wall clock time using %d processes = %ld\n",world_size, (long) (t1 - t0));
        printf("Total Elapsed CPU time using %d processes = %f\n", world_size, (float) (c1 - c0)/CLOCKS_PER_SEC);
}
    //Terminate MPI execution environment
    MPI_Finalize();


}
