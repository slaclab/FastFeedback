#!/bin/sh

caput FBCK:FB01:TR02:STATE 0

# 0.5 HZ, pockcel_perm
#caput -a FBCK:FB01:TR02:POI1 21 0 0 0 0 0 8 0 0 0 256 0 0 54 0 0 0 0 0 0 0 0

# 1 HZ, pockcel_perm
#caput -a FBCK:FB01:TR02:POI1 21 0 0 0 0 0 8 0 0 0 128 0 0 54 0 0 0 0 0 0 0 0

# 5 Hz, pockcel_perm
#caput -a FBCK:FB01:TR02:POI1 21 0 0 0 0 0 8 0 0 0 64 0 0 54 0 0 0 0 0 0 0 0

# 10 Hz, pockcel_perm
caput -a FBCK:FB01:TR02:POI1 21 0 0 0 0 0 8 0 0 0 32 0 0 54 0 0 0 0 0 0 0 0

# 30 Hz, pockcel_perm
#caput -a FBCK:FB01:TR02:POI1 21 0 0 0 0 0 8 0 0 0 16 0 0 54 0 0 0 0 0 0 0 0

# F matrix
caput -a FBCK:FB01:TR02:FMATRIX 50 1 0 0 0 3 0.97811 1.9127 0 0 3 0.12335 0.80696 0 0 3 1.65 -0.33597 0 0 3 3.1276 -0.39243 0 0 3 0 0 1 0 3 0 0 1.022 1.997 3 0 0 1.8976 4.2539 3 0 0 -0.17824 2.3499 3 0 0 -0.29402 1.5882 3 0 0 0 0 0 0 0 0 0

# G matrix
caput -a FBCK:FB01:TR02:GMATRIX 10 2.5181 2.8809 0 0 -0.33363 1.1087 0 0 0 0 -0.34556 2.3591 0 0 -0.5343 0.89322 0 0 0 0 

# Loop configs
caput FBCK:FB01:TR02:NAME InjectorLaunchNULL
caput FBCK:FB01:TR02:INSTALLED 1
caput FBCK:FB01:TR02:TOTALPOI 1
caput FBCK:FB01:TR02:ITERATEFUNC TrajectoryFit
#caput FBCK:FB01:TR02:ITERATEFUNC Noop 

# Measurements
caput FBCK:FB01:TR02:M1DEVNAME BPMS:IN20:525:X
caput FBCK:FB01:TR02:M1USED 0
caput FBCK:FB01:TR02:M1CAMODE 0

caput FBCK:FB01:TR02:M2DEVNAME BPMS:IN20:581:X
caput FBCK:FB01:TR02:M2USED 1
caput FBCK:FB01:TR02:M2CAMODE 0

caput FBCK:FB01:TR02:M3DEVNAME BPMS:IN20:631:X
caput FBCK:FB01:TR02:M3USED 1
caput FBCK:FB01:TR02:M3CAMODE 0

caput FBCK:FB01:TR02:M4DEVNAME BPMS:IN20:651:X
caput FBCK:FB01:TR02:M4USED 1
caput FBCK:FB01:TR02:M4CAMODE 0

caput FBCK:FB01:TR02:M5DEVNAME BPMS:IN20:771:X
caput FBCK:FB01:TR02:M5USED 1
caput FBCK:FB01:TR02:M5CAMODE 0

caput FBCK:FB01:TR02:M6DEVNAME BPMS:IN20:781:X
caput FBCK:FB01:TR02:M6USED 1
caput FBCK:FB01:TR02:M6CAMODE 0

caput FBCK:FB01:TR02:M7DEVNAME BPMS:IN20:925:X
caput FBCK:FB01:TR02:M7USED 0
caput FBCK:FB01:TR02:M7CAMODE 0

caput FBCK:FB01:TR02:M8DEVNAME BPMS:IN20:945:X
caput FBCK:FB01:TR02:M8USED 0
caput FBCK:FB01:TR02:M8CAMODE 0

caput FBCK:FB01:TR02:M9DEVNAME BPMS:IN20:981:X
caput FBCK:FB01:TR02:M9USED 0
caput FBCK:FB01:TR02:M9CAMODE 0

caput FBCK:FB01:TR02:M10DEVNAME BPMS:IN20:525:Y
caput FBCK:FB01:TR02:M10USED 0
caput FBCK:FB01:TR02:M10CAMODE 0

caput FBCK:FB01:TR02:M11DEVNAME BPMS:IN20:581:Y
caput FBCK:FB01:TR02:M11USED 1
caput FBCK:FB01:TR02:M11CAMODE 0

caput FBCK:FB01:TR02:M12DEVNAME BPMS:IN20:631:Y
caput FBCK:FB01:TR02:M12USED 1
caput FBCK:FB01:TR02:M12CAMODE 0

caput FBCK:FB01:TR02:M13DEVNAME BPMS:IN20:651:Y
caput FBCK:FB01:TR02:M13USED 1
caput FBCK:FB01:TR02:M13CAMODE 0

caput FBCK:FB01:TR02:M14DEVNAME BPMS:IN20:771:Y
caput FBCK:FB01:TR02:M14USED 1
caput FBCK:FB01:TR02:M14CAMODE 0

caput FBCK:FB01:TR02:M15DEVNAME BPMS:IN20:781:Y
caput FBCK:FB01:TR02:M15USED 1
caput FBCK:FB01:TR02:M15CAMODE 0

caput FBCK:FB01:TR02:M16DEVNAME BPMS:IN20:925:Y
caput FBCK:FB01:TR02:M16USED 0
caput FBCK:FB01:TR02:M16CAMODE 0

caput FBCK:FB01:TR02:M17DEVNAME BPMS:IN20:945:Y
caput FBCK:FB01:TR02:M17USED 0
caput FBCK:FB01:TR02:M17CAMODE 0

caput FBCK:FB01:TR02:M18DEVNAME BPMS:IN20:981:Y
caput FBCK:FB01:TR02:M18USED 0
caput FBCK:FB01:TR02:M18CAMODE 0

caput FBCK:FB01:TR02:M19USED 0
caput FBCK:FB01:TR02:M20USED 0

# Reference orbit
caput -a FBCK:FB01:TR02:REFORBIT 18 -0.0059313 -0.12648 -0.13236 0.051098 -0.082934 -0.10219 -0.0022403 -0.03867 0.10587 -0.16693 0 0 0 0 0 0 0 0 

# States
#caput FBCK:FB01:TR02:S1NAME FILE:/data/dat/X.Position
caput FBCK:FB01:TR02:S1NAME XPos
caput FBCK:FB01:TR02:S1USED 1

#caput FBCK:FB01:TR02:S2NAME FILE:/data/dat/X.Angle
caput FBCK:FB01:TR02:S2NAME XAng
caput FBCK:FB01:TR02:S2USED 1

#caput FBCK:FB01:TR02:S3NAME FILE:/data/dat/Y.Position
caput FBCK:FB01:TR02:S3NAME YPos
caput FBCK:FB01:TR02:S3USED 1

#caput FBCK:FB01:TR02:S4NAME FILE:/data/dat/Y.Angle
caput FBCK:FB01:TR02:S4NAME YAng
caput FBCK:FB01:TR02:S4USED 1

# Actuators
caput FBCK:FB01:TR02:A1DEVNAME NULL
caput FBCK:FB01:TR02:A1USED 1
caput FBCK:FB01:TR02:A1CAMODE 1

caput FBCK:FB01:TR02:A2DEVNAME NULL
caput FBCK:FB01:TR02:A2USED 1
caput FBCK:FB01:TR02:A2CAMODE 1

caput FBCK:FB01:TR02:A3DEVNAME NULL
caput FBCK:FB01:TR02:A3USED 1
caput FBCK:FB01:TR02:A3CAMODE 1

caput FBCK:FB01:TR02:A4DEVNAME NULL
caput FBCK:FB01:TR02:A4USED 1
caput FBCK:FB01:TR02:A4CAMODE 1

# Energy
caput -a FBCK:FB01:TR02:ACTENERGY 4 0.073564 0.135 0.073564 0.135

# Must set these up in order to have the config app work
caput FBCK:FB01:TR02:MEASNUM 18
caput FBCK:FB01:TR02:ACTNUM 4
caput FBCK:FB01:TR02:STATENUM 4
caput FBCK:FB01:TR02:PGAIN 0.1

# Enable loop
caput FBCK:FB01:TR02:INSTALLED 1
#caput FBCK:FB01:TR02:STATE 1

