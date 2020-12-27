#!/bin/bash

#Script for executing OMP Parallel Implemenation

#------------------------------------
#     SLURM SPECIFIC COMMANDS
#------------------------------------
# Gives a name for the job
#SBATCH --job-name=project_OMP

# Ask the scheduler for Y CPU cores on the same compute node
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=4

# Set the name of the output file
#SBATCH -o project_OMP.%N.%j.out

# Set the name of the error file
#SBATCH -e project_OMP.%N.%j.err


# Print out SLURM job environment variables
echo "Job ID: "$SLURM_JOB_ID
echo "Job Name: "$SLURM_JOB_NAME
echo "Node ID: "$SLURM_NODEID
echo "Node Name: "$SLURMD_NODENAME
echo "Number of Nodes: "$SLURM_JOB_NUM_NODES
echo "Number of tasks: "$SLURM_NTASKS
echo ""

#------------------------------------------
#              OPENMP SPECIFIC
#------------------------------------------

# Prints date
date

#setting openMP specific env variable to set the number of threads
export OMP_NUM_THREADS=4

#echo "Running program using $OMP_NUM_THREADS threads"
# Prints starting new job
echo "Starting new job"

#ensure execute_OMP script is executeable
#chmod +x execute_OMP

# Executes OMP program using folder called small-inputs or large-inputs.
#./execute_OMP small-inputs
./execute_OMP large-inputs

#sort OMP output from program and output to sorted_OMP file
echo "Sorting output"
sort -k 1,1n -k 2,2n -k 3,3n result_OMP.txt > sorted_OMP.txt

echo "Getting difference between output and desired output"
#diff command to display the differences of the sorted and unsorted files
#diff sorted_OMP.txt small-inputs_sorted.txt
#diff sorted_OMP.txt large-inputs_sorted.txt

# Prints finished job
echo "Finished job"

# Prints date
date
#------------------------------------------
