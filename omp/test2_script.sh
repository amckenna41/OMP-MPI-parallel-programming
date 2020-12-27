#!/bin/bash

#Test2 Script

#------------------------------------
#     SLURM SPECIFIC COMMANDS
#------------------------------------
# Gives a name for the job
#SBATCH --job-name=test2

# Ask the scheduler for Y CPU cores on the same compute node
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4

# Set the name of the output file
#SBATCH -o test2.%N.%j.out

# Set the name of the error file
#SBATCH -e test2.%N.%j.err

#------------------------------------------
#              OPENMP SPECIFIC
#------------------------------------------

# Prints date
date

export OMP_NUM_THREADS=64

# Compile sequential program
gcc -o2 searching_sequential.c -o searching_sequential #sequential program with no compiler optimisation

# Compiling the parallel program
#gcc -fopenmp -std=gnu99 searching_OMP_O.c -o searching_OMP_O -lm #- parellel version of program using 2 threds
#gcc -fopenmp -std=gnu99 searching_OMP_1.c -o searching_OMP_1 -lm #- parelle$
gcc -fopenmp -std=gnu99 searching_OMP_2.c -o searching_OMP_2 -lm #- parellel version of program using 2 threds

#gcc -o2 searching_sequential.c -o searching_sequential #sequential program with no compiler optimisation

echo "Running program using ""$OMP_NUM_THREADS"" threads"
echo "Starting new job"

# Execute sequential version of program, calculating execution time
$time ./searching_sequential

# Executes the compiled parallel verision of program
#time ./searching_OMP_O
time ./searching_OMP_2

echo "Finished job"

# Prints date
date
#------------------------------------------
