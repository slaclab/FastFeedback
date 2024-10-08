#!../../bin/linuxRT-x86_64/fastFeedback

# st.cmd file for Fast Feedback Controller IOC sioc-dog-tr01
# Note: contains parameters specific to sioc-dog-tr01 and
#       and call generic st.cmd common to all feedback loops
# author: L. Piccoli, A. Babbitt
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
# How to escape the "sioc-dog-tr01" as the PERL program
# will try to replace it.
# So, uncomment the following and remove the backslash

epicsEnvSet("EPICS_IOC_LOG_CLIENT_INET","${VIOC}")

#======================================================================
## iocAdmin environment variables
#=====================================================================
epicsEnvSet("ENGINEER","J.Mock")
epicsEnvSet("LOCATION","cpu-sys0-fb03")

#========================================================================
# Fast Feedback Application Specific Environment Variables
#========================================================================

#System Location:
epicsEnvSet("FB", "FB06")
epicsEnvSet("LOOP", "TR07")
epicsEnvSet("CONFIG_NAME", "DOG Launch")

# Which BY1 bend magnet do we want to read energy from?
epicsEnvSet("BEND_MAG", "REFS:COL0:950:EDES")

#=====================================================================
# Set MACROS for EVRs
#====================================================================
# FAC = SYS0 ==> LCLS1
# FAC = SYS1 ==> FACET

epicsEnvSet("LOCA","DOG")
epicsEnvSet(FAC,"${LOCA}")
epicsEnvSet(UNIT,"TR01") 
epicsEnvSet(EVR_DEV1,"EVR:${FAC}:${UNIT}")
epicsEnvSet(VEVR, "vevr6")

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
epicsEnvSet("IOCSH_PS1","epics@${VIOC}>")

# ===================================================================
# User defined environment vars
# 
# ===================================================================

# END: Additional Environment variables
# ===================================================================


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
