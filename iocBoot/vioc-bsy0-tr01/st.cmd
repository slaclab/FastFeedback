#
# st.cmd file for Fast Feedback Controller IOC
# author: L. Piccoli, A. Babbitt
# 
# modified:
#   Nov. 05, 2015 L. Piccoli - created
#   Nov. 16, 2015 A. Babbitt - Updated for EVR sharing and LinuxRT
#   A) Updated for LinuxRT real-time priority threads
#   B) Updated for EVR sharing - with older evr substitutions (IOC-SYS0-FB01.substitutions)
#   C) Updated save/restore and iocAdmin functionality (modernization) 
#   D) Updated event module databases, provided macro substitution, and generic files (in progress)
##########################################################

# Where am I?
pwd()
< envPaths

cd ${TOP} 

#######################################################################
#Setup environment variables
#######################################################################
#=======================================================================
#Setup some additional environment variables
#=======================================================================

# tag messages with IOC name
# How to escape the "vioc-b34-fb01" as the PERL program
# will try to repplace it.
# So, uncomment the following and remove the backslash

epicsEnvSet("EPICS_IOC_LOG_CLIENT_INET","${VIOC}")

#======================================================================
## iocAdmin environment variables
#=====================================================================
epicsEnvSet("ENGINEER","A.Babbitt")
epicsEnvSet("LOCATION","cpu-b34-fb01")

#=====================================================================
# Set MACROS for EVRs
#====================================================================
# FAC = SYS0 ==> LCLS1
# FAC = SYS1 ==> FACET

#System Location:
epicsEnvSet("LOCA","B34")

epicsEnvSet(EVR_DEV1,"EVR:B34:fb01")
epicsEnvSet(UNIT,"fb01")
epicsEnvSet(FAC,"SYS0")


#========================================================================
# Fast Feedback Application Specific Environment Variables
#========================================================================

epicsEnvSet("IOC_NAME",  "IOC:SYS0:FB01")
epicsEnvSet("FB", "FB01")
#epicsEnvSet("Loop", "TR01") #save for later  - looking for ways to isolate loop parameters near top of st.cmd 

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
epicsEnvSet("IOCSH_PS1","epics@vioc-b34-fb01>")

# =================================================================
# User defined environment vars
# 
# =================================================================

# END: Additional Environment variables
#======================================================================
######################################################################
## Register all support components
######################################################################
#=======================================================================
dbLoadDatabase("dbd/fastFeedback.dbd")
fastFeedback_registerRecordDeviceDriver(pdbbase)
    
######################################################################
#======================================================================    
# Initialize all Hardware 
#======================================================================    
# Init PMC EVR: To support Timing System Event Receiver
# In this case this EVR is running in a PC under linux
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


# ======= EVR Setup Complete ========================================

#########################################################################
#BEGIN: Load the record database
######################################################################

# ===================================================================
# load evr database
# ===================================================================
dbLoadRecords("db/IOC-SYS0-FB01.db") #Test and Delete - Trying to use macros
#dbLoadRecords("db/IOC-SYS0-FB01.db", "EVR=${EVR_DEV1},FBCK=${FB}") 

# ===================================================================
# Load iocAdmin record instances
# ===================================================================
dbLoadRecords("db/iocAdminSoft.db","IOC=${IOC_NAME}")
dbLoadRecords("db/iocRelease.db","IOC=${IOC_NAME}")
dbLoadRecords("db/iocAdminScanMon.db", "IOC=${IOC_NAME}"}

# ===================================================================
#Load database for autosave status
# ===================================================================
# this creates autosave status PVs named IOC:SYS0:FB01:xxx
# to report autosave status
# save_restoreSet_status_prefix("IOC:SYS0:FB01:") #Test & Delete
dbLoadRecords("db/save_restoreStatus.db", "P=${IOC_NAME}:")

# ===================================================================
# Load application specific databases
# ===================================================================
dbLoadRecords("db/fbckFB01.db")       #Test and Delete - Trying to use macros
#dbLoadRecords("db/fbckFB01.db","FBCK=${FB}")

#########################################################################
#BEGIN: Setup autosave/restore
######################################################################
## Setup autosave/restore will write to vioc-b34-fb01 directory
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
set_requestfile_path("/data/${VIOC}/autosave-req")

#======================================================================
#Where to write the save files that will be used to restore
#======================================================================
set_savefile_path("/data/${VIOC}/autosave")

# ==========================================================
# Prefix that is use to update save/restore status database
# records
# ==========================================================
save_restoreSet_UseStatusPVs(1) save_restoreSet_status_prefix("${IOC_NAME}:")
save_restoreSet_status_prefix("${IOC_NAME}:")
 
 ## Restore datasets
set_pass0_restoreFile("info_settings.sav")
set_pass1_restoreFile("info_settings.sav")


## Restore datasets - Specific to Fast Feedback
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
#=================================================================
#End: Setup autosave/restore
#=================================================================

###################################################################
#===================================================================
#create file of feedback loops
#===================================================================
iocshCmd("dbgrep 'FBCK:*:NAME' > /tmp/loops.txt")
###################################################################

#====================================================================
# Set-up Channel Access Security:
#====================================================================
# Channel Access Security:
# This is required if you use caPutLog.
# Set access security filea
# Load common LCLS Access Configuration File
 < ${ACF_INIT}
                                                                      
######################################################################
#========================================================================
#Start Fast Feedback Threads running
#========================================================================
# Start controller threads (Is EPICS iocInit built-in here?)
ffStart()

# =============================================================
# Start EPICS IOC Process (i.e. all threads will start running)
# =============================================================
#iocInit()
#####################################################################
# =======================================================================
# Turn on caPutLogging:
# =======================================================================    
# Log values only on change to the iocLogServer:
 caPutLogInit("${EPICS_CA_PUT_LOG_ADDR}")
 caPutLogShow(2)
# ======================================================================
    
######################################################################
#=======================================================================
# Setup Real-time priorities after starting the controller threads 
# driver thread
#=======================================================================
cd iocBoot/${VIOC} 
system("/bin/su root -c `pwd`/rtPrioritySetup.vevr0.cmd")

########################################################################
#========================================================================
# Start autosave routines to save data
#========================================================================

#########################################################################
## Start saving datasets
cd("/data/${VIOC}/autosave-req")

cd("/data/${VIOC}/autosave-req")
iocshCmd("makeAutosaveFiles")
 
# Start the save_restore task
# save changes on change, but no faster
# than every 5 seconds.
# Note: the last arg cannot be set to 0
create_monitor_set("info_positions.req", 5 )
create_monitor_set("info_settings.req" , 30 )

# Start saving application specific datasets

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
