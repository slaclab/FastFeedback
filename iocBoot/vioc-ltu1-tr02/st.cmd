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

#   Nov. 20th, 2015 A. Babbitt - Generated new vioc-b34-fb02 to support the following:
#   A) Set-up the 2nd vevr1
#   B) Set-up linuxRT to run two vioc's
#   C) Clean-up st.cmd to use macros for vevr's, vioc's, and feedbacks (generalize/modular)
#   D) Modernize the evr templates => pass in macros versus IOC:SYS:FB03.db#   E) Create generic fbckFB03.db files and pass in macros
#   E) Create generic fbckFB03.db files and pass in macros

#   Nov. 30, 2015 A. Babbitt - Loop Splitting 
#   A) Using generic template files - define loop parameters with macros
#   B) Using macro for virtual EVR  - goal is to just change environment variables (generic st.cmd)
#   C) Test new template files - update template file names
#   D) Clean-up the auto/save restore (just one loop now per st.cmd)
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
# How to escape the "vioc-b34-fb05" as the PERL program
# will try to repplace it.
# So, uncomment the following and remove the backslash

epicsEnvSet("EPICS_IOC_LOG_CLIENT_INET","${VIOC}")

#======================================================================
## iocAdmin environment variables
#=====================================================================
epicsEnvSet("ENGINEER","A.Babbitt")
epicsEnvSet("LOCATION","cpu-b34-fb01")

#========================================================================
# Fast Feedback Application Specific Environment Variables
#========================================================================

epicsEnvSet("FB", "FB01")
epicsEnvSet("IOC_NAME",  "IOC:SYS0:${FB}")
epicsEnvSet("LOOP", "TR03")
epicsEnvSet("CONFIG_NAME", "LaunchLoop3")

#=====================================================================
# Set MACROS for EVRs
#====================================================================
# FAC = SYS0 ==> LCLS1
# FAC = SYS1 ==> FACET

#System Location:
epicsEnvSet("LOCA","B34")

epicsEnvSet(EVR_DEV1,"EVR:SYS0:${FB}")
#epicsEnvSet(UNIT,"fb01")
epicsEnvSet(FAC,"SYS0")
epicsEnvSet(VEVR, "vevr4")

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
epicsEnvSet("IOCSH_PS1","epics@${VIOC}>")

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
eevrmaConfigure(0, "/dev/${VEVR}")


# ======= EVR Setup Complete ========================================

#########################################################################
#BEGIN: Load the record database
######################################################################

# ===================================================================
# load evr database
# ===================================================================
dbLoadRecords("db/EVR-TEMPLATE.db", "EVR=${EVR_DEV1},IOC=${IOC_NAME}") 

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
dbLoadRecords("db/fbck_template.db","FB=${FB},LOOP=${LOOP},CONFIG_NAME=${CONFIG_NAME}")
#If configuring a longitudal feedback, need to uncomment the following line
#dbLoadRecords("db/long_template.db")

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
set_pass0_restoreFile("info_mon_${FB}.sav")
set_pass1_restoreFile("info_mon_${FB}.sav")

set_pass0_restoreFile("info_config_${LOOP}.sav")
set_pass1_restoreFile("info_config_${LOOP}.sav")

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
system("/bin/su root -c `pwd`/rtPrioritySetup.${VEVR}.cmd")

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

makeAutosaveFileFromDbInfo("info_mon_${FB}.req", "autosaveMon${FB}")
create_monitor_set("info_mon_${FB}.req",60,"")

makeAutosaveFileFromDbInfo("info_config_${LOOP}.req", "autosaveConfig${LOOP}")
create_triggered_set("info_config_${LOOP}.req","FBCK:${FB}:${LOOP}:SAVE","")

#Done
