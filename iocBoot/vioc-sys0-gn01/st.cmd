#
# st.cmd file for Fast Feedback Controller IOC vioc-sys0-gn01
# Note: contains parameters specific to vioc-sys0-gn01 and
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
# How to escape the "vioc-sys0-gn01" as the PERL program
# will try to replace it.
# So, uncomment the following and remove the backslash

epicsEnvSet("EPICS_IOC_LOG_CLIENT_INET","${VIOC}")

#======================================================================
## iocAdmin environment variables
#=====================================================================
epicsEnvSet("ENGINEER","A.Babbitt")
epicsEnvSet("LOCATION","cpu-sys0-fb02")

#========================================================================
# Fast Feedback Application Specific Environment Variables
#========================================================================

#System Location:
epicsEnvSet("LOCA","SYS0")
epicsEnvSet("FB", "${LOCA}")
epicsEnvSet("LOOP", "GN01")
epicsEnvSet("CONFIG_NAME", "General1")
epicsEnvSet("IOC_TYPE", "VIOC")
epicsEnvSet("IOC_NAME",  "${IOC_TYPE}:${LOCA}:${LOOP}")
epicsEnvSet("LOCAL_SETPOINTS", "1")

#=====================================================================
# Set MACROS for EVRs
#====================================================================
# FAC = SYS0 ==> LCLS1
# FAC = SYS1 ==> FACET

epicsEnvSet(FAC,"SYS0")
epicsEnvSet(UNIT,"FB02") 
epicsEnvSet(EVR_DEV2,"EVR:${FAC}:${UNIT}")
epicsEnvSet(VEVR, "vevr2")

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
<iocBoot/common/st.cmd

#Done