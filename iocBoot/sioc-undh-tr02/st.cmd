#!../../bin/linuxRT-x86_64/fastFeedback
# st.cmd file for Fast Feedback Controller IOC
# Note: contains parameters specific to vioc-und1-tr02 and
#       and calls generic st.cmd common to all feedback loops 
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
# How to escape the "sioc-undh-tr02" as the PERL program
# will try to repplace it.
# So, uncomment the following and remove the backslash

epicsEnvSet("EPICS_IOC_LOG_CLIENT_INET","${VIOC}")

#======================================================================
## iocAdmin environment variables
#=====================================================================
epicsEnvSet("ENGINEER","J.Mock")
epicsEnvSet("LOCATION","cpu-sys0-fb02")

#========================================================================
# Fast Feedback Application Specific Environment Variables
#========================================================================

#System Location:
epicsEnvSet("FB", "FB05")
epicsEnvSet("LOOP", "TR05")
epicsEnvSet("CONFIG_NAME", "LaunchLoop2")

# Which BY1 bend magnet do we want to read energy from?
epicsEnvSet("BEND_MAG", "BEND:LTUH:125:BDES")

#===================================================================
# Set MACROS for EVRs & VIOC
#===================================================================
# FAC = SYS0 ==> LCLS1
# FAC = SYS1 ==> FACET

epicsEnvSet("LOCA","UNDH")
epicsEnvSet(FAC,"${LOCA}")
epicsEnvSet(UNIT,"TR02") 
epicsEnvSet(EVR_DEV1,"EVR:${FAC}:${UNIT}")
epicsEnvSet(VEVR, "vevr5")

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

# =================================================================
# User defined environment vars
# 
# =================================================================

# END: Additional Environment variables
#======================================================================

# ====================================================================
## Load common fast feedback st.cmd
# ====================================================================
dbLoadDatabase("dbd/fastFeedback.dbd")
fastFeedback_registerRecordDeviceDriver(pdbbase)

dbLoadRecords("db/fbckSettled.db",    "LP=FBCK:$(FB):$(LOOP)")
dbLoadRecords("db/fbckSettledSum.db", "LP=FBCK:$(FB):$(LOOP)")
dbLoadRecords("db/fbckAutoAct.db",    "LP=FBCK:$(FB):$(LOOP)")

dbLoadRecords("db/bsaFbck.db",  "D=FBCK:UNDH:1, EG=mm,   HO=10, LO=-10, AD=5, PR=3, I='', LNK='', ATTR=X_POS, INP=STATE1, SINK_SIZE=1")
dbLoadRecords("db/bsaFbck.db",  "D=FBCK:UNDH:1, EG=mrad, HO=10, LO=-10, AD=5, PR=3, I='', LNK='', ATTR=X_ANG, INP=STATE2, SINK_SIZE=1")
dbLoadRecords("db/bsaFbck.db",  "D=FBCK:UNDH:1, EG=mm,   HO=10, LO=-10, AD=5, PR=3, I='', LNK='', ATTR=Y_POS, INP=STATE3, SINK_SIZE=1")
dbLoadRecords("db/bsaFbck.db",  "D=FBCK:UNDH:1, EG=mrad, HO=10, LO=-10, AD=5, PR=3, I='', LNK='', ATTR=Y_ANG, INP=STATE4, SINK_SIZE=1")

<iocBoot/common/st.cmd

#Done  
