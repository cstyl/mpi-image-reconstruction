import numpy as np
import matplotlib.pyplot as plt
import argparse

parser = argparse.ArgumentParser(description='Read number of repetitions and working directory')
parser.add_argument('-r', dest='reps', type=int, help='Enter the number of the repetitions performed')

args = parser.parse_args()
plotdir = 'res/performance/plot_mulnodes/'
reps = args.reps

maindir = 'res/performance/'
subdir = ['overlap_mulnodes1/', 'overlap_mulnodes2/','overlap_mulnodes3/']
filename = ['overlap_mulnodes1_perf.csv', 'overlap_mulnodes2_perf.csv','overlap_mulnodes3_perf.csv']

img=['192x128', '256x192', '512x384', '768x768']
processes=[6, 12, 18, 24, 30, 36]
nodes = 3
size=[192*128, 256*192, 512*384, 768*768]
avg_time = np.zeros((len(img),nodes,len(processes)))


for i,fin in enumerate(filename):
	file = maindir + subdir[i] + fin
	data = np.genfromtxt(file, delimiter=',', skip_header=1)

	for num, label in enumerate(img):
		# read img type column
		blocks = data[:,0]
		# get data that only match the current img
		img_data = data[blocks==num,:]

		for j,proc in enumerate(processes):
			# get data that only match the current process
			blocks = img_data[:,1]
			proc_data = img_data[blocks==proc,:]
			# take the average time for each version
			avg_time[num,i,j] = np.mean(proc_data[:,4])

# get the reference implementation for 1 process where overlap communications are used
refdir = 'res/performance/overlap_perf1/'
filename = 'overlap_perf1.csv'
file = refdir + filename
avg_time_single = np.zeros(len(img))
data = np.genfromtxt(file, delimiter=',', skip_header=1)

for num, label in enumerate(img):
	# read img type column
	blocks = data[:,0]
	# get data that only match the current img
	img_data = data[blocks==num,:]
	# get data that only match the current process
	blocks = img_data[:,1]
	proc_data = img_data[blocks==1,:]
	# take the average time for each version
	avg_time_single[num] = np.mean(proc_data[:,4])

for i,image in enumerate(img):
	plt.figure()
	plt.plot(processes, avg_time[i,0,:], color='b', marker='*', label="N=1")
	plt.plot(processes, avg_time[i,1,:], color='r', marker='o', label="N=2")
	plt.plot(processes, avg_time[i,2,:], color='g', marker='^', label="N=3")
	plt.xlabel('Number of Processes')
	plt.ylabel('Time (s)')
	plt.legend()
	plt.grid(True)
	plt.savefig(plotdir + 'perf_exec_mul_nodes_img_'+ str(i) +'.eps', format='eps', dpi=1000)
	plt.close()


plt.figure()
plt.plot(size, (avg_time_single/avg_time[:,0,0])/processes[0], color='b', marker='*', label='P,N='+str(processes[0])+',1')
plt.plot(size, (avg_time_single/avg_time[:,0,2])/processes[2], color='r', marker='*', label='P,N='+str(processes[2])+',1')
plt.plot(size, (avg_time_single/avg_time[:,0,5])/processes[5], color='g', marker='*', label='P,N='+str(processes[5])+',1')
plt.plot(size, (avg_time_single/avg_time[:,1,0])/processes[0], color='b', marker='o', label='P,N='+str(processes[0])+',2')
plt.plot(size, (avg_time_single/avg_time[:,1,2])/processes[2], color='r', marker='o', label='P,N='+str(processes[2])+',2')
plt.plot(size, (avg_time_single/avg_time[:,1,5])/processes[5], color='g', marker='o', label='P,N='+str(processes[5])+',2')
plt.plot(size, (avg_time_single/avg_time[:,2,0])/processes[0], color='b', marker='^', label='P,N='+str(processes[0])+',3')
plt.plot(size, (avg_time_single/avg_time[:,2,2])/processes[2], color='r', marker='^', label='P,N='+str(processes[2])+',3')
plt.plot(size, (avg_time_single/avg_time[:,2,5])/processes[5], color='g', marker='^', label='P,N='+str(processes[5])+',3')
plt.xlabel('Size of Image')
plt.ylabel('Efficiency')
plt.legend(loc=4)
# plt.legend()
plt.grid(True)
plt.savefig(plotdir + 'perf_eff_mul_nodes.eps', format='eps', dpi=1000)
plt.close()
