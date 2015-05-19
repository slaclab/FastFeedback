#!/bin/sh

function backup {
  sourceDirectory="$IOC_DATA/ioc-sys0-$1/$2"
  if [ ! -d $sourceDirectory ]; then
    echo "Directory $sourceDirectory does not exist, can't back it up."
    exit -1
  fi
  destDirectory=$sourceDirectory$3
  echo $sourceDirectory " => " $destDirectory
  cp -R $sourceDirectory $destDirectory
  if [ $? != "0" ]; then
    echo "Failed to make copy of $sourceDirectory"
    exit -1
  fi
  if [ ! -d $destDirectory ]; then
    echo "Failed to create backup directory $destDirectory"
    exit -1
  fi
}

function backupIoc {
  dateStr=`date +"%F_%R"`
  echo "=== Backing up $1 ($dateStr) ==="
  backup $1 "autosave" $dateStr
  backup $1 "autosave-req" $dateStr
}

backupIoc "fb01"
backupIoc "fb02"
backupIoc "fb03"
backupIoc "fb04"
#backupIoc "fb05"