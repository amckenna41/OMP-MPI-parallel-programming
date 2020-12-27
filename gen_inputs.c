
//////////////////////////////////////////////////////////////////////////
///                 Generate Input Test cases                          ///
//////////////////////////////////////////////////////////////////////////

/*gen_inputs.c
Description:
  The purpose of this program is to generate text and pattern files used for the seqeunetial and parallel searching programs.
*/

//import all requried header files and dependancies
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <dirent.h>
#include <errno.h>
#define max_file_number 20
#define INPUTSDIR "inputs"

//initialise text and pattern variables and pointers
char *textData;
int textLength;

char *patternData;
int patternLength;

//initialise clock and time
clock_t c0, c1;
time_t t0, t1;

/* function for creating test diretories and pattern and text files */
void create_folders() {

    int i;
    for (i= 0; i <= max_file_number; i++)
    {
        char filename[100];
        char pattern_filename[100];
        char text_filename[100];

        sprintf(filename, "%s/test%d", INPUTSDIR, i);

        /* create directory pointer */
        DIR *dir = opendir(filename);

        /* checking if directory already exists, if not then create it */
        if (dir) {
            printf("Directory already exists\n");
            closedir(dir);

        } else {
            int makedir = mkdir(filename, 0777);
            if (!makedir)
                printf("Directory successfully created\n");
            else {
                printf("Unable to create directory\n");
                exit(1);
            }
        }

        //get full pattern and text filenames
        sprintf(pattern_filename, "%s/test%d/pattern.txt",INPUTSDIR, i);
        sprintf(text_filename, "%s/test%d/text.txt", INPUTSDIR, i);

        /* write to pattern and text files to initially create the files */
        FILE *fp, *ft;
        fp = fopen(pattern_filename, "w");
        ft = fopen(text_filename, "w");

        /* close both files */
        fclose (fp);
        fclose (ft);

        //close directory pointer
        close(dir);

    }
}

/* auxillary function for appending a char to char array*/
void append(char* s, char c) {

        int len = strlen(s);
        s[len] = c;
        s[len+1] = '\0';
}

/* function for creating pattern and text data */
int writeData(int testNumber, int* product_tot)
{
    /* initalise File pointer and pattern and text filenames */
    FILE *f, *fp;
    char pattern_fileName[100];
    char text_fileName[100];

    //generate filenames when pattern/text are in inputs file
    sprintf (pattern_fileName, "%s/test%d/pattern.txt", INPUTSDIR, testNumber);
    sprintf (text_fileName, "%s/test%d/text.txt",INPUTSDIR, testNumber);

    long pattern_max = 0; long text_max = 0;
    int j = 0; int k =0; int l = 0;
    int max_pattern[4] = {1, 10,100,1000};
    int max_text[4] = {1000, 10000, 100000, 1000000};

    /* while loop provides validation that the length of the pattern is less than the text */
    while (text_max <= pattern_max) {

        int rand_index = rand() % 4;
        text_max = max_text[rand_index];
        int rand_index_ = rand() % 4;
        pattern_max = max_text[rand_index_];

    }

    /* initialise pattern and text char arrays to the max size randomly chosen from the max_pattern and max_text arrays */
    char *pattern_char;
    pattern_char = (char *) realloc (pattern_char, sizeof(char)*pattern_max);

    char *text_char;
    text_char = (char *) realloc (text_char, sizeof(char)*text_max);

    printf("Max Pattern%ld\n", pattern_max);
    printf("Max text%ld\n", text_max);

    /* open pattern file for writing */
    f = fopen(pattern_fileName, "w");
    if (f == NULL){
        /*printf("Error");*/
        return 0;

    }

    /* write A's to pattern and end with 'B' */
    for (j = 0; j <=pattern_max; j++)
    {
        // int _rand_index = rand() % 26;
        append(pattern_char, 'A');


    }
    //append B to end of pattern
    append(pattern_char,'B');

    /* write pattern char array to pattern file */
    fprintf(f, "%s",pattern_char);

    /* close pattern file */
    fclose (f);

    /* open text file for writing */
    f = fopen(text_fileName, "w");
       if (f == NULL){
           printf("Error");
           return 0;

       }

    //text_max_max stores the length of the text without the pattern appended
    int text_max_max;
    text_max_max = text_max - pattern_max;

    //iterate through text file and append A's and 1 B
    for (k = 0; k <=text_max_max; k++)
       {
        // char randomletter = 'A' + (_rand_index_);
        append(text_char, 'A');

        }

        //append B to text char
        append(text_char,'B');

        //concatenate the pattern to the end of the text
        strcat(text_char, pattern_char);

    //print text char to file
    fprintf(f, "%s",text_char);

    //close file
    fclose(f);

    return 1;
}


int main(int argc, char** argv)
{

    //create required files and directories
    create_folders();
    int i;
    printf("Writing to pattern and text files\n");

    /* initalise File pointer and pattern and text filenames */
    FILE *f, *fp;
    char pattern_fileName[100];
    char text_fileName[100];

    //product = text size * pattern size (where both have to be a multiple of 10)
    int product_total[4] = {10, 1000, 10000, 1000000};

    //initialise all loop variables
    int m = 0; int k = 0; int j = 0; int l = 0; int n = 0;

    //loop through test cases 0 - 20 and writes to the pattern and text files
    for (i = 0; i <= 5; i++)

    {
            //inner for loop creates for instances for each product
            for (j = 0; j <=4; j++) {

                writeData(m, product_total[j]);

                m ++;
            }
    }

}
