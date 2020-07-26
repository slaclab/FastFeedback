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
epicsEnvSet("CONFIG_NAME", "Longitudinal")

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
dbLoadRecords("db/iocEnergyChirp.db","DEV=FBCK:LI22:2,RF=L2,PHASE_LNK=ACCL:LI22:1:PDES,AMPL_LNK=ACCL:LI22:1:ADES,DRVH=6000")
dbLoadRecords("db/iocEnergyChirp.db","DEV=FBCK:LI25:2,RF=L3,PHASE_LNK=ACCL:LI25:1:PDES,AMPL_LNK=ACCL:LI25:1:ADES,DRVH=16000")
<iocBoot/common/st.cmd

# ====================================================================
# Sequencer scripts to keep track of the CHIRP control and DL2 limits
# ====================================================================
seq(&chirpControl, "IOC=FB01,LOOP=LG01")
seq(&chirpUpdate, "IOC=FB01,LOOP=LG01")
seq(&limitUpdate, "IOC=FB01,LOOP=LG01")

seqShow()

#Done  