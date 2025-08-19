TOP=../..
include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

DIRS += iocs

# Install databases, templates & substitutions like this
DB += circle.db
DB += dbExample1.db
DB += dbExample2.db
DB += ModeBsaVersion.db
DB += dbSubExample.db
DB += user.substitutions
DB += EVR-TEMPLATE.db
DB += getter.db

# If <anyname>.db template is not named <anyname>*.template add
# <anyname>_TEMPLATE = <templatename>

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD EXTRA GNUMAKE RULES BELOW HERE

#TOP=../..
#include $(TOP)/configure/CONFIG
#DIRS += common
#DIRS += iocs
include $(TOP)/configure/RULES_DIRS

