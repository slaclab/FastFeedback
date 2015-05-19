#!/bin/bash
#--------------------------------------------------------------------------
# This script disables Joe's 6x6 Matlab feedback and enables the new 
# Longitudital fast feedback.
#
# The setpoints from Joe's feedback are copied to the Longitudinal setpoints.
#
# This script takes one parameter, which is the gain for the DL2 Energy.
# This gain should be set according to the current energy. Based on some
# experiments 500 should be good for 14 GeV, and 100 for 4 GeV.
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
# Return the DL2 Gain according to the current LEM energy defined in the
# SIOC:SYS0:ML00:AO409 PV.
#
# Energy (MeV)  -> DL2 Gain
# 0 to 4000     -> 100
# 4001 to 6000  -> 250
# 6001 to 9000  -> 400
# 9001 to ...   -> 500
#
# @return gain for DL2 energy based on current beam energy
#--------------------------------------------------------------------------
function getDL2Gain() {
  ENERGY=( 4000 6000 9000 50000 )
  GAIN=( 100 250 400 500 )

  energy_gev=`caget -t SIOC:SYS0:ML00:AO409`
  energy_tmp=`echo $energy_gev*1000|bc`
  energy_mev=`echo $energy_tmp | cut -d"." -f1`
  elements=${#GAIN[@]}

  for (( i=0;i < $elements;i++)); do
    if [ "$energy_mev" -lt ${ENERGY[${i}]} ]; then
      echo ${GAIN[${i}]}
      break
    fi
  done 
}

#--------------------------------------------------------------------------
# Main
#--------------------------------------------------------------------------

#
# Make sure that the Longitudinal fast feedback is OFF before switching
# from the 6x6 Matlab feedback
#
longitudinalState=`caget -t -n FBCK:FB04:LG01:STATE`
if [ "$longitudinalState" == "1 " ]; then
  echo ""
  echo "ERROR: Longitudinal fast feedback must be OFF to switch"
  echo "from the 6x6 Matlab feedback!"
  echo "Please make sure the new Longitudinal feedback"
  echo "is OFF and the 6x6 is ON before trying again."
  echo -e ">>> Hit enter to exit <<<"
  read response
  exit 0
fi

#
# Make sure that the 6x6 Matlab feedback is ON before switching
# to the Longitudinal fast feedback
#
joeState=`is6x6On`
if [ "$state" == "OFF" ]; then
  echo ""
  echo "ERROR: 6x6 Matlab feedback must be ON to switch"
  echo "to the new Longitudinal feedback!"
  echo "Please make sure the new Longitudinal feedback"
  echo "is OFF and the 6x6 is ON before trying again."
  echo -e ">>> Hit enter to exit <<<"
  read response
  exit 0
fi

#
# Get current DL2 Gain
#
gain=`getDL2Gain`

#
# Set the SxxUSED PVs based on the current 6x6 configuration
#
echo "0) Enabling states based on 6x6 current setup"
ca_put FBCK:FB04:LG01:S6USED `caget -t -n SIOC:SYS0:ML00:AO296`
ca_put FBCK:FB04:LG01:S5USED `caget -t -n SIOC:SYS0:ML00:AO295`
ca_put FBCK:FB04:LG01:S4USED `caget -t -n SIOC:SYS0:ML00:AO294`
ca_put FBCK:FB04:LG01:S3USED `caget -t -n SIOC:SYS0:ML00:AO293`
ca_put FBCK:FB04:LG01:S2USED `caget -t -n SIOC:SYS0:ML00:AO292`
ca_put FBCK:FB04:LG01:S1USED `caget -t -n SIOC:SYS0:ML00:AO290`

#
# Turn 6x6 OFF
#
echo "1) Turning 6x6 feedback off"
ca_put SIOC:SYS0:ML00:AO296 0
ca_put SIOC:SYS0:ML00:AO295 0
ca_put SIOC:SYS0:ML00:AO294 0
ca_put SIOC:SYS0:ML00:AO293 0
ca_put SIOC:SYS0:ML00:AO292 0
ca_put SIOC:SYS0:ML00:AO290 0

#
# Copy setpoint parameters from 6x6 to the Longitudinal feedback
#
echo "2) Copying BC1 peak current set point"
ca_put FBCK:FB04:LG01:S3DES `caget -t SIOC:SYS0:ML00:AO016`

echo "3) Copying BC2 peak current set point"
ca_put FBCK:FB04:LG01:S5DES `caget -t SIOC:SYS0:ML00:AO044`

echo "4) Copying LEM energy to DL2 set point"
energy_gev=`caget -t SIOC:SYS0:ML00:AO409`
energy_mev=`echo $energy_gev*1000|bc`
ca_put FBCK:FB04:LG01:S6DES $energy_mev

#
# Reload Gain Matrix, setting DL2 energy gain based on the
# current beam energy
#
echo "5) Changing DL2 energy gain to $gain (reloading GMatrix)"
caput -a FBCK:FB04:LG01:GMATRIX 200 1 0 0 0 0 0 0 1 0 0 0 0 0 0 1 0 0 0 0 0 0 500 0 0 0 0 0 0 1 0 0 0 0 0 0 $gain 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 > /dev/null

# Enable "fancy" phase control
ca_put SIOC:SYS0:ML00:AO058 1
echo "Please turn Longitudinal feedback ON!"

#
# Enable Longitudinal alarms
#
enableALHGroup ACTLNG0
enableALHGroup SENSLNG0
enableALHGroup STATELNG0
enableALHGroup STATLNG0

#
# Change messages that show which feedback is enabled/disabled
#
ca_put SIOC:SYS0:ML02:AO125 1
ca_put SIOC:SYS0:ML02:AO126 2

#
# Show parameters and asks permission to enable Longitudinal
# feedback.
#
echo "----------------------------------------------------------"
echo "> BC1 peak current setpoint: `caget -t FBCK:FB04:LG01:S3DES` amps"
echo "> BC2 peak current setpoint: `caget -t FBCK:FB04:LG01:S5DES` amps"
echo "> DL2 energy setpoint:       `caget -t FBCK:FB04:LG01:S6DES` MeV"
echo "> DL2 energy gain:           $gain"
echo "----------------------------------------------------------"

echo "Done."
