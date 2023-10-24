#==============================================================
#
#  Abs:  Startup Script for cpu-b34-fb01 at B34-137
#  Name: st.cmd
#
#  Desc: The processor is an Advantech 1U server,
#        SKY-8100 industrial pc/server.
#
#  Facility: EED Fasfeedaback Controls
#
#  Auth: 31-Aug-2020, Jeremy Mock     (JMOCK)
#  Rev:  dd-mmm-yyyy, Reviewer's Name (USERNAME)
#--------------------------------------------------------------
#  Mod:
#        21-Sep-2020, K. Luchini      (LUCHINI):
#         extract from $EPICS_IOCS/cpu-b34-fb01/kernel-module-FF.cmd
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
evrManager /dev/evr0mng create vevr11     # sioc-ltus-tr01 test

### wait a bit to device become visible
sleep 1

chmod 666 /dev/vevr0
chmod 666 /dev/vevr1
chmod 666 /dev/vevr2
chmod 666 /dev/vevr3
chmod 666 /dev/vevr4
chmod 666 /dev/vevr5
chmod 666 /dev/vevr6
chmod 666 /dev/vevr7
chmod 666 /dev/vevr11

evrManager /dev/evr0mng init

# Q1: If I don't have a hardware trigger for fast feedback, do I need to allocate resources?
# Ans: No you do not need to map pulse generators to outputs:
#evrManager /dev/evr0mng alloc vevr0  pulsegen 16
#evrManager /dev/evr0mng alloc vevr0  pulsegen 0

# =============================
# Print EVR firmware version:
# =============================
evrManager /dev/evr0mng version

# =============================
# Print EVR Temperature:
# =============================
evrManager /dev/evr0mng temperature

# ============================================================================
# Network Configuration:
# =============================================================================
# Let's activate another ethernet port - FCOM Configuration:
# Let's activate the second network chip for Fast Feedback.
# Nodename: cpu-b34-fb01-fnet
# IP Addr:  172.25.160.17
# Netmask:  255.255.255.192
# The network parameters are for B34; the netmask may be different for PROD
# FCOMMGRP == mc-b034-fcom == 239.219.248.0
# 239.219.248.0 ==> mc-b034-fcom ==> MCAST Address
# ============================================================================
#TODO: Change to eth1
ifconfig eth1 172.25.160.17 netmask 255.255.255.192
export FCOMMCGRP="mc-b034-fcom"

route add -net FCOMMCGRP netmask 255.255.255.192 dev eth1
ip route add 224.0.0.0/4 dev eth1

# Note: the FCOM_MC_PREFIX is different between development and production
# Development: 239.219.248.0
# Production:  239.219.8.0
export FCOM_MC_PREFIX=239.219.248.0
# ==================================================================


#TODO: Get rid off
# Set IP for ATCA communication
ifconfig eth5 10.0.1.1 netmask 255.255.255.0 up

# ==================================================================
# escalating RT priority for network kernel threads (SCHED_FIFO, 84)
# =================================================================
for PID in  $(ps -elF | grep eth5 | grep -v grep | awk '{print $4}'); do chrt -f -p 84 $PID ; done

# Enable Ethernet SFP+ Bottom For Advantech 1U
ifconfig eth5 10.0.1.1 netmask 255.255.255.0 up

# ==================================================================
# escalating RT priority for network kernel threads (SCHED_FIFO, 84)
# =================================================================
for PID in  $(ps -elF | grep eth6 | grep -v grep | awk '{print $4}'); do chrt -f -p 84 $PID ; done

# End of script




