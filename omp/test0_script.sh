#!/bin/bash

#Script for executing all Test0 scripts

#------------------------------------
#     SLURM SPECIFIC COMMANDS
#------------------------------------
# Gives a name for the job
#SBATCH --job-name=test0

# Ask the scheduler for Y CPU cores on the same compute node
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4

# Set the name of the output file
#SBATCH -o test0.%N.%j.out

# Set the name of the error file
#SBATCH -e test0.%N.%j.err

#------------------------------------------
#              OPENMP SPECIFIC
#------------------------------------------

# Prints date
date

#setting openMP specific env variable to set the number of threads
export OMP_NUM_THREADS=2

# Compile sequential program
gcc searching_sequential.c -o searching_sequential

# Compiling parallel openMP program
gcc -fopenmp -std=gnu99 searching_OMP_0.c -o searching_OMP_0 -lm

echo "Running program using $OMP_NUM_THREADS threads"
echo "Starting new job"

#Execute sequential version of program, calculating execution time
time ./searching_sequential

#Execute the Parallel openMP implementation of program, calculating execution time
time ./searching_OMP_0

echo "Finished job"

# Prints date
date
#------------------------------------------
