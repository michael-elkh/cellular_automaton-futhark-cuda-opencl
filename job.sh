#!/bin/env bash

#SBATCH --partition=shared-gpu-EL7
#SBATCH --time=00:40:00
#SBATCH --mail-user=michael.el-kharroubi@etu.hesge.ch
#SBATCH --mail-type=ALL
#SBATCH --gres=gpu:1
#SBATCH --output=benchmark-RES-ID:%J.out

srun ~/baobab_python_env/bin/python benchmark.py