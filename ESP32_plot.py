import matplotlib.pyplot as plt
import numpy

# Get user input
filePath = input("Please enter file path ")
startTime = float(input("Please desired start time "))
endTime = float(input("Please desired length ")) + startTime

# Read data in from files
data = {'x': [], 'y': [], 'z': []}
with open(str(filePath + "AccX.txt"), 'r') as file:
    for line in file:
        data['x'].append(int(line) / 16384.0)
with open(str(filePath + "AccY.txt"), 'r') as file:
    for line in file:
        data['y'].append(int(line) / 16384.0)
with open(str(filePath + "AccZ.txt"), 'r') as file:
    for line in file:
        data['z'].append(int(line) / 16384.0)

times = [num for num in range(len(data['z']))]

# clip data to specified bounds
while times[0] < startTime:
    times.pop(0)
    data['x'].pop(0)
    data['y'].pop(0)
    data['z'].pop(0)
while times[-1] > endTime:
    times.pop(-1)
    data['x'].pop(-1);
    data['y'].pop(-1);
    data['z'].pop(-1); 

times = [t - startTime for t in times] # move times to start time

# plot data and format
fig = plt.figure(figsize=(7, 5))
plot = [fig.add_subplot(111)]
plot[0].plot(times, data['x'], c = "r", linewidth = 2.0)
plot[0].plot(times, data['y'], c = "g", linewidth = 2.0)
plot[0].plot(times, data['z'], c = "b", linewidth = 2.0)
plot[0].axis([0, endTime - startTime, None, None])
plot[0].set_ylabel("g")
plot[0].set_xlabel("Data point #")
plot[0].legend(("X acceleration", "Y acceleration", "Z acceleration"))
plot[0].grid(linewidth = 2.0)
fig.tight_layout()
fig.savefig("acc.png", dpi = 500, transparent=True)
plt.show()