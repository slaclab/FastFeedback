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
#USR_CPPFLAGS += -DDEBUG_PRINT -g -ggdb -DNDEBUG -DCHECK_BEAM -DCONTROL_ACTUATORS -DLOG -DNO_FBCK_PV

# Flags for testing 120Hz Undulator Launch and States on FCOM
#USR_CPPFLAGS += -DDEBUG_PRINT -g -ggdb -DNDEBUG -DCHECK_BEAM -DCONTROL_ACTUATORS -DLOG -DNO_FBCK_PV

# Flags for testing on FB05
#USR_CPPFLAGS += -DDEBUG_PRINT -g -ggdb -DLOG -DnoDEBUG_EVENT -DNDEBUG -DnCHECK_BEAM -DnCONTROL_ACTUATORS

# Flags for Unit Testing
USR_CPPFLAGS += -DDEBUG_PRINT -g -ggdb -DNDEBUG -DCONTROL_ACTUATORS -DLOG -DNO_FBCK_PV -DLINUX
#Question for Luciano -what is this override?  (ababbitt)
override OP_SYS_LDFLAGS =

USR_CPPFLAGS += -I$(BOOSTBINDINGS)
USR_CPPFLAGS += -I$(BOOST)
USR_CPPFLAGS += -I$(CPPUNIT)
USR_CPPFLAGS += -I$(LAPACK)

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

#ababbitt - July 1st, 2015 - Clean-up to only be calling from $PACKAGE_TOP~~~~~~
#LCLS_PACKAGE_DIR=/usr/local/lcls/package

#CPPUNIT_DIR=$(PACKAGE_TOP)/cppunit
#BOOST_DIR=$(PACKAGE_TOP)/boost
#BOOST_BINDINGS_DIR=$(PACKAGE_TOP)/boost-numeric-bindings
#LAPACK_DIR=$(PACKAGE_TOP)/lapack

PROD_IOC = fastFeedback ffUnitTest

# BOOST 1.42 (local on cdlx22)
#USR_CXXFLAGS += -DLINUX

#ifeq ($(BOOST_DIR), )
#USR_CXXFLAGS += -I $(LCLS_PACKAGE_DIR)/boost/include
#else
#USR_CXXFLAGS += -I $(BOOST_DIR)/include
#endif

#ifeq ($(BOOST_BINDINGS_DIR), )
#USR_CXXFLAGS += -I $(LCLS_PACKAGE_DIR)/boost-numeric-bindings/include/boost-numeric-bindings
#else
#USR_CXXFLAGS += -I $(BOOST_BINDINGS_DIR)/include/boost-numeric-bindings
#endif

#ifeq ($(GFORTRAN_DIR), )
#USR_LDFLAGS += -L$(LCLS_PACKAGE_DIR)/gfortran
#else
#USR_LDFLAGS += -L$(GFORTRAN_DIR)
#endif

#ifeq ($(LAPACK_DIR), )
#USR_LDFLAGS += -L$(LCLS_PACKAGE_DIR)/lapack
#else
#USR_LDFLAGS += -L$(LAPACK_DIR)
#endif

#ifeq ($(CPPUNIT_DIR), )
#CPPUNIT_DIR=$(LCLS_PACKAGE_DIR)/cppunit
#endif
#CPPUNIT_DIR=$(CPPUNIT_DIR)/cppunit

#ababbitt - updated this hardpath to linux-x86_64 (However, I am wondering whether we really want to have a hardpath.)
#Also ask about what would need to be performed to get the unit tests working for linux 64 (host machine) - is this even a valuable endeavor.
#fastFeedback_LDFLAGS  += -L $(EPICS_BASE_RELEASE)/lib/linux-x86_64
#Need to understand what this line is doing......how are fastFeedback_SYS_LIBS being used?
# fastFeedback_SYS_LIBS += lapackF blasF gfortran Com dbIoc dbStaticIoc dbtoolsIoc rsrvIoc ca asIoc miscIoc registryIoc

# Libraries fastFeedback links with (-llapack -lblas...)
fastFeedback_SYS_LIBS += lapack blas gfortran Com dbIoc dbStaticIoc dbtoolsIoc rsrvIoc ca asIoc miscIoc registryIoc
#~~~~~~~~ ababbitt - don't really understand this section of code~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~h

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
#USR_CXXFLAGS            += -I $(CPPUNIT_DIR)/include
ffUnitTest_SRCS         += $(CC_TEST_FILES) $(CC_FILES)
ffUnitTest_SRCS_DEFAULT += ffUnitTest.cc
ffUnitTest_LIBS         += $(fastFeedback_LIBS)
#ffUnitTest_LIBS         += cppunit
ffUnitTest_DBD          += $(fastFeedback_DBD)
ffUnitTest_LDFLAGS      += $(fastFeedback_LDFLAGS)
# ababbitt - why are there two ffUnitTest_SYS_LIBS?
ffUnitTest_SYS_LIBS     +=  dl gfortran
#ffUnitTest_SYS_LIBS     += cppunit dl $(fastFeedback_SYS_LIBS)
ffUnitTest_SYS_LIBS     += cppunit $(fastFeedback_SYS_LIBS)

#========================================

include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

