#
# st.cmd file for Fast Feedback Controller IOC
# Note: contains parameters specific to vioc-ltu1-tr02 and
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
# How to escape the "vioc-ltu1-tr02" as the PERL program
# will try to repplace it.
# So, uncomment the following and remove the backslash

epicsEnvSet("EPICS_IOC_LOG_CLIENT_INET","${VIOC}")

#======================================================================
## iocAdmin environment variables
#=====================================================================
epicsEnvSet("ENGINEER","A.Babbitt")
epicsEnvSet("LOCATION","cpu-sys0-fb01")

#========================================================================
# Fast Feedback Application Specific Environment Variables
#========================================================================

#System Location:
epicsEnvSet("FB", "FB02")
epicsEnvSet("LOOP", "TR04")
epicsEnvSet("CONFIG_NAME", "LaunchLoop2")

#===================================================================
# Set MACROS for EVRs & VIOC
#===================================================================
# FAC = SYS0 ==> LCLS1
# FAC = SYS1 ==> FACET

epicsEnvSet("LOCA","LTU1")
epicsEnvSet(FAC,"${LOCA}")
epicsEnvSet(UNIT,"TR02") 
epicsEnvSet(EVR_DEV1,"EVR:${FAC}:${UNIT}")
epicsEnvSet(VEVR, "vevr6")

epicsEnvSet("IOC_TYPE", "VIOC")
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
<iocBoot/common/st.cmd

#Done  
