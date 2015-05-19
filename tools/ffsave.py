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

def saveArrayPv(source, file):
    cmd = "caget -w 60 " + source + " | xargs -iX echo \"caput -a X\" >> " + file
    os.system(cmd)

def savePv(source, file):
    cmd = "caget -w 60 " + source + " | xargs -iX echo \"caput X\" >> " + file
    os.system(cmd)

def saveDevices(feedback, sourceIoc, file, count, type, nameField):
    for i in range(count):
        savePv("FBCK:" + sourceIoc + ":" + feedback + ":" + type + str(i+1) + nameField, file)

    for i in range(count):
        savePv("FBCK:" + sourceIoc + ":" + feedback + ":" + type + str(i+1) + "USED", file)

    if (type != "S"):
        for i in range(count):
            savePv("FBCK:" + sourceIoc + ":" + feedback + ":" + type + str(i+1) + "CAMODE", file)

def save(feedback, sourceIoc, file):
    cmd = "echo \"#!/bin/sh\" > " + file
    os.system(cmd)
    cmd = "echo \"# === Feedback Configuration for " + sourceIoc + ":" + feedback + " ===\" >> " + file
    os.system(cmd)
    
    for pvProperty in pvList:
        savePv("FBCK:" + sourceIoc + ":" + feedback + ":" + pvProperty, file)
        
    saveDevices(feedback, sourceIoc, file, 20, "M", "DEVNAME")
    saveDevices(feedback, sourceIoc, file, 10, "A", "DEVNAME")
    saveDevices(feedback, sourceIoc, file, 10, "S", "NAME")

    saveDevices(feedback, sourceIoc, file, 20, "M", "HIHI")
    saveDevices(feedback, sourceIoc, file, 20, "M", "LOLO")
    saveDevices(feedback, sourceIoc, file, 10, "A", "HIHI")
    saveDevices(feedback, sourceIoc, file, 10, "A", "LOLO")
    saveDevices(feedback, sourceIoc, file, 10, "S", "HIHI")
    saveDevices(feedback, sourceIoc, file, 10, "S", "LOLO")

    for pvProperty in pvArrayList:
        saveArrayPv("FBCK:" + sourceIoc + ":" + feedback + ":" + pvProperty, file)

    cmd = "echo \"echo === Restored configuration for " + sourceIoc + ":" + feedback + " ===\" >> " + file
    os.system(cmd)

    cmd = "chmod a+x " + file
    os.system(cmd)


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

if (len(sys.argv) == 4):
    sourceIoc = checkIoc(sys.argv[1])
    sourceLoop = checkLoop(sys.argv[2])
    fileName = sys.argv[3]
else:
    print "Usage: ffsave.py [source] [loop] [save-to-file]"
    print "  This command saves the configuration PVs of the"
    print "  feedback installed on [source][loop] to the"
    print "  [save-to-file] file."
    print ""
    print "  The [save-to-file] is a shell script which "
    print "  if executed will restore the saved PVs using"
    print "  caput."
    print ""
    print "  Example: ffsave.py FB04 LG01 Longitudinal.conf"
    print "  Example: ffsave.py FB02 TR04 UndulatorLaunch.conf"
    print ""
    print "  In order to restore the PVs to a different IOC/Loop"
    print "  please replace the string in the saved file before"
    print "  running it."
    print ""
    print "  To configure FB05:TR02 with FB02:TR04 configuration"
    print "  do the following steps:"
    print ""
    print "  $ ffsave.py FB02 TR04 UndulatorLaunch.conf"
    print "  Saving configuration of FB02:TR04 -> UndulatorLaunch.conf"
    print "  ... <there may be some epicsThreadOnceOsd messages here> ..."
    print "  $ sed 's/FB02/FB05/' UndulatorLaunch.conf | sed 's/TR04/TR02/' > UndulatorLaunch-FB05:TR02.conf"
    print "  $ chmod a+x UndulatorLaunch-FB05:TR02.conf"
    print "  $ ./UndulatorLaunch-FB05:TR02.conf"
    exit(-1)

print "Saving configuration of " + sourceIoc + ":" + sourceLoop + " -> " + fileName

save(sourceLoop, sourceIoc, fileName)
