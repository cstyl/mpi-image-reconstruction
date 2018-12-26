import numpy as np
import matplotlib.pyplot as plt
import argparse

plotdir = 'res/performance/plots_mulnodes/'
maindir = 'res/performance/'

version = ['overlap', 'no_overlap']
test_name = 'mulnodes'
test_nodes = ['1', '2', '3']

img=['192x128', '256x192', '512x384', '768x768']
processes=[6, 12, 18, 24, 30, 36]
size=[192*128, 256*192, 512*384, 768*768]
avg_time = np.zeros((len(version),len(img),len(test_nodes),len(processes)))


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
				avg_time[v,num,i,j] = np.mean(proc_data[:,4])

	# get the reference implementation
	ref_test_name = 'perf'
	ref_test_nodes = '1'
	
	ref_dir = maindir + ver + '_' + ref_test_name + ref_test_nodes + '/'	
	ref_filename = ver + '_' + ref_test_name + ref_test_nodes + '_perf.csv'
	ref_file = ref_dir + ref_filename

	avg_time_single = np.zeros((len(version),len(img)))
	print('Reading from reference file: ' + ref_file)

	data = np.genfromtxt(ref_file, delimiter=',', skip_header=1)

	for num, label in enumerate(img):
		# read img type column
		blocks = data[:,0]
		# get data that only match the current img
		img_data = data[blocks==num,:]
		# get data that only match the current process
		blocks = img_data[:,1]
		proc_data = img_data[blocks==1,:]
		# take the average time for each version
		avg_time_single[v,num] = np.mean(proc_data[:,4])

	for i,image in enumerate(img):
		plt.figure()
		plt.plot(processes, avg_time[v,i,0,:], color='b', marker='*', label="N=1")
		plt.plot(processes, avg_time[v,i,1,:], color='r', marker='o', label="N=2")
		plt.plot(processes, avg_time[v,i,2,:], color='g', marker='^', label="N=3")
		plt.xlabel('Number of Processes')
		plt.ylabel('Time (s)')
		plt.legend()
		plt.grid(True)
		plt.savefig(plotdir + ver + '_exec_img_'+ str(i) +'.eps', format='eps', dpi=1000)
		plt.close()


	plt.figure()
	plt.plot(size, (avg_time_single[v,:]/avg_time[v,:,0,0])/processes[0], color='b', marker='*', label='P,N='+str(processes[0])+',1')
	plt.plot(size, (avg_time_single[v,:]/avg_time[v,:,0,2])/processes[2], color='r', marker='*', label='P,N='+str(processes[2])+',1')
	plt.plot(size, (avg_time_single[v,:]/avg_time[v,:,0,5])/processes[5], color='g', marker='*', label='P,N='+str(processes[5])+',1')
	plt.plot(size, (avg_time_single[v,:]/avg_time[v,:,1,0])/processes[0], color='b', marker='o', label='P,N='+str(processes[0])+',2')
	plt.plot(size, (avg_time_single[v,:]/avg_time[v,:,1,2])/processes[2], color='r', marker='o', label='P,N='+str(processes[2])+',2')
	plt.plot(size, (avg_time_single[v,:]/avg_time[v,:,1,5])/processes[5], color='g', marker='o', label='P,N='+str(processes[5])+',2')
	plt.plot(size, (avg_time_single[v,:]/avg_time[v,:,2,0])/processes[0], color='b', marker='^', label='P,N='+str(processes[0])+',3')
	plt.plot(size, (avg_time_single[v,:]/avg_time[v,:,2,2])/processes[2], color='r', marker='^', label='P,N='+str(processes[2])+',3')
	plt.plot(size, (avg_time_single[v,:]/avg_time[v,:,2,5])/processes[5], color='g', marker='^', label='P,N='+str(processes[5])+',3')
	plt.xlabel('Size of Image')
	plt.ylabel('Efficiency')
	plt.legend(loc=4)
	# plt.legend()
	plt.grid(True)
	plt.savefig(plotdir + ver + '_eff_img_.eps', format='eps', dpi=1000)
	plt.close()
