#
# st.cmd file for Fast Feedback Controller IOC
# author: L. Piccoli, A. Babbitt
# 
# modified:
#   Nov. 05, 2015 L. Piccoli - created
#   Nov. 16, 2015 A. Babbitt - Updated for EVR sharing and LinuxRT
#   A) Updated for LinuxRT/real-time priority threads
#   B) Updated for EVR sharing
#   C) Updated to use generic templates and macro substitutions
##########################################################

# Where am I?
pwd()
< envPaths

cd ${TOP}

#=======================================================================
#Setup some additional environment variables
#=======================================================================
#Setup environment variables

epicsEnvSet("IOC", "vioc-b34-fb01")
epicsEnvSet("IOC_NAME",  "IOC:SYS0:FB01")
epicsEnvSet("FB", "FB01")
#epicsEnvSet("Loop", "TR01") #save for later  - looking for ways to isolate loop parameters near top of st.cmd 
epicsEnvSet(TOP,"${IOC_APP}")

epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","32000") #Support Large Arrays/Waveforms
epicsEnvSet("IOCSH_PS1","epics@vioc-b34-fb01>")

# bspExtMemProbe only durint init. clear this to avoid the lecture.
bspExtVerbosity=0


# tag messages with IOC name
# How to escape the "vioc-b34-fb01" as the PERL program
# will try to repplace it.
# So, uncomment the following and remove the backslash

epicsEnvSet("EPICS_IOC_LOG_CLIENT_INET","${IOC}")


# =================================================================
# User defined environment vars
# 
# =================================================================
###epicsEnvSet("LOCA_NAME","FB01",1)     # Slate for deletion
###epicsEnvSet("IOC_NAME","FB01",1)      # Slate for deletion
###epicsEnvSet("LOCA2_NAME", "FB01", 1)  # Slate for deletion
 
## iocAdmin
epicsEnvSet("ENGINEER","A.Babbitt")
epicsEnvSet("LOCATION","cpu-b34-fb01")
###epicsEnvSet("STARTUP","/usr/local/lcls/epics/iocCommon/vioc-b34-fb01") # Slate for deletion
###epicsEnvSet("ST_CMD","startup.cmd")  # Slate for deletion
 
#fcomInit(fcomUtilGethostbyname(getenv("FCOMMCGRP"),0),1000)


# Set MACROS for EVRs=================================================
# FAC = SYS0 ==> LCLS1
# FAC = SYS1 ==> FACET

#System Location:
epicsEnvSet("LOCA","B34")

epicsEnvSet(EVR_DEV1,"EVR:B34:fb01")
epicsEnvSet(UNIT,"fb01")
epicsEnvSet(FAC,"SYS0")


#=======================================================================
## Register all support components
#=======================================================================
dbLoadDatabase("dbd/fastFeedback.dbd")
fastFeedback_registerRecordDeviceDriver(pdbbase)

#======================================================================    
# Initialize all Hardware =============================================
#======================================================================    
# Init PMC EVR: To support Timing System Event Receiver
# In this case this EVR is running in a PC under linux

#system("/bin/su root -c $IOC_BOOT/suSetupVevrs.cmd")
### wait the system kernel comes up
#epicsThreadSleep(2)

# Setup for EVR:========================================================

# Add eevrmaConfigure for each VEVR before iocInit.
#
#    eevrmaConfigure <instance> <vevrDevName>
#
#    where instance = EVR instance, starting from 0, incrementing by
#                     for each subsequent card.  Only 1 EVR instance
#                    is allowed for Embedded EVRs.
#
# ===================================================================
# Debug interest level for EVR Driver
# ErDebug=100
            
# Here we have an VEVR
eevrmaConfigure(0, "/dev/vevr0")

# Add evrInitialize (after ErConfigure) if a fiducial routine will be
# registered before iocInit driver initialization:
#evrInitialize()

# ======= EVR Setup Complete ========================================


#=========================================================================    
#BEGIN: Load the record database
#=========================================================================

## Load iocAdmin record instances
dbLoadRecords("db/iocAdminSoft.db","IOC=${IOC_NAME}")
dbLoadRecords("db/iocRelease.db","IOC=${IOC_NAME}")

# load evr database
dbLoadRecords("db/IOC-SYS0-FB01.template") #Test and Delete - Trying to use macros
#dbLoadRecords("db/IOC-SYS0-FB01.template", "EVR=${EVR_DEV1},FBCK=${FB}") 

# Load application
dbLoadRecords("db/fbckFB01.template")       #Test and Delete - Trying to use macros
#dbLoadRecords("db/fbckFB01.template","FBCK=${FB}")

## Setup autosave/restore will write to cpu-b34-fb01 directory
set_requestfile_path("/data/autosave-req")
set_savefile_path("/data/autosave")

# this creates autosave status PVs named IOC:SYS0:FB01:xxx
# to report autosave status
save_restoreSet_status_prefix("IOC:SYS0:FB01:")
dbLoadRecords("db/save_restoreStatus.db", "P=${IOC_NAME}:")

# save restore setup: incomplete restore ok, backup file overwrites
save_restoreSet_IncompleteSetsOk(1)
save_restoreSet_DatedBackupFiles(0)


# evr hardware initialization
drvMrfErFlag=0
ErConfigure(0, 0, 0, 0, 1)
evrInitialize()


## Restore datasets
set_pass0_restoreFile("info_mon_FB01.sav")
set_pass1_restoreFile("info_mon_FB01.sav")

set_pass0_restoreFile("info_config_TR01.sav")
set_pass1_restoreFile("info_config_TR01.sav")
set_pass0_restoreFile("info_config_TR02.sav")
set_pass1_restoreFile("info_config_TR02.sav")
set_pass0_restoreFile("info_config_TR03.sav")
set_pass1_restoreFile("info_config_TR03.sav")
set_pass0_restoreFile("info_config_TR04.sav")
set_pass1_restoreFile("info_config_TR04.sav")
set_pass0_restoreFile("info_config_TR05.sav")
set_pass1_restoreFile("info_config_TR05.sav")
set_pass0_restoreFile("info_config_LG01.sav")
set_pass1_restoreFile("info_config_LG01.sav")
set_pass0_restoreFile("info_config_GN01.sav")
set_pass1_restoreFile("info_config_GN01.sav")
#set_pass0_restoreFile("info_config_GN02.sav")
#set_pass1_restoreFile("info_config_GN02.sav")

## Note that autosave/restore spews a lot of messages and message
## throttling throttles all of other messages below.

#create file of feedback loops
iocshCmd("dbgrep 'FBCK:*:NAME' > /tmp/loops.txt")


#====================================================================
# Set-up Channel Access Security:
#====================================================================
# Channel Access Security:
# This is required if you use caPutLog.
# Set access security filea
# Load common LCLS Access Configuration File
 < ${ACF_INIT}
                                                                      

#========================================================================
#Start EPICS IOC Process (i.e. all threads will start running)
#========================================================================

# Start controller threads
ffStart()
#iocInit()

# =======================================================================
# Turn on caPutLogging:
# =======================================================================    
# Log values only on change to the iocLogServer:
 caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")
 caPutLogShow(2)
# ======================================================================
    

#=======================================================================
# Setup Real-time priorities after starting the controller threads 
# driver thread
#=======================================================================
cd ${IOC_BOOT}
system("/bin/su root -c `pwd`/rtPrioritySetup.vevr0.cmd")


#========================================================================
# Start autosave routines to save data
#========================================================================

## Start saving datasets
cd("/data/${IOC}/autosave-req")

makeAutosaveFileFromDbInfo("info_mon_FB01.req", "autosaveMonFB01")
create_monitor_set("info_mon_FB01.req",60,"")

makeAutosaveFileFromDbInfo("info_config_TR01.req", "autosaveConfigTR01")
create_triggered_set("info_config_TR01.req","FBCK:FB01:TR01:SAVE","")

makeAutosaveFileFromDbInfo("info_config_TR02.req", "autosaveConfigTR02")
create_triggered_set("info_config_TR02.req","FBCK:FB01:TR02:SAVE","")

makeAutosaveFileFromDbInfo("info_config_TR03.req", "autosaveConfigTR03")
create_triggered_set("info_config_TR03.req","FBCK:FB01:TR03:SAVE","")

makeAutosaveFileFromDbInfo("info_config_TR04.req", "autosaveConfigTR04")
create_triggered_set("info_config_TR04.req","FBCK:FB01:TR04:SAVE","")

makeAutosaveFileFromDbInfo("info_config_TR05.req", "autosaveConfigTR05")
create_triggered_set("info_config_TR05.req","FBCK:FB01:TR05:SAVE","")

makeAutosaveFileFromDbInfo("info_config_LG01.req", "autosaveConfigLG01")
create_triggered_set("info_config_LG01.req","FBCK:FB01:LG01:SAVE","")

makeAutosaveFileFromDbInfo("info_config_GN01.req", "autosaveConfigGN01")
create_triggered_set("info_config_GN01.req","FBCK:FB01:GN01:SAVE","")

#makeAutosaveFileFromDbInfo("info_config_GN02.req", "autosaveConfigGN02")
#create_triggered_set("info_config_GN02.req","FBCK:FB01:GN02:SAVE","")

#Done
