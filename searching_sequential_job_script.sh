#!/bin/bash

#------------------------------------
#     SLURM SPECIFIC COMMANDS
#------------------------------------
# Job name
#SBATCH --job-name=searching_sequential

# Ask the scheduler for Y CPU cores on the same compute node
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1

# Output file name
#SBATCH -o searching_sequential.%N.%j.out

# Error file name
#SBATCH -e searching_sequential.%N.%j.err

#------------------------------------------
#            Searching Sequential
#------------------------------------------

# Prints date
date

# Compiling the Program
gcc searching_sequential.c -o searching_sequential #no compiler optimisation, default -O0
#gcc searching_sequential.c -O1 -o searching_sequential #-O1 compiler optimisation
#gcc searching_sequential.c -O2 -o searching_sequential #-O2 compiler optimisation

# Prints starting new job
echo "Starting new job"

# Executes the compiled program
#./searching_sequential
time ./searching_sequential

# Prints finished job
echo "Finished job"

# Prints date
date
