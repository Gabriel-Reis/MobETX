#!/usr/bin/env python
# coding: utf-8

# ## grm to bonn moviments

# In[61]:


import pandas as pd
import os
import gzip


# In[64]:
fileName = "trace"
BonnMotionPath = "../bonnmotion-3.0.1/bin/"

# parsing grm raw data
data = pd.read_csv("../traces/"+fileName+".csv", names =  ['time', 'id', 'x', 'y'], sep = ' ')

# Converting to cooja format
data = data[['id', 'time', 'x', 'y']]

#removing erros
data = data.drop_duplicates(['id', 'time']) # same node in more than one position.

# Sorting by time and exporting to csv.
data.sort('time').to_csv("../traces/"+fileName+"-cooja.csv", sep = ' ', index=False, header=False )


input_file = "../traces/"+fileName+"-cooja.csv"
output_file = os.path.basename(input_file)[:-3] + "movements"
node_info = {}

count = 0
# parse cooja trace 2 bonn movements
with open(input_file, "r") as infile:
    for l in infile.readlines():
        l = l.replace('\n', '')
        l = l.split(' ')
        
        if int(l[0]) in node_info.keys():
            node_info[int(l[0])].append(l[1]+" "+l[2]+" "+l[3]+" ")
        else:
            node_info[int(l[0])] = []
            node_info[int(l[0])].append(l[1]+" "+l[2]+" "+l[3]+" ")
        
        """print(l)
        if count == 10:
            break
        else:
            count += 1"""

# Writing movements
with open("../traces/" + output_file, "w") as outfile:
    for node in sorted(node_info.keys()):
        outfile.write(''.join(str(e) for e in node_info[node])+"\n")

# compacting movements
with open("../traces/" + output_file, 'rb') as in_f:
    data = in_f.read()
    bindata = bytearray(data)
    with gzip.open(BonnMotionPath + output_file + ".gz", "wb") as out_f:
        out_f.write(bindata)

# Writing .stats 
with open("../traces/" + fileName + ".params", "w") as outfile:
    outfile.write("model=SWIM"+"\n")
    outfile.write("ignore=3600.0"+"\n")
    outfile.write("randomSeed=1592456788185"+"\n")
    outfile.write("x=1600.0"+"\n")
    outfile.write("y=1600.0"+"\n")
    outfile.write("duration=3600.0"+"\n")
    outfile.write("nn=10"+"\n")
    outfile.write("circular=false"+"\n")
    outfile.write("J=2D"+"\n")
    outfile.write("nodeRadius=0.1")
# cellDistanceWeight=0.0
# nodeSpeedMultiplier=1.0
# waitingTimeExponent=0.0
# waitingTimeUpperBound=0.0
