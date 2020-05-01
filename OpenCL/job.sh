#!/bin/env bash

#SBATCH --partition=shared-gpu-EL7
#SBATCH --time=00:20:00
#SBATCH --mail-user=michael.el-kharroubi@etu.hesge.ch
#SBATCH --mail-type=ALL
#SBATCH --gres=gpu:1
#SBATCH --output=AC-RES-ID:%J.out

srun AC parity 1000000 -