import numpy as np
import matplotlib.pyplot as plt

plotdir = 'res/performance/plots_perf/'
maindir = 'res/performance/'

version = ['overlap', 'no_overlap']
test_name = 'perf'
test_nodes = '1'

img=['192x128', '256x192', '512x384', '768x768']
processes=[1, 2, 4, 8, 12, 15, 16, 23, 25, 28, 31, 32, 36]

size=[192*128, 256*192, 512*384, 768*768]

avg_time = np.zeros((len(test_nodes), len(img),len(processes)))

for v, ver in enumerate(version):
	for i,test in enumerate(test_nodes):
		test_dir = maindir + ver + '_' + test_name + test_nodes[i] + '/'
		test_filename = ver + '_' + test_name + test + '_perf.csv'
		file = test_dir + test_filename
		print('Reading from file: ' + file)	
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

	for i, test in enumerate(test_nodes):
		plt.figure()
		plt.plot(processes, avg_time[i,0,:], '-*', label=str(img[0]))
		plt.plot(processes, avg_time[i,1,:], '-o', label=str(img[1]))
		plt.plot(processes, avg_time[i,2,:], '-^', label=str(img[2]))
		plt.plot(processes, avg_time[i,3,:], '-+', label=str(img[3]))
		plt.xlabel('Number of Processes')
		plt.ylabel('Time (s)')
		# plt.legend(loc=2)
		plt.legend()
		plt.grid(True)
		plt.savefig(plotdir + ver + '_exec_time.eps', format='eps', dpi=1000)
		plt.close()

		plt.figure()
		plt.plot(processes, avg_time[i,0,0]/avg_time[i,0,:], '-*', label=str(img[0]))
		plt.plot(processes, avg_time[i,1,0]/avg_time[i,1,:], '-o', label=str(img[1]))
		plt.plot(processes, avg_time[i,2,0]/avg_time[i,2,:], '-^', label=str(img[2]))
		plt.plot(processes, avg_time[i,3,0]/avg_time[i,3,:], '-+', label=str(img[3]))
		plt.plot(processes,processes,'k--')
		plt.xlabel('Number of Processes')
		plt.ylabel('Speed Up (times)')
		plt.ylim([0,30])
		plt.legend(loc=2)
		# plt.legend()
		plt.grid(True)
		plt.savefig(plotdir + ver + '_speedup.eps', format='eps', dpi=1000)
		plt.close()

		plt.figure()
		plt.plot(size, (avg_time[i,:,0]/avg_time[i,:,2])/processes[2], '-*', label='Proc='+str(processes[2]))
		plt.plot(size, (avg_time[i,:,0]/avg_time[i,:,3])/processes[3], '-o', label='Proc='+str(processes[3]))
		plt.plot(size, (avg_time[i,:,0]/avg_time[i,:,6])/processes[6], '-^', label='Proc='+str(processes[6]))
		plt.plot(size, (avg_time[i,:,0]/avg_time[i,:,8])/processes[8], '-+', label='Proc='+str(processes[8]))
		plt.plot(size, (avg_time[i,:,0]/avg_time[i,:,10])/processes[10], '->', label='Proc='+str(processes[10]))
		plt.plot(size, (avg_time[i,:,0]/avg_time[i,:,11])/processes[11], '-<', label='Proc='+str(processes[11]))
		plt.xlabel('Size of Image (pixels)')
		plt.ylabel('Efficiency')
		plt.legend(loc=4)
		# plt.legend()
		plt.grid(True)
		plt.savefig(plotdir + ver + '_efficiency.eps', format='eps', dpi=1000)
		plt.close()

#######################################
#######################################
#######################################
# plt.figure()
# plt.plot(processes, avg_time[1,0,:], '-*', label=str(img[0]))
# plt.plot(processes, avg_time[1,1,:], '-o', label=str(img[1]))
# plt.plot(processes, avg_time[1,2,:], '-^', label=str(img[2]))
# plt.plot(processes, avg_time[1,3,:], '-+', label=str(img[3]))
# plt.xlabel('Number of Processes')
# plt.ylabel('Time (s)')
# # plt.legend(loc=2)
# plt.legend()
# plt.grid(True)
# plt.savefig(plotdir + 'perf_exec_no_over.eps', format='eps', dpi=1000)
# plt.close()

# plt.figure()
# plt.plot(processes, avg_time[1,0,0]/avg_time[1,0,:], '-*', label=str(img[0]))
# plt.plot(processes, avg_time[1,1,0]/avg_time[1,1,:], '-o', label=str(img[1]))
# plt.plot(processes, avg_time[1,2,0]/avg_time[1,2,:], '-^', label=str(img[2]))
# plt.plot(processes, avg_time[1,3,0]/avg_time[1,3,:], '-+', label=str(img[3]))
# plt.plot(processes,processes,'k--')
# plt.xlabel('Number of Processes')
# plt.ylabel('Speed Up (times)')
# plt.ylim([0,30])
# plt.legend(loc=2)
# # plt.legend()
# plt.grid(True)
# plt.savefig(plotdir + 'perf_speedup_no_over.eps', format='eps', dpi=1000)
# plt.close()

# plt.figure()
# plt.plot(size, (avg_time[1,:,0]/avg_time[1,:,2])/processes[2], '-*', label='Proc='+str(processes[2]))
# plt.plot(size, (avg_time[1,:,0]/avg_time[1,:,3])/processes[3], '-o', label='Proc='+str(processes[3]))
# plt.plot(size, (avg_time[1,:,0]/avg_time[1,:,6])/processes[6], '-^', label='Proc='+str(processes[6]))
# plt.plot(size, (avg_time[1,:,0]/avg_time[1,:,8])/processes[8], '-+', label='Proc='+str(processes[8]))
# plt.plot(size, (avg_time[1,:,0]/avg_time[1,:,10])/processes[10], '->', label='Proc='+str(processes[10]))
# plt.plot(size, (avg_time[1,:,0]/avg_time[1,:,11])/processes[11], '-<', label='Proc='+str(processes[11]))
# plt.xlabel('Size of Image (pixels)')
# plt.ylabel('Efficiency')
# plt.legend(loc=4)
# # plt.legend()
# plt.grid(True)
# plt.savefig(plotdir + 'perf_eff_no_over.eps', format='eps', dpi=1000)
# plt.close()