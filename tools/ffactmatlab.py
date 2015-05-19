#!/usr/local/lcls/package/python/current/bin/python

import sys
import os
from epics import caget
from array import array
from epics import PV
from subprocess import call

iocs=["FB01","FB02","FB03","FB04","FB05"]
feedbacks=["GN01","TR01","TR02","TR03","TR04","TR05","LG01"]

def checkLoop(argument):
    selectedIoc = "none"
    for feedback in feedbacks:
        if (feedback == argument):
            selectedLoop = feedback
        
    if (selectedLoop == "none"):
        sys.stdout.write("Invalid loop \"")
        sys.stdout.write(argument)
        sys.stdout.write("\", please use ")
        for feedback in feedbacks:
            sys.stdout.write(feedback + ", ")
        print ""
        exit(-1)
    else:
        return selectedLoop

destIoc = "FB05"

if (len(sys.argv) == 4):
    loop = checkLoop(sys.argv[1])
    basePv = sys.argv[2]
    startIndex = int(sys.argv[3])
else:
    print "Usage: ffactmatlab.py [loop] [Matlab Pv base: e.g. SIOC:SYS0:ML02:A0] [start number: e.g. 201]"
    exit(-1)
    
print "FB05 -> Loop " + loop

for i in range(10):
    actPv = basePv + str(i+startIndex)
    actDevName = "FBCK:FB05:" + loop + ":A" + str(i+1) + "DEVNAME" 
    cmd = "caput " + actDevName + " " + actPv
    os.system(cmd)
    

