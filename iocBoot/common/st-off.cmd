#
# generic st.cmd file for all Fast Feedback Controller vioc's
# author: L. Piccoli, A. Babbitt
##########################################################

#======================================================================
######################################################################
## Register all support components
######################################################################
#=======================================================================
#dbLoadDatabase("dbd/fastFeedback.dbd")
#fastFeedback_registerRecordDeviceDriver(pdbbase)
    
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
# Each feedback needs the LOCA_NAME macro for LoopConfiguration.cc
epicsEnvSet("LOCA_NAME","${FB}")
epicsEnvSet("LOCA2_NAME", "${FB}", 1)
#
# Initialize FCOM
epicsEnvSet("FCOM_MC_PREFIX", "239.219.8.0", 1)
fcomInit( ${FCOM_MC_PREFIX}, 1000 )


#########################################################################
#BEGIN: Load the record database
######################################################################

# ===================================================================
# load evr database
# ===================================================================
# EVR database cannot be loaded, there will be conflicts with the VME
# EVR PVs.
# dbLoadRecords("db/EVR-TEMPLATE.db", "EVR=${EVR_DEV1},IOC=${IOC_NAME}") 

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
#dbLoadRecords("db/save_restoreStatus.db", "P=${IOC_NAME}:")

# ===================================================================
# Load application specific databases
# ===================================================================
# These macros are used in PV generation:
#dbLoadRecords("db/fbck_template.db","VIOC=${IOC_TYPE},LOC=${LOCA},FEEDBACK=${FB},LOOP_TYPE=${LOOP},CON_NAME=${CONFIG_NAME},LOCAL_SETPOINTS=${LOCAL_SETPOINTS}")
#########################################################################
#BEGIN: Setup autosave/restore
######################################################################
## Setup autosave/restore will write to vioc directory
#======================================================================
#If all PVs don't connect continue anyways
# save restore setup: incomplete restore ok, backup file overwrites
#======================================================================
#save_restoreSet_IncompleteSetsOk(1)

#======================================================================
# Created save/restore backup files with data string 
# useful for recovery.
#======================================================================
#save_restoreSet_DatedBackupFiles(0)
    
#======================================================================
#Where to find the list of PVs to save
#======================================================================
#set_requestfile_path("/data/${VIOC}/autosave-req")

#======================================================================
#Where to write the save files that will be used to restore
#======================================================================
#set_savefile_path("/data/${VIOC}/autosave")

# ==========================================================
# Prefix that is use to update save/restore status database
# records
# ==========================================================
#save_restoreSet_UseStatusPVs(1) save_restoreSet_status_prefix("${IOC_NAME}:")
#save_restoreSet_status_prefix("${IOC_NAME}:")
 
 ## Restore datasets
#set_pass0_restoreFile("info_settings.sav")
#set_pass1_restoreFile("info_settings.sav")


## Restore datasets - Specific to Fast Feedback
#set_pass0_restoreFile("info_mon_${FB}.sav")
#set_pass1_restoreFile("info_mon_${FB}.sav")

#set_pass0_restoreFile("info_config_${LOOP}.sav")
#set_pass1_restoreFile("info_config_${LOOP}.sav")

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

#Debugging Flags 
#ffll 0 5                                                                      
#ffll 1 5                                                                      
#ffll 2 5                                                                      
#ffll 3 5                                                                      
#ffll 4 5
#ffll 5 5 
#ffll 6 5

######################################################################
#========================================================================
#Start Fast Feedback Threads running
#========================================================================
# Start controller threads (Is EPICS iocInit built-in here?)
#ffStart()

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
#iocshCmd("makeAutosaveFiles")
 
# Start the save_restore task
# save changes on change, but no faster
# than every 5 seconds.
# Note: the last arg cannot be set to 0
#create_monitor_set("info_positions.req", 5 )
#create_monitor_set("info_settings.req" , 30 )

# Start saving application specific datasets

#makeAutosaveFileFromDbInfo("info_mon_${FB}.req", "autosaveMon${FB}")
#create_monitor_set("info_mon_${FB}.req",60,"")

#makeAutosaveFileFromDbInfo("info_config_${LOOP}.req", "autosaveConfig${LOOP}")
#create_triggered_set("info_config_${LOOP}.req","FBCK:${FB}:${LOOP}:SAVE","")

#Done
