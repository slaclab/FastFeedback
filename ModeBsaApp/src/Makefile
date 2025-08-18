TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS BELOW HERE

# Use typed rset structure (see 3.16.1 release notes)
USR_CPPFLAGS += -DUSE_TYPED_RSET

# xxxRecord.h will be created from xxxRecord.dbd
DBDINC += xxxRecord

# Install xxxSupport.dbd into <top>/dbd
DBD += xxxSupport.dbd

# Build an IOC support library
LIBRARY_IOC += ModeBsaSupport

# Compile and add code to the support library
ModeBsaSupport_SRCS += xxxRecord.c
ModeBsaSupport_SRCS += devXxxSoft.c

# Link locally-provided code into the support library,
# rather than directly into the IOC application, that
# causes problems on Windows DLL builds
ModeBsaSupport_SRCS += dbSubExample.c
ModeBsaSupport_SRCS += devModeBsaVersion.c
ModeBsaSupport_SRCS += ModeBsaHello.c
ModeBsaSupport_SRCS += initTrace.c

ModeBsaSupport_LIBS += $(EPICS_BASE_IOC_LIBS)

# Auto-generate a header file containing a version string.
# Version comes from the VCS if available, else date+time.
GENVERSION = ModeBsaVersion.h
# Macro name
GENVERSIONMACRO = ModeBsaVERSION

# Build the IOC application
PROD_IOC = ModeBsa

# ModeBsa.dbd will be created and installed
DBD += ModeBsa.dbd

# ModeBsa.dbd will include these files:
ModeBsa_DBD += base.dbd
ModeBsa_DBD += xxxSupport.dbd
ModeBsa_DBD += dbSubExample.dbd
ModeBsa_DBD += devModeBsaVersion.dbd
ModeBsa_DBD += ModeBsaHello.dbd
ModeBsa_DBD += initTrace.dbd

# ModeBsa_registerRecordDeviceDriver.cpp derives from ModeBsa.dbd
ModeBsa_SRCS += ModeBsa_registerRecordDeviceDriver.cpp

# Build the main IOC entry point where needed
ModeBsa_SRCS_DEFAULT += ModeBsaMain.cpp
ModeBsa_SRCS_vxWorks += -nil-

# Link in the code from our support library
ModeBsa_LIBS += ModeBsaSupport

# To build SNL programs, SNCSEQ must be defined
# in the <top>/configure/RELEASE file
ifneq ($(SNCSEQ),)
    # Build sncExample into ModeBsaSupport
    sncExample_SNCFLAGS += +r
    ModeBsa_DBD += sncExample.dbd
    # A .stt sequence program is *not* pre-processed:
    ModeBsaSupport_SRCS += sncExample.stt
    ModeBsaSupport_LIBS += seq pv
    ModeBsa_LIBS += seq pv

    # Build sncProgram as a standalone program
    PROD_HOST += sncProgram
    sncProgram_SNCFLAGS += +m
    # A .st sequence program *is* pre-processed:
    sncProgram_SRCS += sncProgram.st
    sncProgram_LIBS += seq pv
    sncProgram_LIBS += $(EPICS_BASE_HOST_LIBS)
endif

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
