#Processing output of open MP
k=100
x = [2,4,6,8]
OMP_list = []
th1 = []
th2 = []
th3 = []
th4 = []
th5 = []
import csv
with open("data_OMP.txt", "r") as file:
	row = file.readlines()
 
for c in row:
  c = c.strip()
  split_lines = c.split(" ")
  if split_lines[1] == '1000':
    th1.append(float(split_lines[3])*10)
  elif split_lines[1] == '2000':
    th2.append(float(split_lines[3])*10)
  elif split_lines[1] == '3000':
    th3.append(float(split_lines[3])*10)
  elif split_lines[1] == '4000':
    th4.append(float(split_lines[3])*10)
  elif split_lines[1] == '5000':
    th5.append(float(split_lines[3])*10)
OMP_list.append([])
OMP_list.append(th1)
OMP_list.append(th2)
OMP_list.append(th3)
OMP_list.append(th4)
OMP_list.append(th5)


#Processing output of Pthread
x = [2,4,6,8]
Pthread_list = []
pth1 = []
pth2 = []
pth3 = []
pth4 = []
pth5 = []
import csv
with open("data_pthread.txt", "r") as file:
	row = file.readlines()
 
for c in row:
  c = c.strip()
  split_lines = c.split(" ")
  if split_lines[0] == '1000':
    pth1.append(float(split_lines[2])*10)
  elif split_lines[0] == '2000':
    pth2.append(float(split_lines[2])*10)
  elif split_lines[0] == '3000':
    pth3.append(float(split_lines[2])*10)
  elif split_lines[0] == '4000':
    pth4.append(float(split_lines[2])*10)
  elif split_lines[0] == '5000':
    pth5.append(float(split_lines[2])*10)
Pthread_list.append([])
Pthread_list.append(pth1)
Pthread_list.append(pth2)
Pthread_list.append(pth3)
Pthread_list.append(pth4)
Pthread_list.append(pth5)


#generating plots
import matplotlib.pyplot as plt
for i in range(1,6):
    loc = "Plots/N={}000.jpg".format(i)
    plt.xlabel("No of Threads")
    plt.ylabel("Execution time(sec) * 10")
    plt.title("N = {}000".format(i))
    plt.plot(x, OMP_list[i], color ="red")
    plt.plot(x, Pthread_list[i], color ="blue")
    plt.legend(["open MP", "Pthread"], loc ="upper right")
    plt.savefig(loc)
    plt.clf()
print("Plots Saved in /Plots folder!!")