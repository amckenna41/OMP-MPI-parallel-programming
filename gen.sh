#!/bin/bash

# Script for running test case generation script

# Job name
#SBATCH --job-name=generate

# Ask the scheduler for Y CPU cores on the same compute node
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4

# Output file name
#SBATCH -o gen.%N.%j.out

# Error file name
#SBATCH -e gen.%N.%j.err

echo "Generating test cases"

gcc -o gen_inputs gen_inputs.c

rm -f inputs_omp

ln -s $1 inputs_omp

time ./gen_inputs

echo "Finished job"
