import numpy as np
import matplotlib.pyplot as plt
import argparse

parser = argparse.ArgumentParser(description='Read number of repetitions and working directory')
parser.add_argument('-r', dest='reps', type=int, help='Enter the number of the repetitions performed')

args = parser.parse_args()

reps = args.reps
maindir = 'res/performance/overlap_perf1/'
plotdir = 'res/performance/plot_perf/'
filename = ['overlap_perf1.csv', 'no_overlap_perf1.csv']

img=['192x128', '256x192', '512x384', '768x768']
processes=[1, 2, 4, 8, 12, 15, 16, 23, 25, 28, 31, 32, 36]

size=[192*128, 256*192, 512*384, 768*768]

avg_time = np.zeros((len(filename), len(img),len(processes)))

for i, fin in enumerate(filename):
	file = maindir + fin
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
			avg_time[i,num,j] = np.mean(proc_data[:,4])

plt.figure()
plt.plot(processes, avg_time[0,0,:], '-*', label=str(img[0]))
plt.plot(processes, avg_time[0,1,:], '-o', label=str(img[1]))
plt.plot(processes, avg_time[0,2,:], '-^', label=str(img[2]))
plt.plot(processes, avg_time[0,3,:], '-+', label=str(img[3]))
plt.xlabel('Number of Processes')
plt.ylabel('Time (s)')
# plt.legend(loc=2)
plt.legend()
plt.grid(True)
plt.savefig(plotdir + 'perf_exec_over.eps', format='eps', dpi=1000)
plt.close()

plt.figure()
plt.plot(processes, avg_time[0,0,0]/avg_time[0,0,:], '-*', label=str(img[0]))
plt.plot(processes, avg_time[0,1,0]/avg_time[0,1,:], '-o', label=str(img[1]))
plt.plot(processes, avg_time[0,2,0]/avg_time[0,2,:], '-^', label=str(img[2]))
plt.plot(processes, avg_time[0,3,0]/avg_time[0,3,:], '-+', label=str(img[3]))
plt.plot(processes,processes,'k--')
plt.xlabel('Number of Processes')
plt.ylabel('Speed Up (times)')
plt.ylim([0,30])
plt.legend(loc=2)
# plt.legend()
plt.grid(True)
plt.savefig(plotdir + 'perf_speedup_over.eps', format='eps', dpi=1000)
plt.close()

plt.figure()
plt.plot(size, (avg_time[0,:,0]/avg_time[0,:,2])/processes[2], '-*', label='Proc='+str(processes[2]))
plt.plot(size, (avg_time[0,:,0]/avg_time[0,:,3])/processes[3], '-o', label='Proc='+str(processes[3]))
plt.plot(size, (avg_time[0,:,0]/avg_time[0,:,6])/processes[6], '-^', label='Proc='+str(processes[6]))
plt.plot(size, (avg_time[0,:,0]/avg_time[0,:,8])/processes[8], '-+', label='Proc='+str(processes[8]))
plt.plot(size, (avg_time[0,:,0]/avg_time[0,:,10])/processes[10], '->', label='Proc='+str(processes[10]))
plt.plot(size, (avg_time[0,:,0]/avg_time[0,:,11])/processes[11], '-<', label='Proc='+str(processes[11]))
plt.xlabel('Size of Image (pixels)')
plt.ylabel('Efficiency')
plt.legend(loc=4)
# plt.legend()
plt.grid(True)
plt.savefig(plotdir + 'perf_eff_over.eps', format='eps', dpi=1000)
plt.close()

#######################################
#######################################
#######################################
plt.figure()
plt.plot(processes, avg_time[1,0,:], '-*', label=str(img[0]))
plt.plot(processes, avg_time[1,1,:], '-o', label=str(img[1]))
plt.plot(processes, avg_time[1,2,:], '-^', label=str(img[2]))
plt.plot(processes, avg_time[1,3,:], '-+', label=str(img[3]))
plt.xlabel('Number of Processes')
plt.ylabel('Time (s)')
# plt.legend(loc=2)
plt.legend()
plt.grid(True)
plt.savefig(plotdir + 'perf_exec_no_over.eps', format='eps', dpi=1000)
plt.close()

plt.figure()
plt.plot(processes, avg_time[1,0,0]/avg_time[1,0,:], '-*', label=str(img[0]))
plt.plot(processes, avg_time[1,1,0]/avg_time[1,1,:], '-o', label=str(img[1]))
plt.plot(processes, avg_time[1,2,0]/avg_time[1,2,:], '-^', label=str(img[2]))
plt.plot(processes, avg_time[1,3,0]/avg_time[1,3,:], '-+', label=str(img[3]))
plt.plot(processes,processes,'k--')
plt.xlabel('Number of Processes')
plt.ylabel('Speed Up (times)')
plt.ylim([0,30])
plt.legend(loc=2)
# plt.legend()
plt.grid(True)
plt.savefig(plotdir + 'perf_speedup_no_over.eps', format='eps', dpi=1000)
plt.close()

plt.figure()
plt.plot(size, (avg_time[1,:,0]/avg_time[1,:,2])/processes[2], '-*', label='Proc='+str(processes[2]))
plt.plot(size, (avg_time[1,:,0]/avg_time[1,:,3])/processes[3], '-o', label='Proc='+str(processes[3]))
plt.plot(size, (avg_time[1,:,0]/avg_time[1,:,6])/processes[6], '-^', label='Proc='+str(processes[6]))
plt.plot(size, (avg_time[1,:,0]/avg_time[1,:,8])/processes[8], '-+', label='Proc='+str(processes[8]))
plt.plot(size, (avg_time[1,:,0]/avg_time[1,:,10])/processes[10], '->', label='Proc='+str(processes[10]))
plt.plot(size, (avg_time[1,:,0]/avg_time[1,:,11])/processes[11], '-<', label='Proc='+str(processes[11]))
plt.xlabel('Size of Image (pixels)')
plt.ylabel('Efficiency')
plt.legend(loc=4)
# plt.legend()
plt.grid(True)
plt.savefig(plotdir + 'perf_eff_no_over.eps', format='eps', dpi=1000)
plt.close()