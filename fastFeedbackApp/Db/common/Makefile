TOP=../../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
#  Optimization of db files using dbst (DEFAULT: NO)
#DB_OPT = YES

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this
# each delivered *.template file specified below looks for its
# *.substitutions file

# for each feedback loop
DB += fbckState.db
DB += fbckMeas.db
DB += fbckAct.db
DB += fbckConfig.db
DB += fbckStatus.db
DB += fbckIocConfig.db
DB += fbckIocStatus.db
# Loop substitutions records per loop
DB += fbckLoop.db
DB += fbckLTU.db
DB += fbckLNG.db
# IOC substitutions for PVs per feedback IOC
DB += fbckIoc.db
DB += iocEnergyChirp.db
#
#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

