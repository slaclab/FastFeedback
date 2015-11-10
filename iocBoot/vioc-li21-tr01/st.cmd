#
# st.cmd file for Fast Feedback Controller IOC
# author: L. Piccoli, A. Babbitt
# 
# modified:
#   Nov. 05, 2015 L. Piccoli - created
#
##########################################################

# Where am I?
pwd()
< envPaths

cd("../..")

epicsEnvSet("EPICS_CA_MAX_ARRAY_BYTES","32000")
epicsEnvSet("IOCSH_PS1","epics@vioc-b34-fb01>")

#epicsEnvSet("IOC_DATA","/data/vioc-b34-fb01")
#epicsEnvSet("IOC", "")

# bspExtMemProbe only durint init. clear this to avoid the lecture.
bspExtVerbosity=0

## Register all support components
dbLoadDatabase("dbd/fastFeedback.dbd")
fastFeedback_registerRecordDeviceDriver(pdbbase)

## Load iocAdmin record instances
dbLoadRecords("db/iocAdminSoft.db","IOC=IOC:SYS0:FB01")
dbLoadRecords("db/iocRelease.db","IOC=IOC:SYS0:FB01")
# load evr database
dbLoadRecords("db/IOC-SYS0-FB01.template")
# Load application
dbLoadRecords("db/fbckFB01.template")

## Setup autosave/restore will write to ioc-sys0-fb01 directory
set_requestfile_path("/data/autosave-req")
set_savefile_path("/data/autosave")

# this creates autosave status PVs named IOC:SYS0:FB01:xxx
# to report autosave status
save_restoreSet_status_prefix("IOC:SYS0:FB01:")
dbLoadRecords("db/save_restoreStatus.db", "P=IOC:SYS0:FB01:")

# save restore setup: incomplete restore ok, backup file overwrites
save_restoreSet_IncompleteSetsOk(1)
save_restoreSet_DatedBackupFiles(0)


# evr hardware initialization
drvMrfErFlag=0
ErConfigure(0, 0, 0, 0, 1)
evrInitialize()

# =================================================================
# User defined environment vars
# 
# =================================================================
epicsEnvSet("LOCA_NAME","FB01",1)
epicsEnvSet("IOC_NAME","FB01",1)
epicsEnvSet("LOCA2_NAME", "FB01", 1)

## iocAdmin
epicsEnvSet("ENGINEER","L.Piccoli")
epicsEnvSet("LOCATION","cpu-b34-fb01")
epicsEnvSet("STARTUP","/usr/local/lcls/epics/iocCommon/vioc-b34-fb01")
epicsEnvSet("ST_CMD","startup.cmd")

#fcomInit(fcomUtilGethostbyname(getenv("FCOMMCGRP"),0),1000)

## Restore datasets
set_pass0_restoreFile("info_mon_FB01.sav")
set_pass1_restoreFile("info_mon_FB01.sav")

set_pass0_restoreFile("info_config_TR01.sav")
set_pass1_restoreFile("info_config_TR01.sav")
set_pass0_restoreFile("info_config_TR02.sav")
set_pass1_restoreFile("info_config_TR02.sav")
set_pass0_restoreFile("info_config_TR03.sav")
set_pass1_restoreFile("info_config_TR03.sav")
set_pass0_restoreFile("info_config_TR04.sav")
set_pass1_restoreFile("info_config_TR04.sav")
set_pass0_restoreFile("info_config_TR05.sav")
set_pass1_restoreFile("info_config_TR05.sav")
set_pass0_restoreFile("info_config_LG01.sav")
set_pass1_restoreFile("info_config_LG01.sav")
set_pass0_restoreFile("info_config_GN01.sav")
set_pass1_restoreFile("info_config_GN01.sav")
#set_pass0_restoreFile("info_config_GN02.sav")
#set_pass1_restoreFile("info_config_GN02.sav")

## Note that autosave/restore spews a lot of messages and message
## throttling throttles all of other messages below.

#create file of feedback loops
iocshCmd("dbgrep 'FBCK:*:NAME' > /tmp/loops.txt")

# Start controller threads
ffStart()
#iocInit()

## Start saving datasets
cd("/data/vioc-b34-fb01/autosave-req")
makeAutosaveFileFromDbInfo("info_mon_FB01.req", "autosaveMonFB01")
create_monitor_set("info_mon_FB01.req",60,"")

makeAutosaveFileFromDbInfo("info_config_TR01.req", "autosaveConfigTR01")
create_triggered_set("info_config_TR01.req","FBCK:FB01:TR01:SAVE","")

makeAutosaveFileFromDbInfo("info_config_TR02.req", "autosaveConfigTR02")
create_triggered_set("info_config_TR02.req","FBCK:FB01:TR02:SAVE","")

makeAutosaveFileFromDbInfo("info_config_TR03.req", "autosaveConfigTR03")
create_triggered_set("info_config_TR03.req","FBCK:FB01:TR03:SAVE","")

makeAutosaveFileFromDbInfo("info_config_TR04.req", "autosaveConfigTR04")
create_triggered_set("info_config_TR04.req","FBCK:FB01:TR04:SAVE","")

makeAutosaveFileFromDbInfo("info_config_TR05.req", "autosaveConfigTR05")
create_triggered_set("info_config_TR05.req","FBCK:FB01:TR05:SAVE","")

makeAutosaveFileFromDbInfo("info_config_LG01.req", "autosaveConfigLG01")
create_triggered_set("info_config_LG01.req","FBCK:FB01:LG01:SAVE","")

makeAutosaveFileFromDbInfo("info_config_GN01.req", "autosaveConfigGN01")
create_triggered_set("info_config_GN01.req","FBCK:FB01:GN01:SAVE","")

#makeAutosaveFileFromDbInfo("info_config_GN02.req", "autosaveConfigGN02")
#create_triggered_set("info_config_GN02.req","FBCK:FB01:GN02:SAVE","")

#Done
