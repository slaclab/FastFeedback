#!/bin/bash
#--------------------------------------------------------------------------
# This script the new Longitudinal fast feedback (FB04:LG01) and
# enables Joe's 6x6 Matlab feedback.
#
# The setpoints from Longitudinal fast feedback are copied to Joe's
# Matlab feedback PVs.
#
# Date: Feb/24/2011
# Author: L.Piccoli
#--------------------------------------------------------------------------

#--------------------------------------------------------------------------
# Return the current status of the 6x6 feedback.
#
# @return "ON" if at least one of the 6x6 PVs is 1, "OFF" if all 6x6 PVs
# are 0
#--------------------------------------------------------------------------
function is6x6On() {
    dl1State=`caget -t -n SIOC:SYS0:ML00:AO290 | bc`
    bc1EState=`caget -t -n SIOC:SYS0:ML00:AO292 | bc`
    bc1CState=`caget -t -n SIOC:SYS0:ML00:AO293 | bc`
    bc2EState=`caget -t -n SIOC:SYS0:ML00:AO294 | bc`
    bc2CState=`caget -t -n SIOC:SYS0:ML00:AO295 | bc`
    dl2State=`caget -t -n SIOC:SYS0:ML00:AO296 | bc`
    if [ "$dl1State" -eq 0 ] && \
       [ "$bc1EState" -eq 0 ] && \
       [ "$bc1CState" -eq 0 ] && \
       [ "$bc2EState" -eq 0 ] && \
       [ "$bc2CState" -eq 0 ] && \
       [ "$dl2State" -eq 0 ]; then
      echo "OFF"
    else
      echo "ON"
    fi
}

#--------------------------------------------------------------------------
# Wrapper for a simple caput <PV> <value> command. The output generated
# by caput is redirected to /dev/null, so it does not polute the output
# of the script.
#
# @return "ON" if at least one of the 6x6 PVs is 1, "OFF" if all 6x6 PVs
# are 0
#--------------------------------------------------------------------------
function ca_put {
    caput $1 $2 > /dev/null
}

#--------------------------------------------------------------------------
# Main
#--------------------------------------------------------------------------

#
# Make sure that the Longitudinal fast feedback is ON before switching
# to the 6x6 Matlab feedback
#
longitudinalState=`caget -t -n FBCK:FB04:LG01:STATE`
if [ "$longitudinalState" == "0 " ]; then
  echo ""
  echo "ERROR: Longitudinal fast feedback must be ON to switch"
  echo "to the 6x6 Matlab feedback!"
  echo "Please make sure the new Longitudinal feedback"
  echo "is ON and the 6x6 is OFF before trying again."
  echo -e ">>> Hit enter to exit <<<"
  read response
  exit 0
fi

#
# Make sure that the 6x6 Matlab feedback is OFF before switching
# from the Longitudinal fast feedback
#
joeState=`is6x6On`
if [ "$state" == "ON" ]; then
  echo ""
  echo "ERROR: 6x6 Matlab feedback must be OFF to switch"
  echo "from the new Longitudinal feedback!"
  echo "Please make sure the new Longitudinal feedback"
  echo "is ON and the 6x6 is OFF before trying again."
  echo -e ">>> Hit enter to exit <<<"
  read response
  exit 0
fi

#
# Turn Longitudinal fast feedback OFF
#
echo "1) Turning Longitudital feedback off"
ca_put FBCK:FB04:LG01:STATE 0

#
# Disable Longitudinal alarms
#
disableALHGroup ACTLNG0
disableALHGroup SENSLNG0
disableALHGroup STATELNG0
disableALHGroup STATLNG0

#
# Change messages that show which feedback is enabled/disabled
#
ca_put SIOC:SYS0:ML02:AO125 2
ca_put SIOC:SYS0:ML02:AO126 1

#
# Copy setpoint parameters from the Longitudinal feedback to the 6x6 feedback
#
echo "2) Copying BC1 peak current set point"
ca_put SIOC:SYS0:ML00:AO016 `caget -t FBCK:FB04:LG01:S3DES`

echo "3) Copying BC2 peak current set point"
ca_put SIOC:SYS0:ML00:AO044 `caget -t FBCK:FB04:LG01:S5DES`

echo "4) Copying L3 FCOM energy to L3 CA energy"
ca_put SIOC:SYS0:ML00:AO075 `caget -t FBCK:FB04:LG01:A6`

#
# Show parameters and asks permission to enable 6x6
# feedback.
#
echo "----------------------------------------------------------"
echo "> BC1 peak current setpoint: `caget -t SIOC:SYS0:ML00:AO016` amps"
echo "> BC2 peak current setpoint: `caget -t SIOC:SYS0:ML00:AO044` amps"
echo "> L3 energy                : `caget -t SIOC:SYS0:ML00:AO075` MeV"
echo "----------------------------------------------------------"

# Enable "fancy" phase control
ca_put SIOC:SYS0:ML00:AO058 1
echo "Please turn 6x6 feedback ON!"

echo "Done."
