#!../../bin/linuxRT-x86_64/fastFeedback
# st.cmd file for Fast Feedback Controller IOC
# Note: contains parameters specific to sioc-unds-tr01 and
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
# How to escape the "sioc-unds-tr01" as the PERL program
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
epicsEnvSet("LOOP", "TR02")
epicsEnvSet("CONFIG_NAME", "LaunchLoop2")

# Which BY1 bend magnet do we want to read energy from?
epicsEnvSet("BEND_MAG", "BEND:LTUS:525:BDES")

#===================================================================
# Set MACROS for EVRs & VIOC
#===================================================================
# SYS0 ==> LCLS1
# SYS1 ==> FACET

epicsEnvSet("LOCA","UNDS")
epicsEnvSet(FAC,"${LOCA}")
epicsEnvSet(UNIT,"TR02") 
epicsEnvSet(EVR_DEV1,"EVR:${FAC}:${UNIT}")
epicsEnvSet(VEVR, "vevr12")

epicsEnvSet("IOC_TYPE", "SIOC")
epicsEnvSet("IOC_NAME",  "${IOC_TYPE}:${LOCA}:${UNIT}")
epicsEnvSet("LOCAL_SETPOINTS", "1")

# =================================================================
# User defined environment vars
# 
# =================================================================

# END: Additional Environment variables
#=====================================================================

# ====================================================================
## Load common fast feedback st.cmd
# ====================================================================
dbLoadDatabase("dbd/fastFeedback.dbd")
fastFeedback_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("db/fbckSettled.db",    "LP=FBCK:$(FB):$(LOOP)")
dbLoadRecords("db/fbckSettledSum.db", "LP=FBCK:$(FB):$(LOOP)")
dbLoadRecords("db/fbckAutoAct.db",    "LP=FBCK:$(FB):$(LOOP)")

<iocBoot/common/st.cmd

#Done  
