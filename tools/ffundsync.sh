#!/bin/sh

function cagetretry {
    PV=$1

    for i in `seq 1 6`; do
	value=`caget -w 10 -t $PV`
	status=$?
	if [ "$status" != "0" ]; then
	    echo "Failed to get $PV, trying again ($i)"
	    sleep 1
	else
	    break
	fi
    done

    if [ "$i" == "6" ]; then
	echo "Failed to get $PV"
	ffsyncerror "Failed to get $PV - please try again"
    fi

    echo $value
}

function caputretry {
    PV=$1
    value=$2

    for i in `seq 1 6`; do
	caput -w 10 $PV $value > /dev/null
	status=$?
	if [ "$status" != "0" ]; then
	    echo "Failed to set $PV, trying again ($i)"
	    sleep 1
	else
	    break
	fi
    done

    if [ "$i" == "6" ]; then
	echo "Failed to get $PV"
	ffsyncerror "Failed to set $PV - please try again"
    fi
}

function caputarrayretry {
    PV=$1
    value=$2

    caput -a -w 10 $PV $value > /dev/null
    status=$?
    if [ "$status" != "0" ]; then
	echo "Failed to set $PV"
	sleep 1
	caput -a -w 10 $PV $value > /dev/null
	status=$?
	if [ "$status" != "0" ]; then
	    echo "Failed to set $PV, trying again"
	    sleep 1
	    caput -a -w 10 $PV $value > /dev/null
	    status=$?
	    if [ "$status" != "0" ]; then
		echo "Failed to set $PV, trying again (last time)"
		ffsyncerror "Failed to set $PV"
	    fi
	fi
    fi
}

function ffsyncerror {
    echo "*** FEEDBACKS ARE NOT IN SYNC, TRY AGAIN ***"
    zenity --error --text "Error: $1\n\n*** FEEDBACKS ARE NOT IN SYNC, TRY AGAIN ***"
    exit 1
}

function copyvalue {
    PV="FBCK:$1:$3"
    value=`cagetretry $PV`
    status=$?
    if [ "$status" != "0" ]; then
	echo "Failed to get $PV"
	ffsyncerror "Failed to get $PV"
    fi

    PV="FBCK:$2:$3"
    if [ "$value" == "" ]; then
	caputretry $PV NULL
    else
	caputretry $PV $value
    fi
    status=$?
    if [ "$status" != "0" ]; then
	echo "Failed to set $PV"
	ffsyncerror "Failed to set $PV"
    fi
}

function copyarray {
    PV="FBCK:$1:$3"
    array=`cagetretry $PV`
    status=$?
    if [ "$status" != "0" ]; then
	echo "Failed to get $PV"
	ffsyncerror "Failed to get $PV"
    fi

    PV="FBCK:$2:$3"
    caputarrayretry $PV $array
    status=$?
    if [ "$status" != "0" ]; then
	echo "Failed to set $PV"
	ffsyncerror "Failed to set $PV"
    fi
}

function copyvalues {
    for i in `seq 1 $4`; do
	PV="FBCK:$1:$3$i$5"
	value=`cagetretry $PV`

	status=$?
	if [ "$status" != "0" ]; then
	    echo "Failed to get $PV"
	    ffsyncerror "Failed to get $PV"
	fi
	
	PV="FBCK:$2:$3$i$5"
	if [ "$value" == "" ]; then
	    caputretry $PV NULL
	else
	    caputretry $PV $value
	fi
	status=$?
	if [ "$status" != "0" ]; then
	    echo "Failed to set $PV"
	    ffsyncerror "Failed to set $PV"
	fi
    done  
}

function setstring {
    for i in `seq 1 $4`; do
	PV="FBCK:$2:$3$i$5"
	caputretry $PV NULL
	status=$?
	if [ "$status" != "0" ]; then
	    echo "Failed to set $PV"
	    ffsyncerror "Failed to set $PV"
	fi
    done  
}

function ffsyncconfig {
    echo "Source $1, Destination $2"
 
    echo "Stopping feedback $2"
    caputretry FBCK:$2:STATE 0

    copyarray $1 $2 FMATRIX
    copyarray $1 $2 GMATRIX

    copyvalue $1 $2 MEASNUM
    copyvalue $1 $2 ACTNUM
    copyvalue $1 $2 STATENUM

    copyvalues $1 $2 M 40 USED
    copyvalues $1 $2 S 10 USED
    copyvalues $1 $2 A 10 USED

    setstring $1 $2 A 10 DEVNAME

    copyvalues $1 $2 M 40 DEVNAME
    copyvalues $1 $2 S 10 NAME

    echo "Starting feedback $2"
    caputretry FBCK:$2:STATE 1
}

ffsyncconfig FB03:TR04 FB03:TR05

#caputretry FBCK:FB05:TR05:M1DEVNAME blah