#!/bin/sh

caput FBCK:FB03:TR01:STATE 0

# 0.5 HZ, pockcel_perm
caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 8 0 0 0 256 0 0 54 0 0 0 0 0 0 0 0

# 1 HZ, pockcel_perm
#caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 8 0 0 0 128 0 0 54 0 0 0 0 0 0 0 0

# 5 Hz, pockcel_perm
#caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 8 0 0 0 64 0 0 54 0 0 0 0 0 0 0 0

# 10 Hz, pockcel_perm
#caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 8 0 0 0 32 0 0 54 0 0 0 0 0 0 0 0

# 30 Hz, pockcel_perm
#caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 8 0 0 0 16 0 0 54 0 0 0 0 0 0 0 0

# F matrix
caput -a FBCK:FB03:TR01:FMATRIX 45 1 0 0 0 0.97811 1.9127 0 0 0.12335 0.80696 0 0 0 0 1 0 0 0 1.022 1.997 0 0 1.8976 4.2539 0 0 -0.17824 2.3499 0 0 -0.29402 1.5882 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 

# G matrix
caput -a FBCK:FB03:TR01:GMATRIX 10 2.5181 2.8809 0 0 -0.33363 1.1087 0 0 0 0 -0.34556 2.3591 0 0 -0.5343 0.89322 0 0 0 0 

# Loop configs
caput FBCK:FB03:TR01:NAME Inj
caput FBCK:FB03:TR01:INSTALLED 1
caput FBCK:FB03:TR01:TOTALPOI 1
caput FBCK:FB03:TR01:ITERATEFUNC TrajectoryFit
#caput FBCK:FB03:TR01:ITERATEFUNC Noop 

# Measurements
caput FBCK:FB03:TR01:M1DEVNAME BPMS:IN20:371:X
caput FBCK:FB03:TR01:M1USED 1
caput FBCK:FB03:TR01:M1CAMODE 1

caput FBCK:FB03:TR01:M2DEVNAME BPMS:IN20:425:X
caput FBCK:FB03:TR01:M2USED 1
caput FBCK:FB03:TR01:M2CAMODE 1

caput FBCK:FB03:TR01:M3DEVNAME BPMS:IN20:511:X
caput FBCK:FB03:TR01:M3USED 1
caput FBCK:FB03:TR01:M3CAMODE 1

caput FBCK:FB03:TR01:M4DEVNAME BPMS:IN20:371:Y
caput FBCK:FB03:TR01:M4USED 1
caput FBCK:FB03:TR01:M4CAMODE 1

caput FBCK:FB03:TR01:M5DEVNAME BPMS:IN20:425:Y
caput FBCK:FB03:TR01:M5USED 1
caput FBCK:FB03:TR01:M5CAMODE 1

caput FBCK:FB03:TR01:M6DEVNAME BPMS:IN20:511:Y
caput FBCK:FB03:TR01:M6USED 1
caput FBCK:FB03:TR01:M6CAMODE 1

# Reference orbit
caput -a FBCK:FB03:TR01:REFORBIT 6 -0.0059313 -0.12648 -0.13236 0.051098 -0.082934 -0.10219 

# States
caput FBCK:FB03:TR01:S1NAME FILE:/data/dat/X.Position
caput FBCK:FB03:TR01:S1USED 1

caput FBCK:FB03:TR01:S2NAME FILE:/data/dat/X.Angle
caput FBCK:FB03:TR01:S2USED 1

caput FBCK:FB03:TR01:S3NAME FILE:/data/dat/Y.Position
caput FBCK:FB03:TR01:S3USED 1

caput FBCK:FB03:TR01:S4NAME FILE:/data/dat/Y.Angle
caput FBCK:FB03:TR01:S4USED 1

# Actuators
caput FBCK:FB03:TR01:A1DEVNAME FILE:/data/dat/XCOR:IN20:381:BCTRL
caput FBCK:FB03:TR01:A1USED 1
caput FBCK:FB03:TR01:A1CAMODE 1

caput FBCK:FB03:TR01:A2DEVNAME FILE:/data/dat/XCOR:IN20:521:BCTRL
caput FBCK:FB03:TR01:A2USED 1
caput FBCK:FB03:TR01:A2CAMODE 1

caput FBCK:FB03:TR01:A3DEVNAME FILE:/data/dat/YCOR:IN20:382:BCTRL
caput FBCK:FB03:TR01:A3USED 1
caput FBCK:FB03:TR01:A3CAMODE 1

caput FBCK:FB03:TR01:A4DEVNAME FILE:/data/dat/YCOR:IN20:522:BCTRL
caput FBCK:FB03:TR01:A4USED 1
caput FBCK:FB03:TR01:A4CAMODE 1

# Energy
caput -a FBCK:FB03:TR01:ACTENERGY 4 0.073564 0.135 0.073564 0.135

# Enable loop
caput FBCK:FB03:TR01:INSTALLED 1
#caput FBCK:FB03:TR01:STATE 1

# Must set these up in order to have the config app work
caput FBCK:FB03:TR01:MEASNUM 6
caput FBCK:FB03:TR01:ACTNUM 4
caput FBCK:FB03:TR01:STATENUM 4
caput FBCK:FB03:TR01:PGAIN 0.01
