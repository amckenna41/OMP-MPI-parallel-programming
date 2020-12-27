

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include  <omp.h>



////////////////////////////////////////////////////////////////////////////////
// searching_OMP_1 - Optimised Parallel Implementation using OpenMP
////////////////////////////////////////////////////////////////////////////////

/*searching_OMP_1.c
Description:
  The purpose of this program is to improve upon the searching_OMP_0.c program, improving its performance,
  speed and accuracy. This is achieved by optimising the pattern search algorithm using OpenMP constructs.
*/

//initialise pattern and text arrays
char *textData;
int textLength;

char *patternData;
int patternLength;

//initialise clock and time structures
clock_t c0, c1;
time_t t0, t1;

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

/*readData(int testNumber)
Description:
 read in all text and pattern files, store data and length in respective data and length arrays
Parameters:
    testNumber: index of pattern and text folder to load pattern and text from
 Returns:
    1/0: success or failure in reading in required data
 */
int readData (int testNumber)
{
    FILE *f;
    char fileName[1000];
#ifdef DOS
        sprintf (fileName, "inputs\\test%d\\text.txt", testNumber);
#else
    sprintf (fileName, "inputs/test%d/text.txt", testNumber);
#endif
    f = fopen (fileName, "r");
    if (f == NULL)
        return 0;
    readFromFile (f, &textData, &textLength);
    fclose (f);
#ifdef DOS
        sprintf (fileName, "inputs\\test%d\\pattern.txt", testNumber);
#else
    sprintf (fileName, "inputs/test%d/pattern.txt", testNumber);
#endif
    f = fopen (fileName, "r");
    if (f == NULL)
        return 0;
    readFromFile (f, &patternData, &patternLength);
    fclose (f);

    printf ("Read test number %d\n", testNumber);

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
    int i = 0, j = 0, k = 0, lastI = textLength-patternLength;
    long comparisons_tmp = 0;
    int found = textLength + 1;
    int patFound = -1;
    int num_t, t_id;

    printf("Number of threads execuing program: %d\n", omp_get_num_threads());
    printf("Maximum number of threads execuing program: %d\n ", omp_get_max_threads());
    printf("Number of cores: %d\n", omp_get_num_procs());

//    omp_set_num_threads(32); - Set number of OMP threads
//    omp_set_dynamic(0);     // Explicitly disable dynamic teams
//    omp_set_num_threads(64); // Use 4 threads for all consecutive parallel regions

    /*Serial sequential program transformed into parellel by adding pragma omp component
      found and comparisons_temp are shared among all threads, j and k are private to
      each thread and lastI, textData, patternData and patternLength are firstprivate for each thread
    */
    #pragma omp parallel shared(found, comparisons)           \
                       private(j, k)                         \
                       firstprivate(lastI, textData, patternData, patternLength)
	 {


        //for loop is parallelised , reduction counts the number of comparisons sperately in each thread and then summates at the end
        #pragma omp for reduction(+:comparisons_tmp) schedule(dynamic)

        //iterating through text until end is reached
        for(i = 0; i<= lastI; i++)
        {

            //if end of text has not been reached then search through text for pattern
            if(i < found)
             {
                k=i; j=0;

                //while pattern is in the text and pattern has not finished
                while(textData[k] == patternData[j] && j < patternLength)
                {
                     k++; j++;

                }
                //comparisons counter moved outside the while loop to reduce # of comparisons
                 comparisons_tmp++;

                 //if at end of pattern
                if(j == patternLength)
                {
                    patFound = j;      //patFound temp variable

                    {
                        if(found > i)
                            found = i;
                    }
                }
             }
            }
        }
        //race condition on comparions variable so it is set outside the paraellel loop
        *comparisons = comparisons_tmp;

    //if pattern found in text then then return its position otherwise -1
    if(found < textLength + 1)
             return found;
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
        long comparisons, product_length;


    printf ("Text length = %d\n", textLength);
    printf ("Pattern length = %d\n", patternLength);

    product_length = textLength * patternLength;
    printf("Product Length = %ld\n", product_length);

    result = hostMatch(&comparisons);
    if (result == -1)
        printf ("Pattern not found\n");
    else
        printf ("Pattern found at position %d\n", result);
        printf ("# comparisons = %ld\n", comparisons);

}

/*main(int argc, char **argv)
Description:
    Main calling function for program
*/
int main(int argc, char **argv)
{
    int testNumber = 0;

    //iterate through all test folders and the pattern and text files within them
    while (readData (testNumber))
    {
        c0 = clock(); t0 = time(NULL);
            processData();
        c1 = clock(); t1 = time(NULL);
                printf("Test %d elapsed wall clock time = %ld\n", testNumber, (long) (t1 - t0));
                printf("Test %d elapsed CPU time = %f\n\n", testNumber, (float) (c1 - c0)/CLOCKS_PER_SEC);
        testNumber++;
    }


}
