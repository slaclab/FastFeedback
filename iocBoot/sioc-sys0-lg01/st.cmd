#!../../bin/linuxRT-x86_64/fastFeedback

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
# How to escape the "sioc-sys0-lg01" as the PERL program
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
epicsEnvSet("FB", "FB04")
epicsEnvSet("LOOP", "LG01")
epicsEnvSet("CONFIG_NAME", "Longitudinal")

# Which BY1 bend magnet do we want to read energy from?
epicsEnvSet("BEND_MAG", "BEND:LTUH:125:BDES")

#=====================================================================
# Set MACROS for EVRs & VIOC
#====================================================================
# FAC = SYS0 ==> LCLS1
# FAC = SYS1 ==> FACET

epicsEnvSet("LOCA","SYS0")
epicsEnvSet(FAC,"${LOCA}")
epicsEnvSet(UNIT,"LG01")
epicsEnvSet(EVR_DEV1,"EVR:${FAC}:${UNIT}")
epicsEnvSet(VEVR, "vevr3")

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
dbLoadRecords("db/iocEnergyChirp.db","DEV=FBCK:LI22:1,RF=L2,PHASE_LNK=ACCL:LI22:1:PDES,AMPL_LNK=ACCL:LI22:1:ADES,DRVH=6000")
dbLoadRecords("db/iocEnergyChirp.db","DEV=FBCK:LI25:1,RF=L3,PHASE_LNK=ACCL:LI25:1:PDES,AMPL_LNK=ACCL:LI25:1:ADES,DRVH=16000")
# Load converged status and auto act ref+offset save databases.
dbLoadRecords("db/fbckSettled.db",     "LP=FBCK:$(FB):$(LOOP)")
dbLoadRecords("db/fbckSettledSum.db",  "LP=FBCK:$(FB):$(LOOP)")
dbLoadRecords("db/fbckAutoActLong.db", "LP=FBCK:$(FB):$(LOOP)")

dbLoadRecords("db/bsaFbck.db",  "D=FBCK:SYS0:1, EG=MeV,  HO=200,   LO=-90,    AD=5, PR=3, I='', LNK='', ATTR=DL1_ENERGY,  INP=STATE1, SINK_SIZE=1")
dbLoadRecords("db/bsaFbck.db",  "D=FBCK:SYS0:1, EG=MeV,  HO=400,   LO=-150,   AD=5, PR=3, I='', LNK='', ATTR=BC1_ENERGY,  INP=STATE2, SINK_SIZE=1")
dbLoadRecords("db/bsaFbck.db",  "D=FBCK:SYS0:1, EG=amps, HO=450,   LO=-300,   AD=5, PR=3, I='', LNK='', ATTR=BC1_CURRENT, INP=STATE3, SINK_SIZE=1")
dbLoadRecords("db/bsaFbck.db",  "D=FBCK:SYS0:1, EG=MeV,  HO=6500,  LO=-1700,  AD=5, PR=3, I='', LNK='', ATTR=BC2_ENERGY,  INP=STATE4, SINK_SIZE=1")
dbLoadRecords("db/bsaFbck.db",  "D=FBCK:SYS0:1, EG=amps, HO=50000, LO=-28000, AD=5, PR=3, I='', LNK='', ATTR=BC2_CURRENT, INP=STATE5, SINK_SIZE=1")
dbLoadRecords("db/bsaFbck.db",  "D=FBCK:SYS0:1, EG=MeV,  HO=17400, LO=-1,     AD=5, PR=3, I='', LNK='', ATTR=DL2_ENERGY,  INP=STATE6, SINK_SIZE=1")

<iocBoot/common/st.cmd
# ====================================================================
# Sequencer scripts to keep track of the CHIRP control and DL2 limits
# ====================================================================
seq(&chirpControl, "IOC=FB04,LOOP=LG01")
seq(&chirpUpdate, "IOC=FB04,LOOP=LG01")
seq(&limitUpdate, "IOC=FB04,LOOP=LG01")

seqShow()

#Done  
