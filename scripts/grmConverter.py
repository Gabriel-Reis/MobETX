# coding: utf-8
import pandas as pd
import datetime
import gzip
import os

print("Starting...")
#Import .csv
fileName = "trace"
# BonnMotionPath = ""
BonnMotionPath = "../bonnmotion-3.0.1/bin/"
data = pd.read_csv("../traces/"+fileName+".csv", names =  ['time', 'id', 'x', 'y'], sep = ' ')
print("CSV read...")

#Change Cols
data = data.loc[:,['id', 'time', 'x', 'y']]
# data = data[['id', 'time', 'x', 'y']]
timemin = data['time'].min()
#Remove ids <=0
data.drop(data[data.id<=0].index, inplace=True)
# data.drop(data[data.id>=10].index, inplace=True)

#ADD node 0, in center of space
new_row = pd.DataFrame({"time":[0], "id":[0], "x":data['x'].max()/2, "y":data['x'].max()/2})
data = data.append(new_row, ignore_index=True)

#Order DF
data.sort(['time'], inplace=True)
#Remove duplicates
data = data.drop_duplicates(['id', 'time']) # same node in more than one position.

#Save -cooja.csv
data.to_csv("../traces/"+fileName+"-cooja.csv", sep = ' ', index=False, header=False )
print("1/6 DONE")

#Prepare Bonnmotion Movements
input_file = "../traces/"+fileName+"-cooja.csv"
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
print("2/6 DONE")

# Writing movements (Bonnmotion)
with open("../traces/" + output_file, "w") as outfile:
    for node in sorted(node_info.keys()):
        outfile.write(''.join(str(e) for e in node_info[node])+"\n")
print("3/6 DONE")

# compacting movements
with open("../traces/" + output_file, 'rb') as in_f:
    dataMov = in_f.read()
    bindata = bytearray(dataMov)
    with gzip.open(BonnMotionPath + output_file + ".gz", "wb") as out_f:
        out_f.write(bindata)
print("4/6 DONE")

# stats and info vars
model="SWIM"
ignore=3600.0
seed=1592456788185
x=1600.0
y=1600.0
nn=100
circular="false"
j="2D"
nodeRadius=300

# Writing .params
with open(BonnMotionPath+fileName + "-cooja" + ".params", "w") as outfile:
    outfile.write("model="+model+"\n")
    outfile.write("ignore="+str(ignore)+"\n")
    outfile.write("randomSeed="+str(seed)+"\n")
    outfile.write("x="+str(x)+"\n")
    outfile.write("y="+str(y)+"\n")
    outfile.write("duration="+str(data['time'].max())+"\n")
    outfile.write("nn="+str(data['id'].max()+1)+"\n")
    outfile.write("circular="+circular+"\n")
    outfile.write("J="+j+"\n")
    outfile.write("nodeRadius="+str(nodeRadius))
print("5/6 DONE")

# Writing trace info
with open("../traces/"+fileName + "-cooja" + ".info", "w") as outfile:
    outfile.write("model name = "+fileName+"\n")
    outfile.write("x = "+str(x)+"\n")
    outfile.write("max x = "+str(data['x'].max())+"\n")
    outfile.write("y = "+str(y)+"\n")
    outfile.write("max y = "+str(data['y'].max())+"\n")
    outfile.write("duration = "+str(data['time'].max())+"\n")
    m, s = divmod(timemin, 60)
    h, m = divmod(m, 60)
    d, h = divmod(h, 24)
    outfile.write("first move before node 1 = "+'{:d}d{:d}:{:02d}:{:02d}'.format(d, h, m, s)+"s\n")
    m, s = divmod(data['time'].max(), 60)
    h, m = divmod(m, 60)
    d, h = divmod(h, 24)
    outfile.write("last move = "+'{:d}d{:d}:{:02d}:{:02d}'.format(d, h, m, s)+"s\n")
    outfile.write("nodes = "+str(data['id'].max()+1)+"\n")
    outfile.write("nodeRadius = "+str(nodeRadius))
print("6/6 DONE")
print("END")