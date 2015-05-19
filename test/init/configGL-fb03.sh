#!/bin/sh

caput FBCK:FB03:TR01:STATE 0
caput -a FBCK:FB03:TR01:POI1 6 12 23 41 23 32 53

# F matrix
caput -a FBCK:FB03:TR01:FMATRIX 20 1 0 0 0 -0.61 2.19 0 0 0 0 1 0 0 0 1.89 2.26 0 0.1 0.1 0.1 0.1

# G matrix
caput -a FBCK:FB03:TR01:GMATRIX 10 0.1 0.1 0.1 0.1 0.1 0.1 0.1 1 1 1

# Loop configs
caput FBCK:FB03:TR01:NAME TestLoop
caput FBCK:FB03:TR01:INSTALLED 1
caput FBCK:FB03:TR01:TOTALPOI 1
caput FBCK:FB03:TR01:ITERATEFUNC TrajectoryFit

# Measurements
caput FBCK:FB03:TR01:M1DEVNAME FF:BPMS:IN20:371:X
caput FBCK:FB03:TR01:M1USED 1
caput FBCK:FB03:TR01:M1CAMODE 1
caput FF:BPMS:IN20:371:X 1
caput FF:BPMS:IN20:371:X.EGU mm

caput FBCK:FB03:TR01:M2DEVNAME FF:BPMS:IN20:425:X
caput FBCK:FB03:TR01:M2USED 1
caput FBCK:FB03:TR01:M2CAMODE 1
caput FF:BPMS:IN20:425:X 3
caput FF:BPMS:IN20:425:X.EGU mm

caput FBCK:FB03:TR01:M3DEVNAME FF:BPMS:IN20:371:Y
caput FBCK:FB03:TR01:M3USED 1
caput FBCK:FB03:TR01:M3CAMODE 1
caput FF:BPMS:IN20:371:Y 2
caput FF:BPMS:IN20:371:Y.EGU mm

caput FBCK:FB03:TR01:M4DEVNAME FF:BPMS:IN20:425:Y
caput FBCK:FB03:TR01:M4USED 1
caput FBCK:FB03:TR01:M4CAMODE 1
caput FF:BPMS:IN20:425:Y 4
caput FF:BPMS:IN20:425:Y.EGU mm

# Reference Orbit
caput -a FBCK:FB03:TR01:REFORBIT 4 0.1 0.2 0.3 0.4

# States
#caput FBCK:FB03:TR01:S1NAME FILE:/tmp/X.Position
caput FBCK:FB03:TR01:S1NAME NULL
caput FBCK:FB03:TR01:S1USED 1

#caput FBCK:FB03:TR01:S2NAME FILE:/tmp/X.Angle
caput FBCK:FB03:TR01:S2NAME NULL
caput FBCK:FB03:TR01:S2USED 1

#caput FBCK:FB03:TR01:S3NAME FILE:/tmp/Y.Position
caput FBCK:FB03:TR01:S3NAME NULL
caput FBCK:FB03:TR01:S3USED 1

#caput FBCK:FB03:TR01:S4NAME FILE:/tmp/Y.Angle
caput FBCK:FB03:TR01:S4NAME NULL
caput FBCK:FB03:TR01:S4USED 1

# Actuators
caput FBCK:FB03:TR01:A1DEVNAME FF:XCOR:IN20:221:BCTRL
caput FBCK:FB03:TR01:A1USED 1
caput FBCK:FB03:TR01:A1CAMODE 1

caput FBCK:FB03:TR01:A2DEVNAME FF:YCOR:IN20:222:BCTRL
caput FBCK:FB03:TR01:A2USED 1
caput FBCK:FB03:TR01:A2CAMODE 1

caput FBCK:FB03:TR01:A3DEVNAME FF:XCOR:IN20:381:BCTRL
caput FBCK:FB03:TR01:A3USED 0
caput FBCK:FB03:TR01:A3CAMODE 1

caput FBCK:FB03:TR01:A4DEVNAME FF:YCOR:IN20:382:BCTRL
caput FBCK:FB03:TR01:A4USED 0
caput FBCK:FB03:TR01:A4CAMODE 1

# Reference Orbit
caput -a FBCK:FB03:TR01:ACTENERGY 2 0.13 0.11 

# Write same values to the test bpms before enabling the loop
caput FF:BPMS:IN20:371:X 0.1
caput FF:BPMS:IN20:425:X -0.05
caput FF:BPMS:IN20:371:Y 0.04
caput FF:BPMS:IN20:425:Y -0.02

caput FF:BPMS:IN20:371:FBCK OFF
caput FF:BPMS:IN20:425:FBCK OFF

caput FF:BPMS:IN20:371:TMIT 2e9
caput FF:BPMS:IN20:425:TMIT 2e9

caput FF:XCOR:IN20:221:BCTRL 0.333
caput FF:YCOR:IN20:222:BCTRL 0.555

caput FF:XCOR:IN20:221:FBCK OFF
caput FF:XCOR:IN20:222:FBCK OFF

#enable loop
caput FBCK:FB03:TR01:INSTALLED 1
caput FBCK:FB03:TR01:STATE ON
