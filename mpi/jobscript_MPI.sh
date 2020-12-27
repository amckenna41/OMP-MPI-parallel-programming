#!/bin/bash

#------------------------------------
#     SLURM SPECIFIC COMMANDS
#------------------------------------
# Gives a name for the job
#SBATCH --job-name=project_MPI

# Ask the scheduler to run N MPI processes on N compute nodes
#SBATCH --nodes=4
#SBATCH --ntasks=8

# Set the name of the output file
#SBATCH -o project_MPI.%j.out

# Set the name of the error file
#SBATCH -e project_MPI.%j.err

# Print out SLURM job environment variables
echo "Job ID: "$SLURM_JOB_ID
echo "Job Name: "$SLURM_JOB_NAME
echo "Node ID: "$SLURM_NODEID
echo "Node Name: "$SLURMD_NODENAME
echo "Number of tasks: "$SLURM_NTASKS
echo "Number of Nodes: "$SLURM_JOB_NUM_NODES
echo ""
#------------------------------------------
#               MPI SPECIFIC
#------------------------------------------

# Load mpi module
module add mpi/openmpi

# Prints date
date

# Prints starting new job
echo "Starting new job"

#ensure execute_MPI script is executeable
chmod +x execute_MPI

#execute MPI program using execute_MPI script using large or small inputs folder
#./execute_MPI small-inputs
./execute_MPI large-inputs

#sort output from MPI program and store sorted output to sorted_MPI file
sort -k 1,1n -k 2,2n -k 3,3n result_MPI.txt > sorted_MPI.txt

#diff command to display the differences of the sorted and unsorted files
#diff sorted_MPI.txt small-inputs_sorted.txt
diff sorted_MPI.txt large-inputs-sorted.txt

# Prints finished job
echo "Finished job"

# Prints date
date

#------------------------------------------
