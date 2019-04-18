TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#========================================

# for gdb/ddd debugger:
# (ababbitt) USR_CFLAGS_linux-x86 += -g
#USR_CFLAGS           += -DDEBUG_PRINT

#------------------------------------------------------------------------------
# Compile flags
#
# -DCHECK_BEAM: Update measurements only if pockcel_perm bit is set - must be
#  used for production builds and removed for tests using FILES as input/outputs
#
# -DCONTROL_ACTUATORS: Must be enabled to send actuator updates to the devices.
#  If not enable the new values will show up it the Axx PVs, but won't be
#  sent on ChannelAccess or FCOM.
#
# DEBUG_PRINT_FLAGS:
# * DEBUG_PULSEID_FLAG
# * DEBUG_EVENT_FLAG
# * DEBUG_CA_FLAG
#==============================================================================
# Flags for release
USR_CPPFLAGS += -DDEBUG_PRINT -g -ggdb -DNDEBUG -DCHECK_BEAM -DCONTROL_ACTUATORS -DLOG

# Flags for testing 120Hz Undulator Launch and States on FCOM
#USR_CPPFLAGS += -DDEBUG_PRINT -g -ggdb -DNDEBUG -DCHECK_BEAM -DCONTROL_ACTUATORS -DLOG -DNO_FBCK_PV

# Flags for testing on production - with no actuators 
#USR_CPPFLAGS += -DDEBUG_PRINT -g -ggdb -DLOG -DnoDEBUG_EVENT -DNDEBUG -DnCHECK_BEAM -DnCONTROL_ACTUATORS

# Flags for Unit Testing
#USR_CPPFLAGS += -DDEBUG_PRINT -g -ggdb -DNDEBUG -DCONTROL_ACTUATORS -DLOG -DNO_FBCK_PV 

# ==================================================================
# Path to "NON EPICS" External PACKAGES: USER INCLUDES
# =================================================================
USR_CPPFLAGS += -I$(BOOSTBINDINGS_INCLUDE)/boost-numeric-bindings
USR_CPPFLAGS += -I$(BOOST_INCLUDE)
USR_CPPFLAGS += -I$(CPPUNIT_INCLUDE)
USR_CPPFLAGS += -I$(LAPACK_INCLUDE)
USR_CPPFLAGS += -I$(GFORT_INCLUDE)
USR_CPPFLAGS += -I$(QUADMATH_INCLUDE)
USR_CPPFLAGS += -I$(DL_INCLUDE)

# ========================================================
# Let's build the Following IOC Application:
# ========================================================
#ifeq ($(T_A),linuxRT-x86_64)
PROD_IOC = fastFeedback
#endif

DBD = fastFeedback.dbd
fastFeedback_DBD += base.dbd
fastFeedback_DBD += iocAdmin.dbd
fastFeedback_DBD += devIocStats.dbd #Part of the iocAdmin module
fastFeedback_DBD += asSupport.dbd
fastFeedback_DBD += caPutLog.dbd
fastFeedback_DBD += ffRecord.dbd
fastFeedback_DBD += devFfDeviceView.dbd
fastFeedback_DBD += devFfConfig.dbd
fastFeedback_DBD += evrSupport.dbd #Needed for EVR
fastFeedback_DBD += devMrfEr.dbd   #Needed for EVR
fastFeedback_DBD += fcomUtil.dbd
fastFeedback_DBD += fcomIocshSupport.dbd
fastFeedback_DBD += AutoActRef.dbd


# The system command as part of EPICS BASE is 
# available for all architectures
fastFeedback_DBD += system.dbd

# New BSA
fastFeedback_DBD += bsaCore.dbd


# ========================================
# Required for Qsrv
# ========================================
fastFeedback_DBD += PVAServerRegister.dbd
fastFeedback_DBD += qsrv.dbd
# ========================================

# =======================================================

# ========================================================================
# Source code for fastFeedback IOC Application
# =========================================================================
SRC_DIRS += $(TOP)/fastFeedbackApp/src/framework
SRC_DIRS += $(TOP)/fastFeedbackApp/src/util
SRC_DIRS += $(TOP)/fastFeedbackApp/src/algo


# Source Files
CC_FILES = Thread.cc EventReceiver.cc EventGenerator.cc PatternManager.cc \
	Timer.cc Device.cc CommunicationChannel.cc FileChannel.cc \
	NullChannel.cc ActuatorDevice.cc MeasurementDevice.cc \
	MeasurementCollector.cc DeviceView.cc StateDevice.cc \
	CollectorThread.cc LoopConfiguration.cc LoopThread.cc Loop.cc \
	Algorithm.cc SineWave.cc ExecThread.cc ExecConfiguration.cc \
	PatternGenerator.cc Commands.cc CaChannel.cc FcomChannel.cc \
	ffRecord.c devFfDeviceView.cc ChannelAccess.cc \
	PvData.cc devFfConfig.cc Pattern.cc TimeUtil.cc \
	PatternMask.cc Log.cc SetpointDevice.cc \
	Longitudinal.cc MeasurementAverage.cc LongitudinalChirp.cc \
	TrajectoryFitGeneral.cc TrajectoryFitStatic.cc InjectorLaunch.cc \
	TrajectoryFitBy1.cc TrajectoryFit.cc FcomChannelStates.cc \
	TrajectoryFitPoly.cc BunchCharge.cc TrajectoryFitPinv.cc \
	TrajectoryFitEact.cc EventLogger.cc

## ======================================================================      
# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd

fastFeedback_SRCS += $(CC_FILES) chirpUpdate.st chirpControl.st limitUpdate.st
fastFeedback_SRCS += AutoActRef.c
fastFeedback_SRCS += fastFeedback_registerRecordDeviceDriver.cpp
fastFeedback_SRCS_DEFAULT += fastFeedbackMain.cpp
# ==================================================================================

# ========================================================
# Link in external package libraries (-llapack -lblas...)
# Also, link system library (i.e. gfortran)
# System library must be reference like so;
# <IOCAPP>_SYS_LIB
# ========================================================
lapack_DIR             =  $(LAPACK_LIB)
fastFeedback_LIBS      += lapack

blas_DIR               =  $(LAPACK_LIB)
fastFeedback_LIBS      += blas

ifeq ($(T_A),linuxRT-x86_64)
gfortran_DIR=$(GFORT_LIB)
quadmath_DIR = $(QUADMATH_LIB)

fastFeedback_LIBS  += gfortran
fastFeedback_LIBS  += quadmath
else
fastFeedback_SYS_LIBS  += gfortran
endif

# ========================================================
# Link in EPICS MODULES and BASE
# =======================================================

# ================================================
# Adding Support for EPICS V4 (i.e. EPICS PVA)
# ================================================
# Link in the code from qsrv
fastFeedback_LIBS += qsrv

# ================================================
# EPICS_BASE_PVA_CORE_LIBS comes from pvAccessCPP 
# and is defined at <>/cfg/CONFIG_PVACCESS_MODULE
# ================================================
fastFeedback_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)

fastFeedback_LIBS += $(EPICS_BASE_IOC_LIBS)
fastFeedback_LIBS += miscUtils
fastFeedback_LIBS += devIocStats
fastFeedback_LIBS += caPutLog
fastFeedback_LIBS += autosave
fastFeedback_LIBS += seq pv
fastFeedback_LIBS += evrSupport
fastFeedback_LIBS += devMrfEr
fastFeedback_LIBS += fcom fcomIocshSupport fcomUtil
fastFeedback_LIBS += udpCommBSD

#fastFeedback_LIBS += drvMrf (Migrating to EVRsharing)
ifeq ($(T_A),linuxRT-x86_64)
  fastFeedback_LIBS += drvMrf
  evrma_DIR = $(EVRMA_LIB)
  fastFeedback_LIBS += evrma
endif
  
ifeq ($(T_A),rhel6-x86_64)
  fastFeedback_LIBS += drvMrf
  evrma_DIR = $(EVRMA_LIB)
  fastFeedback_LIBS += evrma
endif

# Link in New BSA Support
fastFeedback_LIBS += eventBsaSupport
fastFeedback_LIBS += BsaCore

#Link in Library for perfMeasure (new)
#fastFeedback_LIBS += perfMeasure


# ========================================================================
# CPPUNIT Tests IOC Application
# ========================================================================
# Let's build the Following IOC Application
#PROD_IOC         +=  ffUnitTest

DBD              += ffUnitTest.dbd
ffUnitTest_DBD   += base.dbd
ffUnitTest_DBD   += iocAdmin.dbd
ffUnitTest_DBD   += asSupport.dbd
ffUnitTest_DBD   += caPutLog.dbd
ffUnitTest_DBD   += evrSupport.dbd
ffUnitTest_DBD   += devMrfEr.dbd

# The system command as part of EPICS BASE is 
# available for all architectures
ffUnitTest_DBD += system.dbd

# New BSA
ffUnitTest_DBD += bsaCore.dbd

# ========================================
# Required for Qsrv
# ========================================
ffUnitTest_DBD += PVAServerRegister.dbd
ffUnitTest_DBD += qsrv.dbd



# ========================================================================
# Source code for Unit Test IOC Application
# =========================================================================
SRC_DIRS += $(TOP)/fastFeedbackApp/src/test
# Test Source Files
CC_TEST_FILES = EventReceiverTest.cc ThreadTest.cc \
	EventGeneratorTest.cc PatternTest.cc PatternManagerTest.cc \
	TimerTest.cc DeviceTest.cc FileChannelTest.cc \
	ActuatorDeviceTest.cc MeasurementDeviceTest.cc \
	StateDeviceTest.cc MeasurementCollectorTest.cc \
	DeviceViewTest.cc CollectorThreadTest.cc \
	LoopConfigurationTest.cc LoopThreadTest.cc LoopTest.cc \
	AlgorithmTest.cc SineWaveTest.cc ExecThreadTest.cc \
	ExecConfigurationTest.cc PatternGeneratorTest.cc \
	CaChannelTest.cc FcomChannelTest.cc TrajectoryFitTest.cc \
	PatternMaskTest.cc TimeTest.cc TimeAverageTest.cc \
	EventLoggerTest.cc

#	PvDataTest.cc ExecConfigurationTest.cc PatternGeneratorTest.cc \

ffUnitTest_SRCS         += $(CC_TEST_FILES) $(CC_FILES)

# Where is your Main IOC App? Did you break the convention?
ffUnitTest_SRCS_DEFAULT += ffUnitTest.cc

# =========================================================================
#  Link in external package libraries (-llapack -lblas...)
# Also, link system library (i.e. gfortran)
# System library must be reference like so;
# <IOCAPP>_SYS_LIB
# =========================================================================
cppunit_DIR             =  $(CPPUNIT_LIB)
ffUnitTest_LIBS         += cppunit

# we have already defined this library location above but will
# redefine for decoupling and clarity
lapack_DIR              =  $(LAPACK_LIB)
ffUnitTest_LIBS         += lapack

blas_DIR                =  $(LAPACK_LIB)
ffUnitTest_LIBS         += blas

ifeq ($(T_A),linuxRT-x86_64)
gfortran_DIR = $(GFORT_LIB)
dl_DIR = $(DL_LIB)
quadmath_DIR = $(QUADMATH_LIB)

ffUnitTest_LIBS     += dl 
ffUnitTest_LIBS     += gfortran
ffUnitTest_LIBS     += quadmath
else
ffUnitTest_SYS_LIBS     += dl
ffUnitTest_SYS_LIBS     += gfortran
endif

ifeq ($(T_A),linuxRT-x86_64)
   evrClientLab_LIBS += drvMrf
   evrma_DIR = $(EVRMA_LIB)
   evrClientLab_LIBS += evrma
endif
  
ifeq ($(T_A),rhel6-x86_64)
   evrClientLab_LIBS += drvMrf
   evrma_DIR = $(EVRMA_LIB)
   evrClientLab_LIBS += evrma
endif 


# ========================================================
# Link in EPICS MODULES and BASE
# =======================================================
ffUnitTest_LIBS += miscUtils
ffUnitTest_LIBS += devIocStats
ffUnitTest_LIBS += caPutLog
ffUnitTest_LIBS += autosave
ffUnitTest_LIBS += evrSupport
ffUnitTest_LIBS += devMrfEr
ffUnitTest_LIBS += drvMrf
ffUnitTest_LIBS += seq pv
ffUnitTest_LIBS += fcom fcomIocshSupport fcomUtil
ffUnitTest_LIBS += udpCommBSD

#fastFeedback_LIBS += drvMrf (Migrating to EVRsharing)
ifeq ($(T_A),linuxRT-x86_64)
  ffUnitTest_LIBS += drvMrf
  evrma_DIR = $(EVRMA_LIB)
  ffUnitTest_LIBS += evrma
endif
  
ifeq ($(T_A),rhel6-x86_64)
  ffUnitTest_LIBS += drvMrf
  evrma_DIR = $(EVRMA_LIB)
  ffUnitTest_LIBS += evrma
endif

# Link in New BSA Support
ffUnitTest_LIBS += eventBsaSupport
ffUnitTest_LIBS += BsaCore

# ================================================
# Adding Support for EPICS V4 (i.e. EPICS PVA)
# ================================================
# Link in the code from qsrv
ffUnitTest_LIBS += qsrv

# ================================================
# EPICS_BASE_PVA_CORE_LIBS comes from pvAccessCPP 
# and is defined at <>/cfg/CONFIG_PVACCESS_MODULE
# ================================================
ffUnitTest_LIBS += $(EPICS_BASE_PVA_CORE_LIBS)


ffUnitTest_LIBS += $(EPICS_BASE_IOC_LIBS)
#=============================================================================


include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

