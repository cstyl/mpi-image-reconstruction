import numpy as np
import matplotlib.pyplot as plt
import argparse

print('Running plots_mulnodes.py')

parser = argparse.ArgumentParser(description='Read number of repetitions and working directory')
parser.add_argument('-v', dest='version', type=str, help='Enter the version of the executable')
args = parser.parse_args()

plotdir = 'res/performance/plots_mulnodes/'
maindir = 'res/performance/'

version = args.version
test_name = 'mulnodes'
test_nodes = ['1', '2', '3']

print('Plotting results from ' + test_name + ' test with ' + version + ' communications')

img=['192x128', '256x192', '512x384', '768x768']
processes=[6, 12, 18, 24, 30, 36]
size=[192*128, 256*192, 512*384, 768*768]
avg_time = np.zeros((len(img),len(test_nodes),len(processes)))

for i,test in enumerate(test_nodes):
	test_dir = maindir + version + '_' + test_name + test_nodes[i] + '/'
	test_filename = version + '_' + test_name + test + '_perf.csv'
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
			avg_time[num,i,j] = np.mean(proc_data[:,4])

# get the reference implementation
ref_test_name = 'perf'
ref_test_nodes = '1'

ref_dir = maindir + version + '_' + ref_test_name + ref_test_nodes + '/'	
ref_filename = version + '_' + ref_test_name + ref_test_nodes + '_perf.csv'
ref_file = ref_dir + ref_filename

avg_time_single = np.zeros(len(img))
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
	avg_time_single[num] = np.mean(proc_data[:,4])

print('Plotting execution time vs number of processes for diffrent nodes:')
for i,image in enumerate(img):
	plt.figure()
	plt.plot(processes, avg_time[i,0,:], color='b', marker='*', label="N=1")
	plt.plot(processes, avg_time[i,1,:], color='r', marker='o', label="N=2")
	plt.plot(processes, avg_time[i,2,:], color='g', marker='^', label="N=3")
	plt.xlabel('Number of Processes')
	plt.ylabel('Time (s)')
	plt.legend()
	plt.grid(True)
	plt.savefig(plotdir + version + '_exec_img_'+ str(i) +'.eps', format='eps', dpi=1000)
	plt.close()
	print('\tImage' + image + ' completed.')

print('Plotting efficiency vs size of image per node:')
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
plt.savefig(plotdir + version + '_eff_img_.eps', format='eps', dpi=1000)
plt.close()
print('Plots completed.')
