#!/bin/bash

#------------------------------------
#     SLURM SPECIFIC COMMANDS
#------------------------------------
# Gives a name for the job
#SBATCH --job-name=searching_MPI_1

# Ask the scheduler to run N MPI processes on N compute nodes
#SBATCH --nodes=4
#SBATCH --ntasks=8

# Set the name of the output file
#SBATCH -o searching_MPI_1.%j.out

# Set the name of the error file
#SBATCH -e searching_MPI_1.%j.err

# Print out SLURM job environment variables
echo "Job ID: "$SLURM_JOB_ID
echo "Job Name: "$SLURM_JOB_NAME
echo "Node ID: "$SLURM_NODEID
echo "Node Name: "$SLURMD_NODENAME
echo "Number of tasks: "$SLURM_NTASKS
echo "Number of	Nodes: "$SLURM_JOB_NUM_NODES
echo ""
#------------------------------------------
#               MPI SPECIFIC
#------------------------------------------

# Load mpi module
module add mpi/openmpi

# Prints date
date

# Compiling the Program
mpicc searching_MPI_1.c -o searching_MPI_1

# Prints starting new job
echo "Starting new job"

# Executes the compiled program on 2,4 and 8 MPI processes
$time mpirun -np 2 searching_MPI_1
$time #mpirun -np 4 searching_MPI_1
#$time mpirun -np 8 searching_MPI_1

# Prints finished job
echo "Finished job"

# Prints date
date


#------------------------------------------
