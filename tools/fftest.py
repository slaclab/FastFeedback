#!/usr/local/lcls/package/python/current/bin/python

import sys
import os
import time
from epics import caget
from array import array
from epics import PV
from subprocess import call

iocs=["FB01","FB02","FB03","FB04","FB05"]
feedbacks=["GN01","TR01","TR02","TR03","TR04","TR05","LG01"]

states=["S1USED","S2USED","S3USED","S4USED","S5USED","S6USED"]

pvList=["NAME","ITERATEFUNC","TOTALPOI","SELECTALLDEVICES","MEASNUM","ACTNUM","STATENUM","PGAIN","IGAIN"]
pvArrayList=["FMATRIX","GMATRIX","MEASDSPR","ACTENERGY","REFORBIT","POI1","POI2","POI3","POI4"]

def setCompute(ioc, loop):
    cmd = "caput FBCK:" + ioc + ":" + loop + ":MODE 0  > /dev/null"
    os.system(cmd)

def setEnable(ioc, loop):
    cmd = "caput FBCK:" + ioc + ":" + loop + ":MODE 1  > /dev/null"
    os.system(cmd)

def setState(ioc, loop, state):
    cmd = "caput FBCK:" + ioc + ":" + loop + ":STATE " + state + " > /dev/null"
    os.system(cmd)

def enableState(ioc, loop, state):
    setLoopState(ioc, loop, state, "1")
    
def disableState(ioc, loop, state):
    setLoopState(ioc, loop, state, "0")

def setLoopState(ioc, loop, state, value):
    cmd = "caput FBCK:" + ioc + ":" + loop + ":" + state + "USED " + value + " > /dev/null"
    os.system(cmd)


ioc = "FB05"
loop = "LG01"
    
delay = 0
iterations = 0
countPv = ""

if (len(sys.argv) == 4):
    loop = sys.argv[1]
    delay = int(sys.argv[2])
    iterations = int(sys.argv[3])

if (len(sys.argv) == 5):
    loop = sys.argv[1]
    delay = int(sys.argv[2])
    iterations = int(sys.argv[3])
    countPv = sys.argv[4]

print "Testing loop " + ioc + ":" + loop

    
for i in range(iterations):
    print "Iteration " + str(i) + " for " + loop

    if (countPv != ""):
        cmd = "caput " + countPv + " " + str(i + 1) + " > /dev/null"
        os.system(cmd)

    setState(ioc, loop, "1")
    setEnable(ioc, loop)
    setCompute(ioc, loop)

#    time.sleep(20)
    time.sleep(1)
    
    setCompute(ioc, loop)
    time.sleep(1)
    setEnable(ioc, loop)
    time.sleep(1)

    # Toggle states many times, if LG01 is specified
    if (loop == "LG01"):
        enableState(ioc, loop, "S1")
        enableState(ioc, loop, "S2")
        enableState(ioc, loop, "S3")
        enableState(ioc, loop, "S4")
        enableState(ioc, loop, "S5")
        enableState(ioc, loop, "S6")

#        time.sleep(5)
        time.sleep(delay)

        disableState(ioc, loop, "S6")
        disableState(ioc, loop, "S5")
        disableState(ioc, loop, "S4")
        disableState(ioc, loop, "S3")
        disableState(ioc, loop, "S2")
        disableState(ioc, loop, "S1")
    else:
        time.sleep(delay)
    
    setCompute(ioc, loop)
    time.sleep(1)

    setEnable(ioc, loop)

    # Toggle states many times, if LG01 is specified
    if (loop == "LG01"):
        enableState(ioc, loop, "S1")
        enableState(ioc, loop, "S2")
        enableState(ioc, loop, "S3")
        enableState(ioc, loop, "S4")
        enableState(ioc, loop, "S5")
        enableState(ioc, loop, "S6")

        time.sleep(delay)
        
        disableState(ioc, loop, "S6")
        disableState(ioc, loop, "S5")
        disableState(ioc, loop, "S4")
        disableState(ioc, loop, "S3")
        disableState(ioc, loop, "S2")
        disableState(ioc, loop, "S1")
    else:
        time.sleep(delay)
        
    setCompute(ioc, loop)

    setEnable(ioc, loop)

    # Toggle states many times, if LG01 is specified
    if (loop == "LG01"):
        enableState(ioc, loop, "S1")
        enableState(ioc, loop, "S2")
        enableState(ioc, loop, "S3")
        enableState(ioc, loop, "S4")
        enableState(ioc, loop, "S5")
        enableState(ioc, loop, "S6")

#        time.sleep(5)
        time.sleep(delay)

        disableState(ioc, loop, "S6")
        disableState(ioc, loop, "S5")
        disableState(ioc, loop, "S4")
        disableState(ioc, loop, "S3")
        disableState(ioc, loop, "S2")
        disableState(ioc, loop, "S1")
    else:
        time.sleep(delay)
        
    setCompute(ioc, loop)

    setState(ioc, loop, "0")
    time.sleep(1)
#    time.sleep(delay)

