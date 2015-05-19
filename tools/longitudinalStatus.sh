#!/bin/bash
#--------------------------------------------------------------------------
# This script prints the current state of Joe's 6x6 Matlab feedback and
# the new Longitudinal fast feedback (running on FB04:LG01)
#
# Date: Feb/25/2011
# Author: L.Piccoli
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

state=`is6x6On`
if [ "$state" == "OFF" ]; then
  echo "It is OFF"
fi


echo "--- Joe's 6x6 feedback status: $state"
echo "DL1 Energy: `caget -t -n SIOC:SYS0:ML00:AO290`"
echo "BC1 Energy: `caget -t -n SIOC:SYS0:ML00:AO292`"
echo "BC1 Bunch : `caget -t -n SIOC:SYS0:ML00:AO293`"
echo "BC2 Energy: `caget -t -n SIOC:SYS0:ML00:AO294`"
echo "BC2 Bunch : `caget -t -n SIOC:SYS0:ML00:AO295`"
echo "DL2 Energy: `caget -t -n SIOC:SYS0:ML00:AO296`"
echo ""
echo "--- Longitudinal feedback status: `caget -t FBCK:FB04:LG01:STATE`"
echo "DL1 Energy State: `caget -t FBCK:FB04:LG01:S1USED`"
echo "BC1 Energy State: `caget -t FBCK:FB04:LG01:S2USED`"
echo "BC1 Bunch State : `caget -t FBCK:FB04:LG01:S3USED`"
echo "BC2 Energy State: `caget -t FBCK:FB04:LG01:S4USED`"
echo "BC2 Bunch State : `caget -t FBCK:FB04:LG01:S5USED`"
echo "DL2 Energy State: `caget -t FBCK:FB04:LG01:S6USED`"
echo ""

#
# Value for the DL2 energy gain based on the LEM energy
#
function getDL2Gain() {
  ENERGY=( 4000 6000 9000 20000 )
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

gain=`getDL2Gain`
echo "Gain=$gain"
