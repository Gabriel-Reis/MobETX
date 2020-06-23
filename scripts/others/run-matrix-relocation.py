
# coding: utf-8

# # Imports

# In[1]:


import logging
reload(logging)
logger = logging.getLogger(__name__)
logging.basicConfig(format='%(asctime)s %(levelname)s:%(message)s', 
                    level=logging.INFO, 
                    datefmt='%I:%M:%S')


# In[2]:


import xml.etree.ElementTree as ET
from os import listdir, makedirs
from os.path import isfile, join, exists
import shutil
from subprocess import call


# ### building blocks

# In[3]:


wdir = "./"
simulation_XML = "./experiments-matrix-101.csc"
N = 5


# ### Code

# ###### handle simulation csc xml file

# In[ ]:


tracesDir = [f for f in listdir(wdir) if "traces" in f.split('-')]
#print tracesDir
for td in tracesDir:
    
    onlyfiles = [f for f in listdir(wdir + td) if isfile(join(wdir + td, f))]
    #print onlyfiles
    
    for trace in onlyfiles:
        xmlTree = ET.parse(simulation_XML)
        plugins = xmlTree.findall("plugin")

        for p in plugins:
            if "Mobility" in p.text:
                #print p.tag, p.attrib, p.text
                #print dir(p)
                #print p.find("plugin_config").find("positions").text
                #print p.find("plugin_config").find("positions").text.split("/")
                #print "/".join(p.find("plugin_config").find("positions").text.split("/")[:-2]) + "/" + td + "/" + trace
                new_trace = "/".join(p.find("plugin_config").find("positions").text.split("/")[:-2]) + "/" + td + "/" + trace
                p.find("plugin_config").find("positions").text = new_trace
                
                xmlTree.write(simulation_XML)
        
        logging.info("RUNNING THE SIMULATIONS FOR " + td + " " + trace)
        command = "sh RUN_REPEATED "+ str(N) + " " + simulation_XML[2:-4]
        print command
        try:
            import os
            os.system(command)
            #call(command)
            #call("ls")
        except:
            logging.error("The command "+command+" couldn't be called.\n")
            
        output_dir = wdir + "logs/" +  "-".join(["logs", td, trace[:-4]]) + "/"
        logging.info("CREATING OUTPUT LOG DIR " + output_dir)
        
        if not exists(output_dir):
            makedirs(output_dir)
        
        logfiles = [f for f in listdir(wdir) if isfile(join(wdir, f)) and "log" in f]
        
        if len(logfiles) > 0:
            logging.info("MOVING THE FOLLOWING FILES: to " + output_dir + " ".join(logfiles) )

            for logf in logfiles:
                shutil.move(wdir + logf, output_dir)
        
        
        logging.info("SIMULATION FINISHED\n\n")
        #break 
    #break

