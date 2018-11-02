#!../../bin/linux-x86_64/fastFeedback

## You may have to change fastFeedback to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

#######################################################################
#Setup environment variables
#######################################################################
#=======================================================================
#Setup some additional environment variables
#=======================================================================

# tag messages with IOC name
# How to escape the "sioc-sys0-fb01" as the PERL program
# will try to replace it.
# So, uncomment the following and remove the backslash

epicsEnvSet("EPICS_IOC_LOG_CLIENT_INET","${IOC}")

#======================================================================
## iocAdmin environment variables
#=====================================================================
epicsEnvSet("ENGINEER","J. Luong")
epicsEnvSet("LOCATION","cpu-sys0-fb01")


#=====================================================================
# Set MACROS for EVRs & VIOC
#====================================================================
# FAC = SYS0 ==> LCLS1
# FAC = SYS1 ==> FACET

epicsEnvSet("LOCA","SYS0") 
epicsEnvSet("UNIT", "FB01")

epicsEnvSet("IOC_TYPE", "SIOC")
epicsEnvSet("IOC_NAME",  "${IOC_TYPE}:${LOCA}:${UNIT}")
epicsEnvSet("LOCAL_SETPOINTS", "1")
# ========================================================
# Support Large Arrays/Waveforms; Number in Bytes
# Please calculate the size of the largest waveform
# that you support in your IOC.  Do not just copy numbers
# from other apps.  This will only lead to an exhaustion
# of resources and problems with your IOC.
# The default maximum size for a channel access array is
# 16K bytes.
# ========================================================

epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","32000") #Support Large Arrays/Waveforms
epicsEnvSet("IOCSH_PS1","epics@${IOC}>")

# ===================================================================
# User defined environment vars
# 
# ===================================================================

# END: Additional Environment variables
# ===================================================================




## Register all support components
dbLoadDatabase "dbd/fastFeedback.dbd"
fastFeedback_registerRecordDeviceDriver pdbbase

## Load record instances


# ===================================================================
# Load iocAdmin record instances
# ===================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=${IOC_NAME}")
dbLoadRecords("db/iocRelease.db","IOC=${IOC_NAME}")
dbLoadRecords("db/iocAdminScanMon.db", "IOC=${IOC_NAME}")

# ===================================================================
#Load database for autosave status
# ===================================================================
# this creates autosave status PVs named IOC:SYS0:FB01:xxx
# to report autosave status
dbLoadRecords("db/save_restoreStatus.db", "P=${IOC_NAME}:")

# ===================================================================
# Load application specific databases
# ===================================================================
# These macros are used in PV generation:
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB01:TR01")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB01:TR02")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB01:TR03")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB01:TR04")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB01:TR05")

dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB02:TR01")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB02:TR02")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB02:TR03")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB02:TR04")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB02:TR05")

dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB03:TR01")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB03:TR02")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB03:TR03")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB03:TR04")

dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB04:LG01")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB02:GN01")

dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB05:TR01")
dbLoadRecords("db/fbckSettled.db","LP=FBCK:FB05:TR05")

### Convergence status records
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB01:TR01")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB01:TR02")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB01:TR03")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB01:TR04")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB01:TR05")

dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB02:TR01")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB02:TR02")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB02:TR03")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB02:TR04")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB02:TR05")

dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB03:TR01")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB03:TR02")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB03:TR03")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB03:TR04")

dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB04:LG01")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB02:GN01")

dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB05:TR01")
dbLoadRecords("db/fbckSettledSum.db","LP=FBCK:FB05:TR05")

#########################################################################
#BEGIN: Setup autosave/restore
######################################################################
## Setup autosave/restore will write to vioc directory
#======================================================================
#If all PVs don't connect continue anyways
# save restore setup: incomplete restore ok, backup file overwrites
#======================================================================
save_restoreSet_IncompleteSetsOk(1)

#======================================================================
# Created save/restore backup files with data string 
# useful for recovery.
#======================================================================
save_restoreSet_DatedBackupFiles(0)
    
#======================================================================
#Where to find the list of PVs to save
#======================================================================
set_requestfile_path("${IOC_DATA}/${IOC}/autosave-req")

#======================================================================
#Where to write the save files that will be used to restore
#======================================================================
set_savefile_path("${IOC_DATA}/${IOC}/autosave")

# ==========================================================
# Prefix that is use to update save/restore status database
# records
# ==========================================================
save_restoreSet_UseStatusPVs(1) save_restoreSet_status_prefix("${IOC_NAME}:")
save_restoreSet_status_prefix("${IOC_NAME}:")
 
 ## Restore datasets
set_pass0_restoreFile("info_settings.sav")
set_pass1_restoreFile("info_settings.sav")

set_pass0_restoreFile("info_positions.sav")
set_pass1_restoreFile("info_positions.sav")


## Note that autosave/restore spews a lot of messages and message
## throttling throttles all of other messages below.
#=================================================================
#End: Setup autosave/restore
#=================================================================



#====================================================================
# Set-up Channel Access Security:
#====================================================================
# Channel Access Security:
# This is required if you use caPutLog.
# Set access security filea
# Load common LCLS Access Configuration File
 < ${ACF_INIT}




# =============================================================
# Start EPICS IOC Process (i.e. all threads will start running)
# =============================================================
iocInit()
#####################################################################
# =======================================================================
# Turn on caPutLogging:
# =======================================================================    
# Log values only on change to the iocLogServer:
 caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")
 caPutLogShow(2)
# ======================================================================
    


########################################################################
#========================================================================
# Start autosave routines to save data
#========================================================================

#########################################################################
## Start saving datasets
cd("${IOC_DATA}/${IOC}/autosave-req")
iocshCmd("makeAutosaveFiles")
 
# Start the save_restore task
# save changes on change, but no faster
# than every 5 seconds.
# Note: the last arg cannot be set to 0
create_monitor_set("info_positions.req", 5 )
create_monitor_set("info_settings.req" , 30 )
