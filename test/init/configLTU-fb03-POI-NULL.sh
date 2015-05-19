#!/bin/sh
# L2 Launch 
# BPMS -> File
# Magnets -> File
# Test multiple patterns!

caput FBCK:FB03:TR01:STATE 0

# 0.5 HZ, pockcel_perm
caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 8 0 0 0 256 0 0 54 0 0 0 0 0 0 0 0

# 1 HZ, pockcel_perm, exclude 0.5 Hz
caput -a FBCK:FB03:TR01:POI2 21 0 0 0 0 0 8 0 0 0 128 0 0 54 0 0 0 0 0 0 256 0

# 30 Hz, pc, inc TS4, exc TS1,2,3,5,6
caput -a FBCK:FB03:TR01:POI1 21 0 0 8 0 0 8 0 0 0 16 0 0 55 0 0 0 0 0 0 0 0

# !30Hz, no pc, inc TS4, exc TS1,2,3,4,5,6
caput -a FBCK:FB03:TR01:POI2 21 0 0 8 0 0 0 0 0 0 0 0 0 55 0 0 0 0 0 0 16 0


# F matrix
caput -a FBCK:FB03:TR01:FMATRIX 20 1 0 0 0 3 0.23827 10.9815 0 0 3 -1.1693 20.1173 0 0 3 -3.4697 44.6116 0 0 3 -3.1147 34.9654 0 0 3 0 0 1 0 3 0 0.00049525 1.7651 11.1121 3 -0.00010161 0.0010574 4.3439 37.3343 3 -0.00010916 0.0011588 3.6038 35.0322 3 -0.0001864 0.0019898 5.6193 59.5173 3

# G matrix
caput -a FBCK:FB03:TR01:GMATRIX 16 -13.8057 -15.7795 0 0 -0.030877 -1.3951 0 0 -0.0041469 0 -36.1979 8.6777 0.00021016 0 2.0431 -2.0813

# Loop configs
caput FBCK:FB03:TR01:NAME LTU-2POI-NULL
caput FBCK:FB03:TR01:INSTALLED 1
caput FBCK:FB03:TR01:TOTALPOI 2
caput FBCK:FB03:TR01:ITERATEFUNC TrajectoryFitBy1

# Measurements
caput FBCK:FB03:TR01:M1DEVNAME BPMS:LTU1:620:X
caput FBCK:FB03:TR01:M1USED 0
caput FBCK:FB03:TR01:M1CAMODE 0

caput FBCK:FB03:TR01:M2DEVNAME BPMS:LTU1:640:X
caput FBCK:FB03:TR01:M2USED 0
caput FBCK:FB03:TR01:M2CAMODE 0

caput FBCK:FB03:TR01:M3DEVNAME BPMS:LTU1:660:X
caput FBCK:FB03:TR01:M3USED 0
caput FBCK:FB03:TR01:M3CAMODE 0

caput FBCK:FB03:TR01:M4DEVNAME BPMS:LTU1:680:X
caput FBCK:FB03:TR01:M4USED 1
caput FBCK:FB03:TR01:M4CAMODE 0

caput FBCK:FB03:TR01:M5DEVNAME BPMS:LTU1:720:X
caput FBCK:FB03:TR01:M5USED 1
caput FBCK:FB03:TR01:M5CAMODE 0

caput FBCK:FB03:TR01:M6DEVNAME BPMS:LTU1:730:X
caput FBCK:FB03:TR01:M6USED 1
caput FBCK:FB03:TR01:M6CAMODE 0

caput FBCK:FB03:TR01:M7DEVNAME BPMS:LTU1:740:X
caput FBCK:FB03:TR01:M7USED 1
caput FBCK:FB03:TR01:M7CAMODE 0

caput FBCK:FB03:TR01:M8DEVNAME BPMS:LTU1:750:X
caput FBCK:FB03:TR01:M8USED 1
caput FBCK:FB03:TR01:M8CAMODE 0

caput FBCK:FB03:TR01:M9DEVNAME BPMS:LTU1:760:X
caput FBCK:FB03:TR01:M9USED 0
caput FBCK:FB03:TR01:M9CAMODE 0

caput FBCK:FB03:TR01:M10DEVNAME BPMS:LTU1:770:X
caput FBCK:FB03:TR01:M10USED 0
caput FBCK:FB03:TR01:M10CAMODE 0

caput FBCK:FB03:TR01:M11DEVNAME BPMS:LTU1:620:Y
caput FBCK:FB03:TR01:M11USED 0
caput FBCK:FB03:TR01:M11CAMODE 0

caput FBCK:FB03:TR01:M12DEVNAME BPMS:LTU1:640:Y
caput FBCK:FB03:TR01:M12USED 0
caput FBCK:FB03:TR01:M12CAMODE 0

caput FBCK:FB03:TR01:M13DEVNAME BPMS:LTU1:660:Y
caput FBCK:FB03:TR01:M13USED 0
caput FBCK:FB03:TR01:M13CAMODE 0

caput FBCK:FB03:TR01:M14DEVNAME BPMS:LTU1:680:Y
caput FBCK:FB03:TR01:M14USED 1
caput FBCK:FB03:TR01:M14CAMODE 0

caput FBCK:FB03:TR01:M15DEVNAME BPMS:LTU1:720:Y
caput FBCK:FB03:TR01:M15USED 1
caput FBCK:FB03:TR01:M15CAMODE 0

caput FBCK:FB03:TR01:M16DEVNAME BPMS:LTU1:730:Y
caput FBCK:FB03:TR01:M16USED 1
caput FBCK:FB03:TR01:M16CAMODE 0

caput FBCK:FB03:TR01:M17DEVNAME BPMS:LTU1:740:Y
caput FBCK:FB03:TR01:M17USED 1
caput FBCK:FB03:TR01:M17CAMODE 0

caput FBCK:FB03:TR01:M18DEVNAME BPMS:LTU1:750:Y
caput FBCK:FB03:TR01:M18USED 1
caput FBCK:FB03:TR01:M18CAMODE 0

caput FBCK:FB03:TR01:M19DEVNAME BPMS:LTU1:760:Y
caput FBCK:FB03:TR01:M19USED 0
caput FBCK:FB03:TR01:M19CAMODE 0

caput FBCK:FB03:TR01:M20DEVNAME BPMS:LTU1:770:Y
caput FBCK:FB03:TR01:M20USED 0
caput FBCK:FB03:TR01:M20CAMODE 0

caput FBCK:FB03:TR01:M21USED 0
caput FBCK:FB03:TR01:M22USED 0
caput FBCK:FB03:TR01:M23USED 0
caput FBCK:FB03:TR01:M24USED 0
caput FBCK:FB03:TR01:M25USED 0
caput FBCK:FB03:TR01:M26USED 0
caput FBCK:FB03:TR01:M27USED 0
caput FBCK:FB03:TR01:M28USED 0
caput FBCK:FB03:TR01:M29USED 0
caput FBCK:FB03:TR01:M30USED 0
caput FBCK:FB03:TR01:M31USED 0
caput FBCK:FB03:TR01:M32USED 0
caput FBCK:FB03:TR01:M33USED 0
caput FBCK:FB03:TR01:M34USED 0
caput FBCK:FB03:TR01:M35USED 0
caput FBCK:FB03:TR01:M36USED 0
caput FBCK:FB03:TR01:M37USED 0
caput FBCK:FB03:TR01:M38USED 0
caput FBCK:FB03:TR01:M39USED 0
caput FBCK:FB03:TR01:M40USED 0

# Reference orbit
caput -a FBCK:FB03:TR01:REFORBIT 8 0 0 0 0 0 0 0 0

# States
caput FBCK:FB03:TR01:S1NAME NULL
caput FBCK:FB03:TR01:S1USED 1

caput FBCK:FB03:TR01:S2NAME NULL
caput FBCK:FB03:TR01:S2USED 1

caput FBCK:FB03:TR01:S3NAME NULL
caput FBCK:FB03:TR01:S3USED 1

caput FBCK:FB03:TR01:S4NAME NULL
caput FBCK:FB03:TR01:S4USED 1

# Actuators
caput FBCK:FB03:TR01:A1DEVNAME NULL
caput FBCK:FB03:TR01:A1USED 1
caput FBCK:FB03:TR01:A1CAMODE 1

caput FBCK:FB03:TR01:A2DEVNAME NULL
caput FBCK:FB03:TR01:A2USED 1
caput FBCK:FB03:TR01:A2CAMODE 1

caput FBCK:FB03:TR01:A3DEVNAME NULL
caput FBCK:FB03:TR01:A3USED 1
caput FBCK:FB03:TR01:A3CAMODE 1

caput FBCK:FB03:TR01:A4DEVNAME NULL
caput FBCK:FB03:TR01:A4USED 1
caput FBCK:FB03:TR01:A4CAMODE 1

# Energy
caput -a FBCK:FB03:TR01:ACTENERGY 4 0.25 0.25 0.25 0.25

# Must set these up in order to have the config app work
caput FBCK:FB03:TR01:MEASNUM 20
caput FBCK:FB03:TR01:ACTNUM 4
caput FBCK:FB03:TR01:STATENUM 4
caput FBCK:FB03:TR01:PGAIN 0.1

# Enable loop
caput FBCK:FB03:TR01:INSTALLED 1
####caput FBCK:FB03:TR01:STATE 1
