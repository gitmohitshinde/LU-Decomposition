#!/bin/bash

echo "Enter Your choice \n1.Serial LU decomposition \n2.LU Decomposition using open MP \n3.LU Decomposition using Pthread  " 
read -p "=>" input


if [ $input -eq 1 ]
then
     touch data_serial.txt
     > data_serial.txt
     for n in 1000 2000 3000 4000 5000
          do
          ./serial_output $n >> data_serial.txt
          echo "serial code with n = $n done!"
     done
fi

if [ $input -eq 2 ]
then
     touch data_OMP.txt
     > data_OMP.txt
     for threads in 2 4 6 8
     do
          for n in 1000 2000 3000 4000 5000
          do
          cs=$(($n/$threads))
          ./OMP_output $n $cs $threads 121 >> data_OMP.txt
          echo "OMP code with $threads threads, N = $n done!"
     done
     done
fi
#file data_OMP.txt contains rows in form <no of Threads> <dimension of matrix> <chunk size> <time taken>

if [ $input -eq 3 ]
then
     touch data_pthread.txt
     > data_pthread.txt
     for threads in 2 4 6 8
     do
          for n in 1000 2000 3000 4000 5000
          do
          ./Pthread_output $n $threads >> data_pthread.txt
          echo "Pthread code with $threads threads , N = $n done!"
     done
     done
fi
#file data_pthread.txt contains rows in form <dimension of matrix> <no of Threads> <time taken>

echo("Generating plots....")
python3 Generate_plot.py 
    