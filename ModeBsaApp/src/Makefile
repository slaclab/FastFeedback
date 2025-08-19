TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

# Use typed rset structure (see 3.16.1 release notes)
USR_CPPFLAGS += -std=c++17 -DUSE_TYPED_RSET



ModeBsaSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

# Build the IOC application
#PROD_IOC = ModeBsa

PROD_IOC_linuxRT-x86_64 = ModeBsa
PROD_IOC = $(PROD_IOC_$(T_A))

# ModeBsa.dbd will be created and installed
DBD += ModeBsa.dbd

# ModeBsa.dbd will include these files:
ModeBsa_DBD += base.dbd
ModeBsa_DBD += asyn.dbd
#ModeBsa_DBD += devModeBsaVersion.dbd
ModeBsa_DBD += evrSupport.dbd #Needed for EVR
ModeBsa_DBD += devMrfEr.dbd   #Needed for EVR
ModeBsa_DBD += register.dbd

# New BSA
ModeBsa_DBD += bsaCore.dbd

# ModeBsa_registerRecordDeviceDriver.cpp derives from ModeBsa.dbd
ModeBsa_SRCS += ModeBsa_registerRecordDeviceDriver.cpp
ModeBsa_SRCS += getter.cpp

# Build the main IOC entry point where needed
ModeBsa_SRCS_DEFAULT += ModeBsaMain.cpp
ModeBsa_SRCS_vxWorks += -nil-

# Link in the code from our support library
ModeBsa_LIBS += asyn
ModeBsa_LIBS += miscUtils
ModeBsa_LIBS += evrSupport
ModeBsa_LIBS += devMrfEr
ModeBsa_LIBS += drvMrf
evrma_DIR = $(EVRMA_LIB)
ModeBsa_LIBS += evrma

# Link in New BSA Support
ModeBsa_LIBS += eventBsaSupport
ModeBsa_LIBS += BsaCore

#ModeBsa_SRCS += devModeBsaVersion.c





# Link QSRV (pvAccess Server) if available
ifdef EPICS_QSRV_MAJOR_VERSION
    ModeBsa_LIBS += qsrv
    ModeBsa_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)
    ModeBsa_DBD += PVAServerRegister.dbd
    ModeBsa_DBD += qsrv.dbd
endif

# Finally link IOC to the EPICS Base libraries
ModeBsa_LIBS += $(EPICS_BASE_IOC_LIBS)

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD EXTRA GNUMAKE RULES BELOW HERE

# Explicit dependency needed for generated header file
devModeBsaVersion$(DEP): $(COMMON_DIR)/$(GENVERSION)