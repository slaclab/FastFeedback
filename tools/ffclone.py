#!/usr/local/lcls/package/python/current/bin/python

import sys
import os
from epics import caget
from array import array
from epics import PV
from subprocess import call

iocs=["FB01","FB02","FB03","FB04","FB05"]
feedbacks=["GN01","TR01","TR02","TR03","TR04","TR05","LG01"]

pvList=["NAME","ITERATEFUNC","TOTALPOI","SELECTALLDEVICES","MEASNUM","ACTNUM","STATENUM","PGAIN","IGAIN"]
pvArrayList=["FMATRIX","GMATRIX","MEASDSPR","ACTENERGY","REFORBIT","POI1","POI2","POI3","POI4"]

def copyArrayPv(source, dest, verbose):
    if (verbose == True):
        cmd = "caput -w 5 -a " + dest + " `caget -t " + source + "`"
    else:
        cmd = "caput -w 5 -a " + dest + " `caget -t " + source + "` > /dev/null"
    os.system(cmd)

def copyPv(source, dest, verbose):
    if (verbose == True):
        cmd = "caput -w 5" + dest + " `caget -t " + source + "`"
    else:
        cmd = "caput -w 5 " + dest + " `caget -t " + source + "` > /dev/null 2> /dev/null"
    os.system(cmd)

def copyDevices(feedback, sourceIoc, destIoc, count, type, nameField):
    for i in range(count):
        copyPv("FBCK:" + sourceIoc + ":" + feedback + ":" + type + str(i+1) + nameField,
               "FBCK:" + destIoc + ":" + feedback + ":" + type + str(i+1) + nameField, False)

    for i in range(count):
        copyPv("FBCK:" + sourceIoc + ":" + feedback + ":" + type + str(i+1) + "USED",
               "FBCK:" + destIoc + ":" + feedback + ":" + type + str(i+1) + "USED", False)

    if (type != "S"):
        for i in range(count):
            copyPv("FBCK:" + sourceIoc + ":" + feedback + ":" + type + str(i+1) + "CAMODE",
                   "FBCK:" + destIoc + ":" + feedback + ":" + type + str(i+1) + "CAMODE", False)

def clone(feedback, sourceIoc, destIoc):
    for pvProperty in pvList:
        copyPv("FBCK:" + sourceIoc + ":" + feedback + ":" + pvProperty,
               "FBCK:" + destIoc + ":" + feedback + ":" + pvProperty, False)
        
    copyDevices(feedback, sourceIoc, destIoc, 20, "M", "DEVNAME")
    copyDevices(feedback, sourceIoc, destIoc, 10, "A", "DEVNAME")
    copyDevices(feedback, sourceIoc, destIoc, 10, "S", "NAME")

    copyDevices(feedback, sourceIoc, destIoc, 20, "M", "HIHI")
    copyDevices(feedback, sourceIoc, destIoc, 20, "M", "LOLO")
    copyDevices(feedback, sourceIoc, destIoc, 10, "A", "HIHI")
    copyDevices(feedback, sourceIoc, destIoc, 10, "A", "LOLO")
    copyDevices(feedback, sourceIoc, destIoc, 10, "S", "HIHI")
    copyDevices(feedback, sourceIoc, destIoc, 10, "S", "LOLO")

    for pvProperty in pvArrayList:
        copyArrayPv("FBCK:" + sourceIoc + ":" + feedback + ":" + pvProperty,
                    "FBCK:" + destIoc + ":" + feedback + ":" + pvProperty, False)

def checkIoc(argument):
    selectedIoc = "none"
    for ioc in iocs:
        if (ioc == argument):
            selectedIoc = ioc
        
    if (selectedIoc == "none"):
        sys.stdout.write("Invalid IOC \"")
        sys.stdout.write(argument)
        sys.stdout.write("\", please use ")
        for ioc in iocs:
            sys.stdout.write(ioc + ", ")
        print ""
        exit(-1)
    else:
        return selectedIoc

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
selectedLoop = "all"
if (len(sys.argv) == 2):
    sourceIoc = checkIoc(sys.argv[1])
elif (len(sys.argv) == 3):
    sourceIoc = checkIoc(sys.argv[1])
    selectedLoop = checkLoop(sys.argv[2])
else:
    print "Usage: ffclone.py [source] [loop]"
    exit(-1)
    
print "Source " + sourceIoc + " -> Destination " + destIoc
print "Loop " + selectedLoop

if (selectedLoop == "all"):
    for feedback in feedbacks:
        print "  -> " + feedback + " ..."
        clone(feedback, sourceIoc, destIoc)
        cmd = "caput FBCK:" + destIoc + ":" + feedback + ":SAVE 0"  
        os.system(cmd)
        cmd = "caput FBCK:" + destIoc + ":" + feedback + ":SAVE 1"  
        os.system(cmd)
else:
    print "  -> " + selectedLoop + " ..."
    clone(selectedLoop, sourceIoc, destIoc)
    cmd = "caput FBCK:" + destIoc + ":" + selectedLoop + ":SAVE 0"  
    os.system(cmd)
    cmd = "caput FBCK:" + destIoc + ":" + selectedLoop + ":SAVE 1"  
    os.system(cmd)
