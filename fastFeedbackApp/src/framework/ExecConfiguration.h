/* 
 * File:   ExecConfiguration.h
 * Author: lpiccoli
 *
 * Created on June 10, 2010, 2:20 PM
 */

#ifndef _EXECCONFIGURATION_H
#define	_EXECCONFIGURATION_H

#include <map>
#include <vector>
#include <string>
#include "Defs.h"
#include "LoopConfiguration.h"
#include "PvData.h"
#include "PatternMask.h"

FF_NAMESPACE_START

typedef std::map<std::string, LoopConfiguration *> LoopConfigurationMap;
const std::string FFCONTROLLER_VERSION = "$Name:  $";

/**
 * The ExecConfiguration contains configuration values associated with the whole
 * feedback system. Most of the configuration is provided by PVs.
 * 
 * @author L.Piccoli
 */
class ExecConfiguration {
public:

    ExecConfiguration();

    virtual ~ExecConfiguration() {
    };

    int initialize();
    int reconfigure();
    void clear();
    LoopConfiguration *getLoopConfiguration(std::string loopName);

    static ExecConfiguration &getInstance();
    int getLoopIndex(std::string loopName);
    int getSlotNames(std::vector<std::string> &slotNames);

    /**
     * Map containing the feedback loop configurations, keyed by the
     * loop (slot) name
     */
    LoopConfigurationMap _loopConfigurations;

    /**
     * Time delay between a fiducial callback and start of measurement collection
     */
    double _measurementDelay;

    /**
     * Delay time after the fiducial, before collecting measurements.
     *
     * This attribute maps to the $(AREA):TIMERDELAY PV
     */
    PvData<long> _timerDelayPv;

    /**
     * PV String waveform in that holds a list of available algorithms.
     *
     * This attribute is mapped to the IOC:SYS0:FBxx:ITERFUNCS
     */
    PvDataWaveform<std::string> _algorithmsAvailable;

    /**
     * This PvData is mapped to the SIOC:SYS0:FBxx:TMITLOW PV through
     * ChannelAccess link. Whenever the low TMIT value changes this
     * attribute changes
     */
    PvData<double> _tmitLowPv;

    /**
     * This PvData is mapped to the SIOC:SYS0:FBxx:BY1BDES PV through
     * ChannelAccess link. Whenever the BY1BDES PV changes this attribute
     * is changed as well.
     */
    PvData<double> _by1BdesPv;

    /**
     * This PvData is mapped to the IOC:IN20:LS11:PASER_PWR_READBACK PV through
     * ChannelAccess link. Whenever the PASER_PWR_READBACK PV changes this attribute
     * is changed as well.
     */
    PvData<double> _laserPowerReadbackPv;

    /**
     * This PvData is mapped to the SIOC:SYS0:FBxx:DL1_EREF PV through
     * ChannelAccess link. Whenever the DL1_EREF PV changes this attribute
     * is changed as well.
     */
    PvData<double> _dl1ErefPv;

    /**
     * This PvData is mapped to the SIOC:SYS0:FBxx:BC1_EREF PV through
     * ChannelAccess link. Whenever the BC1_EREF PV changes this attribute
     * is changed as well.
     */
    PvData<double> _bc1ErefPv;

    /**
     * This PvData is mapped to the SIOC:SYS0:FBxx:BC2_EREF PV through
     * ChannelAccess link. Whenever the BC2_EREF PV changes this attribute
     * is changed as well.
     */
    PvData<double> _bc2ErefPv;

    /**
     * This PvData is mapped to the SIOC:SYS0:FBxx:DL2_EREF PV through
     * ChannelAccess link. Whenever the DL2_EREF PV changes this attribute
     * is changed as well.
     */
    PvData<double> _dl2ErefPv;

    /**
     * This PvData is mapped to the Matlab PV SIOC:SYS0:ML00:AO280 PV through
     * ChannelAccess link. That PV contains the LOLO limit for the DL2 energy
     * actuator that is used by the Longitudinal feedback.
     */
    PvData<double> _dl2EnLoloPv;

    /**
     * This PvData is mapped to the Matlab PV SIOC:SYS0:ML00:AO281 PV through
     * ChannelAccess link. That PV contains the HIHI limit for the DL2 energy
     * actuator that is used by the Longitudinal feedback.
     */
    PvData<double> _dl2EnHihiPv;

    PvData<double> _s29AsumPv;

    /** Set to true if PatternGenerator is configured. Used for testing only. */
    bool _hasPatternGenerator;
};

FF_NAMESPACE_END

#endif	/* _EXECCONFIGURATION_H */

