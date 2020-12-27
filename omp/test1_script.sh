#!/bin/bash

#Scipt for all C and OpenMP implementations for Part B Test1

#------------------------------------
#     SLURM SPECIFIC COMMANDS
#------------------------------------
# Gives a name for the job
#SBATCH --job-name=test1

# Ask the scheduler for 4 CPU cores on the same compute node
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4

# Set the name of the output file
#SBATCH -o test1.%N.%j.out

# Set the name of the error file
#SBATCH -e test1.%N.%j.err

#------------------------------------------
#              OPENMP SPECIFIC
#------------------------------------------

# Prints date
date

#OMP specific env variable for setting # of threads
export OMP_NUM_THREADS=2

# Compile sequential program
gcc searching_sequential.c -o searching_sequential

# Compiling the parallel program
#gcc -fopenmp -std=gnu99 searching_OMP_O.c -o searching_OMP_O -lm #- parellel version of program using 2 threds
gcc -fopenmp -std=gnu99 searching_OMP_1.c -o searching_OMP_1 -lm #- parellel version of program using 2 threds

echo "Running program using ""$OMP_NUM_THREADS"" threads"
echo "Starting new job"

# Execute sequential version of program, calculating execution time
$time ./searching_sequential

# Execute the compiled parallel program, calculating execution time
#time ./searching_OMP_O
time ./searching_OMP_1

echo "Finished job"

# Prints date
date
#------------------------------------------
