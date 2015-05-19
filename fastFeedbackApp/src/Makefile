TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#========================================

# for gdb/ddd debugger:
USR_CFLAGS_linux-x86 += -g
USR_CFLAGS_RTEMS     += -g -ggdb
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

# This is to keep debugging symbols when building for RTEMS
# OP_SYS_LDFLAGS contains -Wl,-S
override OP_SYS_LDFLAGS =

SRC_DIRS += $(TOP)/ffControllerApp/src/framework
SRC_DIRS += $(TOP)/ffControllerApp/src/util
SRC_DIRS += $(TOP)/ffControllerApp/src/algo
SRC_DIRS += $(TOP)/ffControllerApp/src/test

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

BOOST_DIR=$(PACKAGE_TOP)/boost
BOOST_BINDINGS_DIR=$(PACKAGE_TOP)/boost-numeric-bindings
GFORTRAN_RTEMS_DIR=$(PACKAGE_TOP)/gfortran-rtems
LAPACK_RTEMS_DIR=$(PACKAGE_TOP)/lapack
LAPACK_DIR=$(PACKAGE_TOP)/lapack

# RTEMS Starts here ====================================================
ifeq ($(OS_CLASS), RTEMS)
PROD_IOC = feedback #feedbackTest

USR_CXXFLAGS += -DRTEMS


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

ifeq ($(GFORTRAN_RTEMS_DIR), )
USR_LDFLAGS += -L$(LCLS_PACKAGE_DIR)/gfortran-rtems
else
USR_LDFLAGS += -L$(GFORTRAN_RTEMS_DIR)
endif

ifeq ($(LAPACK_RTEMS_DIR), )
USR_LDFLAGS += -L$(LCLS_PACKAGE_DIR)/lapack
else
USR_LDFLAGS += -L$(LAPACK_RTEMS_DIR)
endif

USR_LDFLAGS += -L$(EPICS_BASE_RELEASE)/lib/RTEMS-beatnik
feedback_LDFLAGS  += -L $(EPICS_BASE_RELEASE)/lib/linux-x86
feedback_SYS_LIBS += lapack_RTEMS blas_RTEMS gfortran Com ca dbIoc dbStaticIoc asIoc dbtoolsIoc rsrvIoc miscIoc registryIoc

# the dbd file
DBD = feedback.dbd
feedback_DBD += base.dbd
feedback_DBD += iocAdmin.dbd
feedback_DBD += autosaveSupport.dbd
feedback_DBD += caPutLog.dbd
feedback_DBD += devFfDeviceView.dbd
feedback_DBD += devFfConfig.dbd

feedback_DBD += evrSupport.dbd
feedback_DBD += devMrfEr.dbd
feedback_DBD += fcomUtil.dbd

# IOC Source Files
feedback_SRCS         += feedback_registerRecordDeviceDriver.cpp
feedback_SRCS         += $(CC_FILES) chirpControl.st chirpUpdate.st limitUpdate.st
# Is this for softioc only?
#feedback_SRCS_DEFAULT += ffControllerMain.cpp
feedback_SRCS_vxWorks += -nil-
feedback_SRCS_RTEMS   += -nil-

# libs rtems IOC app
feedback_LIBS += miscUtils
feedback_LIBS += devIocStats
feedback_LIBS += rtemsutils
feedback_LIBS += evrSupport
feedback_LIBS += devMrfEr
feedback_LIBS += mrfVme64x
feedback_LIBS += fcom fcomIocshSupport fcomUtil
feedback_LIBS += udpComm
feedback_LIBS += autosave
feedback_LIBS += caPutLog
feedback_LIBS += seq pv
feedback_LIBS += $(EPICS_BASE_IOC_LIBS)

# CPPUNIT Tests
feedbackTest_SRCS         += $(CC_TEST_FILES)
feedbackTest_SRCS_DEFAULT += ffUnitTest.cc 

# LINUX Starts here ====================================================
else

PROD_IOC = ffSoft ffUnitTest

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

ffSoft_LDFLAGS  += -L $(EPICS_BASE_RELEASE)/lib/linux-x86
ffSoft_SYS_LIBS += lapackF blasF gfortran Com dbIoc dbStaticIoc dbtoolsIoc rsrvIoc ca asIoc miscIoc registryIoc

# SIOC Source Files
ffSoft_SRCS += $(CC_FILES) chirpControl.st

# CPPUNIT Tests
USR_CXXFLAGS            += -I $(CPPUNIT_DIR)/include
ffUnitTest_LDFLAGS      += -L $(CPPUNIT_DIR)/lib $(ffSoft_LDFLAGS)
ffUnitTest_SYS_LIBS     += cppunit dl $(ffSoft_SYS_LIBS)
ffUnitTest_SYS_LIBS     += $(ffSoft_SYS_LIBS)
ffUnitTest_SRCS         += $(CC_TEST_FILES) $(CC_FILES)
ffUnitTest_SRCS_DEFAULT += ffUnitTest.cc

DBD = ffSoft.dbd
ffSoft_DBD += base.dbd
ffSoft_DBD += iocAdmin.dbd
ffSoft_DBD += autosaveSupport.dbd
ffSoft_DBD += caPutLog.dbd
ffSoft_DBD += ffRecord.dbd
ffSoft_DBD += devFfDeviceView.dbd
ffSoft_DBD += devFfConfig.dbd

# <name>_registerRecordDeviceDriver.cpp will be created from <name>.dbd
ffSoft_SRCS         += ffSoft_registerRecordDeviceDriver.cpp
ffSoft_SRCS_DEFAULT += ffControllerMain.cpp
ffSoft_SRCS_vxWorks += -nil-
ffSoft_SRCS_RTEMS   += -nil-

# libs linux IOC app
ffSoft_LIBS += miscUtils
ffSoft_LIBS += devIocStats
ffSoft_LIBS += caPutLog
ffSoft_LIBS += autosave
ffSoft_LIBS += seq pv
ffSoft_LIBS += $(EPICS_BASE_IOC_LIBS)
endif

#========================================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

