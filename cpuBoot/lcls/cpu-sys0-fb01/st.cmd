#==============================================================
#
#  Abs:  Startup Script for cpu-sys0-fb01 at B005-3023
#
#  Name: st.cmd
#
#  Desc: The processor is an Advantech 1U server,
#        SKY-8100 industrial pc/server.
#
#  Facility: LCLS Fasfeedaback Controls
#
#  Auth: 27-Aug-2020, Jeremy Mock     (JMOCK)
#  Rev:  dd-mmm-yyyy, Reviewer's Name (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        21-Sep-2020, K. Luchini      (LUCHINI):
#         extract from $EPICS_IOCS/cpu-sys-fb01/kernel-module.cmd
#
#==============================================================

# =========================================
# Turn on local IPMI Client support
# ========================================
modprobe ipmi_devintf
ipmitool lan print

# =========================================
# Timing - evr pci device managment
# =========================================
loadModule.sh $EPICS_CPU_LIB/evrma.ko
chmod 666 /dev/evr0mng

evrManager /dev/evr0mng create vevr0
evrManager /dev/evr0mng create vevr1
evrManager /dev/evr0mng create vevr2
evrManager /dev/evr0mng create vevr3
evrManager /dev/evr0mng create vevr4
evrManager /dev/evr0mng create vevr5
evrManager /dev/evr0mng create vevr6
evrManager /dev/evr0mng create vevr7
evrManager /dev/evr0mng create vevr8
evrManager /dev/evr0mng create vevr9
# evrManager /dev/evr0mng create vevr10 <-- upcoming softline longitudinal
evrManager /dev/evr0mng create vevr11
evrManager /dev/evr0mng create vevr12
evrManager /dev/evr0mng create vevr13 <-- upcoming fast UNDS feedback

# wait a bit to device become visible
sleep 1

chmod 666 /dev/vevr0
chmod 666 /dev/vevr1
chmod 666 /dev/vevr2
chmod 666 /dev/vevr3
chmod 666 /dev/vevr4
chmod 666 /dev/vevr5
chmod 666 /dev/vevr6
chmod 666 /dev/vevr7
chmod 666 /dev/vevr8
chmod 666 /dev/vevr9
# chmod 666 /dev/vevr10
chmod 666 /dev/vevr11
chmod 666 /dev/vevr12
chmod 666 /dev/vevr13
echo "Initializing evrManager"
evrManager /dev/evr0mng init

# ============================================================================
# Let's activate another ethernet port:
# Let's activate the second network chip for Fast Feedback. cpu-sys0-fb01-fnet
# Name:   cpu-sys0-fb01-fnet.slac.stanford.edu
# Address: 172.27.29.102

# ============================================================================
ifconfig eth3 172.27.29.102 netmask 255.255.252.0
export FCOMMCGRP="mc-lcls-fcom"
route add -net $FCOMMCGRP netmask 255.255.252.0 dev eth3
ip route add 224.0.0.0/4 dev eth3
export FCOM_MC_PREFIX=239.219.8.0

# End of script




