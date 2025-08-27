#!../../bin/linuxRT-x86_64/ModeBsa
# st.cmd file for Fast Feedback Controller IOC
# Note: contains parameters specific to sioc-ltus-tr01 and
#       and calls generic st.cmd common to all feedback loops 
# author: L. Piccoli, A. Babbitt, R. Reno
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
# How to escape the "sioc-ltus-tr01" as the PERL program
# will try to repplace it.
# So, uncomment the following and remove the backslash

epicsEnvSet("EPICS_IOC_LOG_CLIENT_INET","${VIOC}")

#======================================================================
## iocAdmin environment variables
#=====================================================================
epicsEnvSet("ENGINEER","J.Mock")
epicsEnvSet("LOCATION","cpu-sys0-fb01")

#========================================================================
# Fast Feedback Application Specific Environment Variables
#========================================================================

#System Location:
epicsEnvSet("FB", "FB04")
epicsEnvSet("LOOP", "TR01")
epicsEnvSet("D", "FBCK:$(FB):$(LOOP)")
epicsEnvSet("CONFIG_NAME", "LaunchLoop1")

# Which BY1 bend magnet do we want to read energy from?
epicsEnvSet("BEND_MAG", "BEND:LTUS:525:BDES")

#===================================================================
# Set MACROS for EVRs & VIOC
#===================================================================
# SYS0 ==> LCLS1
# SYS1 ==> FACET

epicsEnvSet("LOCA","LTUS")
epicsEnvSet(FAC,"${LOCA}")
epicsEnvSet(UNIT,"TR01") 
epicsEnvSet(EVR_DEV1,"EVR:${FAC}:${UNIT}")
epicsEnvSet(VEVR, "vevr11")


epicsEnvSet("IOC_TYPE", "SIOC")
epicsEnvSet("IOC_NAME",  "${IOC_TYPE}:${LOCA}:${UNIT}")
epicsEnvSet("LOCAL_SETPOINTS", "1")
epicsEnvSet("IOCSH_PS1","epics@${VIOC}>")

# =================================================================
# User defined environment vars
# 
# =================================================================

# END: Additional Environment variables
#======================================================================
dbLoadDatabase("dbd/ModeBsa.dbd")
ModeBsa_registerRecordDeviceDriver(pdbbase)


dbLoadRecords("db/getter.db", "USER=lujko,PORT=lujko1,ADDR=0,TIMEOUT=0")

#iocBoot/common/st.cmd CODE STARTS HERE

# Environment varaible for logInit
epicsEnvSet("SUBSYS","fbck")
    
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

#########################################################################
#BEGIN: Load the record database
######################################################################

# ===================================================================
# load evr database
# ===================================================================
dbLoadRecords("db/EVR-TEMPLATE.db", "EVR=${EVR_DEV1},IOC=${IOC_NAME}") 

# ===================================================================
# load bsa database
# ===================================================================
dbLoadRecords("db/bsaFbck.db",  "D=BEAM:HXR, EG=mm,   HO=10, LO=-10, AD=5, PR=3, I='', LNK='', ATTR=X_POS, INP=HXR_CHANNEL, SINK_SIZE=1")
dbLoadRecords("db/bsaFbck.db",  "D=BEAM:SXR, EG=mm,   HO=10, LO=-10, AD=5, PR=3, I='', LNK='', ATTR=X_POS, INP=SXR_CHANNEL, SINK_SIZE=1")
    
######################################################################
#=======================================================================
# Setup Real-time priorities after starting the controller threads 
# driver thread
#=======================================================================
#cd iocBoot/${VIOC} 
system("rtPrioritySetup.cmd.evr ${VEVR}")

evrInitialize();

##Driver Launches
GetterDriverConfigure("lujko1")

iocInit

#Done  
