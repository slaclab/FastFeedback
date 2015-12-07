TOP=../../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE

#----------------------------------------------------
# Create and install (or just install)
# databases, templates, substitutions like this
# each delivered *.template file specified below looks for its
# *.substitutions file

# databases for each feedback loop are in common. Here we create
# the iocs


#Generic evr substitution file (specify evr and feedback through macros in st.cmd)
#Should repace all previous six evr substitution files 
DB += EVR-TEMPLATE.db

#fbckFB_template.db is a generic file to configure all the feedback loops 
DB += fbck_template.db

# ========================================
# # Install timing/event related databases
# ========================================
DB_INSTALLS += $(EVENT)/db/EvrPmc.db
DB_INSTALLS += $(EVENT)/db/EvrPci.db
DB_INSTALLS += $(EVENT)/db/Pattern.db
DB_INSTALLS += $(EVENT)/db/PCI-trig.db
DB_INSTALLS += $(EVENT)/db/PMC-trig.db

# ========================================================
# # Install databases from iocAdmin package
# These databases support IOC Health and Monitoring
# ========================================================
#This is for soft ioc's only
DB_INSTALLS += $(IOCADMIN)/db/iocAdminSoft.db 
DB_INSTALLS += $(IOCADMIN)/db/iocAdminScanMon.db
DB_INSTALLS += $(IOCADMIN)/db/iocScanMonSum.db
DB_INSTALLS += $(IOCADMIN)/db/iocScanMon.db
IOCRELEASE_DB += iocRelease.db


#=========================================================
#Install Database to support reading the status of the autosave package
#========================================================

#  autosave
DB_INSTALLS += $(AUTOSAVE)/db/save_restoreStatus.db

#gen_trig_edl:
#	create_edm_event_msi.py IOC-SYS0-FB0{1,2,3,4,5}trig.substitutions

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES

