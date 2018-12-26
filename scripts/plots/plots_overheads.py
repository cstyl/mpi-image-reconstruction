import numpy as np
import matplotlib.pyplot as plt
import argparse

print('Running plots_overheads.py')

parser = argparse.ArgumentParser(description='Read version of executable')
parser.add_argument('-v', dest='version', type=str, help='Enter the version of the executable')
args = parser.parse_args()

plotdir = 'res/overheads/plots_overheads/'
maindir = 'res/overheads/'

version = args.version
test_name = ['delta', 'avg']

print('Plotting results from overheads test with ' + version + ' communications')

img=['192x128', '256x192', '512x384', '768x768']
freq=['No_Max', 'Max_Fr=1']
procs=(2, 4, 8, 12, 19)
size=[192*128, 256*192, 512*384, 768*768]
avg_time = np.zeros((len(test_name),len(procs),len(img),len(freq)+1))

for i, test in enumerate(test_name):
	test_dir = maindir + version + '_' + test + '/'
	test_filename = version + '_' + test + '_result.csv'
	file = test_dir + test_filename

	print('Reading from file: ' + file)	
	data = np.genfromtxt(file, delimiter=',', skip_header=1)

	for k,proc in enumerate(procs):
		# get per process data
		blocks = data[:,2]
		proc_data = data[blocks==proc,:]
		for num, label in enumerate(img):
			# read img type column
			blocks = proc_data[:,0]
			# get data that only match the current img
			img_data = proc_data[blocks==num,:]

			for j,ver in enumerate(freq):
				# get data that only match the current version
				blocks = img_data[:,1]
				ver_data = img_data[blocks==j,:]
				# take the average time for each version
				avg_time[i,k,num,j] = np.mean(ver_data[:,5])

			# get the overheads
			avg_time[i,k,num,2] = avg_time[i,k,num,1] - avg_time[i,k,num,0]

# plot the ratio of overheads/time w/out early stop
plt.figure()
plt.plot(size, avg_time[0,0,:,2]/avg_time[0,0,:,0], '-*', label='P='+str(procs[0]))
plt.plot(size, avg_time[0,1,:,2]/avg_time[0,1,:,0], '-*', label='P='+str(procs[1]))
plt.plot(size, avg_time[0,2,:,2]/avg_time[0,2,:,0], '-*', label='P='+str(procs[2]))
plt.plot(size, avg_time[0,3,:,2]/avg_time[0,3,:,0], '-*', label='P='+str(procs[3]))
plt.plot(size, avg_time[0,4,:,2]/avg_time[0,4,:,0], '-*', label='P='+str(procs[4]))
plt.xlabel('Size of Image (pixels)')
plt.ylabel('Ratio')
plt.legend(loc=4)
# plt.legend()
plt.grid(True)
plt.savefig(plotdir + version + '_delta_overheads.eps', format='eps', dpi=1000)
plt.close()

# plot the ratio of overheads/time w/out early stop
plt.figure()
plt.plot(size, avg_time[1,0,:,2]/avg_time[1,0,:,0], '-*', label='P='+str(procs[0]))
plt.plot(size, avg_time[1,1,:,2]/avg_time[1,1,:,0], '-*', label='P='+str(procs[1]))
plt.plot(size, avg_time[1,2,:,2]/avg_time[1,2,:,0], '-*', label='P='+str(procs[2]))
plt.plot(size, avg_time[1,3,:,2]/avg_time[1,3,:,0], '-*', label='P='+str(procs[3]))
plt.plot(size, avg_time[1,4,:,2]/avg_time[1,4,:,0], '-*', label='P='+str(procs[4]))
plt.xlabel('Size of Image (pixels)')
plt.ylabel('Ratio')
plt.legend(loc=4)
# plt.legend()
plt.grid(True)
plt.savefig(plotdir + version + '_avg_overheads.eps', format='eps', dpi=1000)
plt.close()

iterations=[1190, 867, 1435, 1226]
plt.figure()
plt.plot(size, iterations, '-*')
plt.xlabel('Size of Image (pixels)')
plt.ylabel('Iterations')
plt.grid(True)
plt.savefig(plotdir + version + '_early_stop.eps', format='eps', dpi=1000)
plt.close()

