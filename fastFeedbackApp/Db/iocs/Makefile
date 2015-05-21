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

# databases for each feedback loop are in common. Here we create
# the iocs

#include evr substitutions file
DB += IOC-SYS0-FB01.template
DB += IOC-SYS0-FB02.template
DB += IOC-SYS0-FB03.template
DB += IOC-SYS0-FB04.template
DB += IOC-SYS0-FB05.template
DB += IOC-SYS0-FB10.template

#now include overall substitution file template
DB += fbckFB10.template
DB += fbckFB05.template
DB += fbckFB04.template
DB += fbckFB03.template
DB += fbckFB02.template
DB += fbckFB01.template

# trigger
DB += IOC-SYS0-FB01trig.db
DB += IOC-SYS0-FB02trig.db
DB += IOC-SYS0-FB03trig.db
DB += IOC-SYS0-FB04trig.db
DB += IOC-SYS0-FB05trig.db

DB_INSTALLS += $(IOCADMIN)/db/iocAdminSoft.db 
IOCRELEASE_DB += iocRelease.db

#  autosave
DB_INSTALLS += $(AUTOSAVE)/db/save_restoreStatus.db

gen_trig_edl:
	create_edm_event_msi.py IOC-SYS0-FB0{1,2,3,4,5}trig.substitutions

#----------------------------------------------------
# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE
include $(TOP)/configure/RULES.Db

