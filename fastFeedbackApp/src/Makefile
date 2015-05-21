TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#========================================

# for gdb/ddd debugger:
USR_CFLAGS_linux-x86 += -g
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
USR_CPPFLAGS += -DDEBUG_PRINT -g -ggdb -DNDEBUG -DCHECK_BEAM -DCONTROL_ACTUATORS -DLOG -DNO_FBCK_PV

# Flags for testing 120Hz Undulator Launch and States on FCOM
#USR_CPPFLAGS += -DDEBUG_PRINT -g -ggdb -DNDEBUG -DCHECK_BEAM -DCONTROL_ACTUATORS -DLOG -DNO_FBCK_PV

# Flags for testing on FB05
#USR_CPPFLAGS += -DDEBUG_PRINT -g -ggdb -DLOG -DnoDEBUG_EVENT -DNDEBUG -DnCHECK_BEAM -DnCONTROL_ACTUATORS

override OP_SYS_LDFLAGS =

SRC_DIRS += $(TOP)/fastFeedbackApp/src/framework
SRC_DIRS += $(TOP)/fastFeedbackApp/src/util
SRC_DIRS += $(TOP)/fastFeedbackApp/src/algo
SRC_DIRS += $(TOP)/fastFeedbackApp/src/test

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

# Test Source Files
#CC_TEST_FILES = 
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

LCLS_PACKAGE_DIR=/usr/local/lcls/package

CPPUNIT_DIR=$(PACKAGE_TOP)/cppunit
BOOST_DIR=$(PACKAGE_TOP)/boost
BOOST_BINDINGS_DIR=$(PACKAGE_TOP)/boost-numeric-bindings
LAPACK_DIR=$(PACKAGE_TOP)/lapack

PROD_IOC = fastFeedback ffUnitTest

# BOOST 1.42 (local on cdlx22)
USR_CXXFLAGS += -DLINUX

ifeq ($(BOOST_DIR), )
USR_CXXFLAGS += -I $(LCLS_PACKAGE_DIR)/boost/include
else
USR_CXXFLAGS += -I $(BOOST_DIR)/include
endif

ifeq ($(BOOST_BINDINGS_DIR), )
USR_CXXFLAGS += -I $(LCLS_PACKAGE_DIR)/boost-numeric-bindings/include/boost-numeric-bindings
else
USR_CXXFLAGS += -I $(BOOST_BINDINGS_DIR)/include/boost-numeric-bindings
endif

ifeq ($(GFORTRAN_DIR), )
USR_LDFLAGS += -L$(LCLS_PACKAGE_DIR)/gfortran
else
USR_LDFLAGS += -L$(GFORTRAN_DIR)
endif

ifeq ($(LAPACK_DIR), )
USR_LDFLAGS += -L$(LCLS_PACKAGE_DIR)/lapack
else
USR_LDFLAGS += -L$(LAPACK_DIR)
endif

ifeq ($(CPPUNIT_DIR), )
CPPUNIT_DIR=$(LCLS_PACKAGE_DIR)/cppunit
endif

fastFeedback_LDFLAGS  += -L $(EPICS_BASE_RELEASE)/lib/linux-x86
fastFeedback_SYS_LIBS += lapackF blasF gfortran Com dbIoc dbStaticIoc dbtoolsIoc rsrvIoc ca asIoc miscIoc registryIoc


DBD = fastFeedback.dbd
fastFeedback_DBD += base.dbd
fastFeedback_DBD += iocAdmin.dbd
fastFeedback_DBD += autosaveSupport.dbd
fastFeedback_DBD += caPutLog.dbd
fastFeedback_DBD += ffRecord.dbd
fastFeedback_DBD += devFfDeviceView.dbd
fastFeedback_DBD += devFfConfig.dbd
fastFeedback_DBD += evrSupport.dbd
fastFeedback_DBD += devMrfEr.dbd

fastFeedback_SRCS += $(CC_FILES) chirpControl.st
fastFeedback_SRCS += fastFeedback_registerRecordDeviceDriver.cpp
fastFeedback_SRCS_DEFAULT += ffControllerMain.cpp

# libs linux IOC app
fastFeedback_LIBS += miscUtils
fastFeedback_LIBS += devIocStats
fastFeedback_LIBS += caPutLog
fastFeedback_LIBS += autosave
fastFeedback_LIBS += seq pv
fastFeedback_LIBS += evrSupport
fastFeedback_LIBS += devMrfEr
fastFeedback_LIBS += drvMrf
fastFeedback_LIBS += $(EPICS_BASE_IOC_LIBS)

# CPPUNIT Tests
USR_CXXFLAGS            += -I $(CPPUNIT_DIR)/include
ffUnitTest_SRCS         += $(CC_TEST_FILES) $(CC_FILES)
ffUnitTest_SRCS_DEFAULT += ffUnitTest.cc
ffUnitTest_LIBS         += $(fastFeedback_LIBS)
ffUnitTest_DBD          += $(fastFeedback_DBD)
ffUnitTest_LDFLAGS      += -L $(CPPUNIT_DIR)/lib $(fastFeedback_LDFLAGS)
ffUnitTest_SYS_LIBS     += cppunit dl $(fastFeedback_SYS_LIBS)
ffUnitTest_SYS_LIBS     += $(fastFeedback_SYS_LIBS)

#========================================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

