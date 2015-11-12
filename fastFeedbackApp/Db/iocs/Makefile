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

#include evr substitutions file
DB += IOC-SYS0-FB01.db
DB += IOC-SYS0-FB02.db
DB += IOC-SYS0-FB03.db
DB += IOC-SYS0-FB04.db
DB += IOC-SYS0-FB05.db
DB += IOC-SYS0-FB10.db

DB_INSTALLS += $(IOCADMIN)/db/iocAdminSoft.db 
IOCRELEASE_DB += iocRelease.db

#  autosave
DB_INSTALLS += $(AUTOSAVE)/db/save_restoreStatus.db

#gen_trig_edl:
#	create_edm_event_msi.py IOC-SYS0-FB0{1,2,3,4,5}trig.substitutions

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES

