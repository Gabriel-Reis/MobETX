import pandas as pd
import numpy as np
import datetime
import gzip
import os

traces_file_path = "../traces/"
BonnMotionPath = "../bonnmotion-3.0.1/bin/"
#BonnMotionPath = ""
#Define trace used
trace_used = "RandomWalk"


# Traces set
trace_indexes = pd.Series(["Boundless","Column","GaussMarkov","Grm","ManhattanGrid","Nomadic","Pursue","RandomWalk",
                           "RandomWaypoint","Slaw","Static","SteadyStateRandomWaypoint"],
                          index=[0,1,2,3,4,5,6,7,8,9,10,11])
traces_file_names = ["trace_boundless","trace_column","trace_gaussmarkov","trace_grm","trace_manhattangrid",
                     "trace_nomadic","trace_pursue","trace_randomwalk","trace_randomwalkwaypoint","trace_slaw",
                     "trace_static","trace_steadystaterandomwaypoint"]
traces_files_index = (['id', 'time', 'x', 'y'],['id', 'time', 'x', 'y'],['id', 'time', 'x', 'y'],['time', 'id', 'x', 'y'],
                      ['id', 'time', 'x', 'y'],['id', 'time', 'x', 'y'],['id', 'time', 'x', 'y'],['id', 'time', 'x', 'y'],
                      ['id', 'time', 'x', 'y'],['id', 'time', 'x', 'y'],['id', 'time', 'x', 'y'],['id', 'time', 'x', 'y'],
                      ['id', 'time', 'x', 'y'],['id', 'time', 'x', 'y'],['id', 'time', 'x', 'y'],['id', 'time', 'x', 'y'])


#Import trace .csv
trace_index_value = trace_indexes[trace_indexes == trace_used].index[0]
fileName = traces_file_path+traces_file_names[trace_index_value]
file=traces_file_names[trace_index_value]
data = pd.read_csv(""+fileName+".csv", names =  traces_files_index[trace_index_value], sep = ' ', dtype = {"id":np.int32,"x":np.float32,"y":np.float32})


#Remove ids <=0
time_min = data['time'].min()
data.drop(data[data.id<=0].index, inplace=True)
#ADD node 0, in center of space
x = data['x'].max()/2;
y = data['y'].max()/2;
print(x);
new_row = pd.DataFrame({"time":[0], "id":[0], "x":x, "y":y})
data = data.append(new_row, ignore_index=True)
#Change Cols
data = data.loc[:,['id', 'time', 'x', 'y']]
#Order DF
data.sort(['time'], inplace=True)
#Remove duplicates
data = data.drop_duplicates(['id', 'time']) # same node in more than one position.


#Save -coojaReady.csv
data.to_csv(""+fileName+"-coojaReady.csv", sep = ' ', index=False, header=False )
print("1/5 DONE")


#Prepare Bonnmotion Movements
input_file = ""+fileName+"-coojaReady.csv"
output_file = os.path.basename(input_file)[:-3] + "movements"
node_info = {}

# parse cooja trace to bonnMotion movements
with open(input_file, "r") as infile:
    for l in infile.readlines():
        l = l.replace('\n', '')
        l = l.split(' ')
        
        if int(l[0]) in node_info.keys():
            node_info[int(l[0])].append(l[1]+" "+l[2]+" "+l[3]+" ")
        else:
            node_info[int(l[0])] = []
            node_info[int(l[0])].append(l[1]+" "+l[2]+" "+l[3]+" ")
print("2/5 DONE")


# Writing movements (Bonnmotion)
with open("" + output_file, "w") as outfile:
    for node in sorted(node_info.keys()):
        outfile.write(''.join(str(e) for e in node_info[node])+"\n")
print("3/5 DONE")

# compacting movements
with open("" + output_file, 'rb') as in_f:
    dataMov = in_f.read()
    bindata = bytearray(dataMov)
    with gzip.open(BonnMotionPath + output_file + ".gz", "wb") as out_f:
        out_f.write(bindata)
print("4/5 DONE")


# stats and info vars
ignore="3600.0"
seed="1592456788185"
circular="false"
j="2D"
# Base params
params = "model="+trace_used+"\nignore="+ignore+"\nrandomSeed="+seed+"\nx="+str(int(data['x'].max())+5)+"\ny="+str(int(data['y'].max())+5)+"\nduration="+str(data['time'].max())+"\nnn="+str(data['id'].max()+1)+"\ncircular=false\nJ=2D"
#Specified params
Bound = "deltaT=10.0\naccelMax=1.5\nalpha=90.0\nminspeed=0.5\nmaxspeed=1.5\nmaxpause=60.0"
Column = "numgroups=20\nrefptseparation=50.0\nmaxdist=15.0\nminspeed=0.5\nmaxspeed=1.5\nmaxpause=60.0"
Gauss = "updateFrequency=10.0\nmaxspeed=1.5\nangleStdDev=0.39269908169872414\nspeedStdDev=0.5\ninputX=1000.0\ninputY=1000.0\nbounce=false\ninitGauss=true\nuniformSpeed=false"
Grm = "nodeRadius=300"
ManhattanGrid = "xblocks=10\nyblocks=10\nupdateDist=5.0\nturnProb=0.5\nspeedChangeProb=0.8\nminSpeed=0.2\nmeanSpeed=1.1\nspeedStdDev=0.1\npauseProb=0.19999999999999996\nmaxPause=1800.0\n"
Nomadic = "avgMobileNodesPerGroup=3.0\ngroupSizeDeviation=5.0\nmaxdist=50.0\nrefmaxpause=60.0\nminspeed=0.1\nmaxspeed=4.1\nmaxpause=600.0"
Pursue = "maxspeed=4.1\nminspeed=0.1\naggressiveness=0.5\npursueRandomnessMagnitude=100.0"
RandomWalk = "mode=s\nmodeDelta=600.0\nminspeed=0.5\nmaxspeed=4.1\nmaxpause=60.0"
RandomWaypoint = "dim=3\nminspeed=0.5\nmaxspeed=4.1\nmaxpause=600.0"
Slaw = "dim=4\nminspeed=0.5\nmaxspeed=1.5\nmaxpause=60.0"
Static = "densityLevels=1"
SteadyStateRandomWaypoint = "speedMean=4.1\nspeedDelta=3.0\npauseMean=600.0\npauseDelta=400.0"


# Writing .params
with open(BonnMotionPath+file + "-coojaReady" + ".params", "w") as outfile:
    outfile.write(params)
    if trace_used=="Grm":
        outfile.write("\n"+Grm)
    elif trace_used=="Boundless":
        outfile.write("\n"+Bound)
    elif trace_used=="Column":
        outfile.write("\n"+Column)
    elif trace_used=="GaussMarkov":
        outfile.write("\n"+Gauss)
    elif trace_used=="Nomadic":
        outfile.write("\n"+Nomadic)
    elif trace_used=="ManhattanGrid":
        outfile.write("\n"+ManhattanGrid)
    elif trace_used=="Pursue":
        outfile.write("\n"+Pursue)
    elif trace_used=="RandomWalk":
        outfile.write("\n"+RandomWalk)
    elif trace_used=="RandomWaypoint":
        outfile.write("\n"+RandomWaypoint)
    elif trace_used=="Slaw":
        outfile.write("\n"+Slaw)
    elif trace_used=="Static":
        outfile.write("\n"+Static)
    elif trace_used=="SteadyStateRandomWaypoint":
        outfile.write("\n"+SteadyStateRandomWaypoint)
print("5/5 DONE")

print("Files:")
print("---->"+fileName+"-coojaReady.csv")
print("---->"+os.path.basename(input_file)[:-3] + "movements")
print("---->"+BonnMotionPath + output_file + ".gz")

#Writing trace info
with open(fileName + "-coojaReady" + ".info", "w") as outfile:
    outfile.write("model name = "+fileName+"\n")
    outfile.write("x = "+str(int(data['x'].max())+5)+"\n")
    outfile.write("max x = "+str(data['x'].max())+"\n")
    outfile.write("y = "+str(int(data['y'].max())+5)+"\n")
    outfile.write("max y = "+str(data['y'].max())+"\n")
    outfile.write("duration = "+str(data['time'].max())+"\n")
    outfile.write("first move = "+str(time_min)+"\n")
    # m, s = divmod(time_min, 60)
    # h, m = divmod(m, 60)
    # d, h = divmod(h, 24)
    #outfile.write("first move before node 1 = "+'{:d}d{:d}:{:02d}:{:02d}'.format(d, h, m, s)+"s\n")
    # m, s = divmod(data['time'].max(), 60)
    # h, m = divmod(m, 60)
    # d, h = divmod(h, 24)
    #outfile.write("last move = "+'{:d}d{:d}:{:02d}:{:02d}'.format(d, h, m, s)+"s\n")
    outfile.write("last move = "+str(data['time'].max())+"\n")
    outfile.write("nodes = "+str(data['id'].max()+1)+"\n")
print("6/5 DONE")
print("END")