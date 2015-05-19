#!/bin/sh
# L2 Launch 
# BPMS -> File
# Magnets -> File

caput FBCK:FB03:TR01:STATE 0

# 0.5 HZ, pockcel_perm
#caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 8 0 0 0 256 0 0 54 0 0 0 0 0 0 0 0

# 1 HZ, pockcel_perm
#caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 8 0 0 0 128 0 0 54 0 0 0 0 0 0 0 0

# 5 Hz, pockcel_perm
#caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 8 0 0 0 64 0 0 54 0 0 0 0 0 0 0 0

# 10 Hz, pockcel_perm
#caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 8 0 0 0 32 0 0 54 0 0 0 0 0 0 0 0

# 30 Hz, pockcel_perm
#caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 0 0 0 0 16 0 0 54 0 0 0 0 0 0 0 0

# 60 Hz -> using TS4
caput -a FBCK:FB03:TR01:POI1 21 0 0 8 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0

# F matrix
caput -a FBCK:FB03:TR01:FMATRIX 0

# G matrix
caput -a FBCK:FB03:TR01:GMATRIX 0

# Loop configs
caput FBCK:FB03:TR01:NAME RateTest 
caput FBCK:FB03:TR01:INSTALLED 1
caput FBCK:FB03:TR01:TOTALPOI 1
caput FBCK:FB03:TR01:ITERATEFUNC Noop 

# Measurements -> No Measurements

# Reference orbit
caput -a FBCK:FB03:TR01:REFORBIT 0

# States -> No States

# Actuators -> No Actuators

# Energy
caput -a FBCK:FB03:TR01:ACTENERGY 0

# Must set these up in order to have the config app work
caput FBCK:FB03:TR01:MEASNUM 0
caput FBCK:FB03:TR01:ACTNUM 0
caput FBCK:FB03:TR01:STATENUM 0
caput FBCK:FB03:TR01:PGAIN 0.1

# Enable loop
caput FBCK:FB03:TR01:INSTALLED 1
caput FBCK:FB03:TR01:STATE 1
