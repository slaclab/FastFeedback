#!../../bin/linuxRT-x86_64/fastFeedback
#
# st.cmd file for Fast Feedback Controller IOC
# Note: contains parameters specific to vioc-sys0-lg01 and
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
# How to escape the "sioc-sys0-lg02" as the PERL program
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
epicsEnvSet("FB", "FB01")
epicsEnvSet("LOOP", "LG01")
epicsEnvSet("CONFIG_NAME", "LaunchLoop2")

# Which BY1 bend magnet do we want to read energy from?
epicsEnvSet("BEND_MAG", "BEND:LTUS:525:BDES")

#=====================================================================
# Set MACROS for EVRs & VIOC
#====================================================================
# FAC = SYS0 ==> LCLS1
# FAC = SYS1 ==> FACET

epicsEnvSet("LOCA","SYS0")
epicsEnvSet(FAC,"${LOCA}")
epicsEnvSet(UNIT,"LG02")
epicsEnvSet(EVR_DEV1,"EVR:${FAC}:${UNIT}")
epicsEnvSet(VEVR, "vevr10")

epicsEnvSet("IOC_TYPE","SIOC")
epicsEnvSet("IOC_NAME",  "${IOC_TYPE}:${LOCA}:${UNIT}")
#This parameter must be set to 0 for Longitudinal Feedback loops and 1 
# for all other types of feedback loops
epicsEnvSet("LOCAL_SETPOINTS", "0")
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
#=====================================================================

# ====================================================================
## Load common fast feedback st.cmd
# ====================================================================
dbLoadDatabase("dbd/fastFeedback.dbd")
fastFeedback_registerRecordDeviceDriver(pdbbase)
dbLoadRecords("db/bsaFbck.db",  "D=FBCK:UND0:1, EG=mm,   HO=10, LO=-10, AD=5, PR=3, I='', LNK='', ATTR=X_POS, INP=STATE1, SINK_SIZE=1")
dbLoadRecords("db/bsaFbck.db",  "D=FBCK:UND0:1, EG=mrad, HO=10, LO=-10, AD=5, PR=3, I='', LNK='', ATTR=X_ANG, INP=STATE2, SINK_SIZE=1")
dbLoadRecords("db/bsaFbck.db",  "D=FBCK:UND0:1, EG=mm,   HO=10, LO=-10, AD=5, PR=3, I='', LNK='', ATTR=Y_POS, INP=STATE3, SINK_SIZE=1")
dbLoadRecords("db/bsaFbck.db",  "D=FBCK:UND0:1, EG=mrad, HO=10, LO=-10, AD=5, PR=3, I='', LNK='', ATTR=Y_ANG, INP=STATE4, SINK_SIZE=1")

# Loading a record type that will differentiate which longitudinal controls
# are being used. This helps to write to the correct PVs for the different
# Feedbacks. This one is for SXR (Type 1):
dbLoadRecords("db/fbckLongType.db", "AREA=$(FB), LOOP=$(LOOP), LG_TYPE=1")

<iocBoot/common/st.cmd

#Done  
