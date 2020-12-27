//////////////////////////////////////////////////////////////////////////
///                 Sequential Searching Program                       ///
//////////////////////////////////////////////////////////////////////////

/*searching_sequential.c
Description:
  The purpose of this program is to search sequentially/linearly for a series of patterns within a text file. The result of the sequential
  search is returned and the elapsed program time is calculated.
*/

//import all requried header files and dependancies
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

//initialise text and pattern variables and pointers
char *textData;
int textLength;

char *patternData;
int patternLength;

//initialise clock and time
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

/*readData(int patternNumber)
Description:
 read in text file and pattern file index according to patternNumber parameter
Parameters:
    patternNumber: index of pattern to read in from file
 Returns:
    0,1 (int) - 1 if success in reading in files, 0 otherwise
*/
int readData(int patternNumber)
{
    //initialise pointer to text file
    FILE *f;
        char fileName[1000];
    #ifdef DOS
            sprintf (fileName, "inputs\\text.txt");
    #else
        sprintf (fileName, "inputs/text.txt");
    #endif

        //open text file - if error opening then return 0
        f = fopen (fileName, "r");
        if (f == NULL)
            return 0;

        //iterate through text file and read contents
        readFromFile (f, &textData, &textLength);

        //close text file
        fclose (f);

    #ifdef DOS
            sprintf (fileName, "inputs\\pattern%d.txt", patternNumber);
    #else
        sprintf (fileName, "inputs/pattern%d.txt", patternNumber);
    #endif

        //open pattern file - if error opening then return 0
        f = fopen (fileName, "r");
        if (f == NULL)
            return 0;

        //iterate through pattern file and read contents
        readFromFile (f, &patternData, &patternLength);

        //close text file
        fclose (f);

        return 1;
}

/*hostMatch(long *comparisons)
 Description:
  iterate through text file and search for pattern, counting the number of comparisons and returning if the pattern was found or not
 Parameters:
     comparisons: count the number of comparisons made by the pattern search
  Returns:
     i,-1 (int): i if pattern found, -1 if not found
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
  process text and pattern files to try and find the pattern in the text
Parameters:
  None
Returns:
  void

*/
void processData()
{
    unsigned int result;
        long comparisons;

    printf ("Text length = %d\n", textLength);
    printf ("Pattern length = %d\n", patternLength);

    result = hostMatch(&comparisons);

    if (result == -1)
        printf ("Pattern not found\n");
    else
        printf ("Pattern found at position %d\n", result);
        printf ("Number of comparisons = %ld\n", comparisons);

}

//main calling function
int main(int argc, char **argv)
{
    int testNumber;

    //initialise testNumber to first pattern file number
    testNumber = 1;

    //iterate through all pattern files and search for them within the text file
    while (readData(testNumber))
    {
        c0 = clock(); t0 = time(NULL);
            processData();
        c1 = clock(); t1 = time(NULL);

        printf("Test %d elapsed wall clock time = %ld\n", testNumber, (long) (t1 - t0));
        printf("Test %d elapsed CPU time = %f\n\n", testNumber, (float) (c1 - c0)/CLOCKS_PER_SEC);
        testNumber++;
    }


}
