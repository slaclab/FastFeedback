#!/bin/sh
# Longitudital Feedback

caput FBCK:FB03:TR01:STATE 0

# 10 Hz, pockcel_perm
#caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 8 0 0 0 32 0 0 54 0 0 0 0 0 0 0 0

# 0.5 Hz, pockcel_perm
caput -a FBCK:FB03:TR01:POI1 21 0 0 0 0 0 8 0 0 0 256 0 0 54 0 0 0 0 0 0 0 0

# F matrix
caput -a FBCK:FB03:TR01:FMATRIX 36 1 2 3 4 5 6 6 5 4 3 2 1 1 2 3 4 5 6 6 5 4 3 2 1 1 2 3 4 5 6 6 5 4 3 2 1

# G matrix
caput -a FBCK:FB03:TR01:GMATRIX 36 1 1 1 1 1 1 2 2 2 2 2 2 3 3 3 3 3 3 0.5 0.5 0.5 0.5 0.5 0.5 1 1 1 1 1 1 2 2 2 2 2 2

# Loop configs
caput FBCK:FB03:TR01:NAME Longitudinal
caput FBCK:FB03:TR01:INSTALLED 1
caput FBCK:FB03:TR01:TOTALPOI 1
caput FBCK:FB03:TR01:ITERATEFUNC Longitudinal

# Measurements
caput FBCK:FB03:TR01:M1DEVNAME FF:BPMS:M1:X
caput FBCK:FB03:TR01:M1USED 1
caput FBCK:FB03:TR01:M1CAMODE 1

caput FBCK:FB03:TR01:M2DEVNAME FF:BPMS:M2:X
caput FBCK:FB03:TR01:M2USED 0
caput FBCK:FB03:TR01:M2CAMODE 1

caput FBCK:FB03:TR01:M3DEVNAME FF:BPMS:M3:X
caput FBCK:FB03:TR01:M3USED 0
caput FBCK:FB03:TR01:M3CAMODE 1

caput FBCK:FB03:TR01:M4DEVNAME FF:BLEN:M4:IMAX
caput FBCK:FB03:TR01:M4USED 0
caput FBCK:FB03:TR01:M4CAMODE 1

caput FBCK:FB03:TR01:M5DEVNAME FF:BPMS:M5:X
caput FBCK:FB03:TR01:M5USED 0
caput FBCK:FB03:TR01:M5CAMODE 1

caput FBCK:FB03:TR01:M6DEVNAME FF:BLEN:M6:IMAX
caput FBCK:FB03:TR01:M6USED 0
caput FBCK:FB03:TR01:M6CAMODE 1

caput FBCK:FB03:TR01:M7DEVNAME FF:BPMS:M7:X
caput FBCK:FB03:TR01:M7USED 0
caput FBCK:FB03:TR01:M7CAMODE 1

caput FBCK:FB03:TR01:M8DEVNAME FF:BPMS:M8:X
caput FBCK:FB03:TR01:M8USED 0
caput FBCK:FB03:TR01:M8CAMODE 1

caput FBCK:FB03:TR01:M9DEVNAME FF:BPMS:M9:X
caput FBCK:FB03:TR01:M9USED 0
caput FBCK:FB03:TR01:M9CAMODE 1

caput FBCK:FB03:TR01:M10DEVNAME ""
caput FBCK:FB03:TR01:M11DEVNAME ""
caput FBCK:FB03:TR01:M12DEVNAME ""

caput FBCK:FB03:TR01:M10USED 0
caput FBCK:FB03:TR01:M11USED 0
caput FBCK:FB03:TR01:M12USED 0
caput FBCK:FB03:TR01:M13USED 0
caput FBCK:FB03:TR01:M14USED 0
caput FBCK:FB03:TR01:M15USED 0
caput FBCK:FB03:TR01:M16USED 0
caput FBCK:FB03:TR01:M17USED 0
caput FBCK:FB03:TR01:M18USED 0
caput FBCK:FB03:TR01:M19USED 0
caput FBCK:FB03:TR01:M20USED 0
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
caput -a FBCK:FB03:TR01:REFORBIT 10 0 0 0 0 0 0 0 0 0 0

# States
caput FBCK:FB03:TR01:S1NAME DL1.Energy
caput FBCK:FB03:TR01:S1USED 0

caput FBCK:FB03:TR01:S2NAME BC1.Energy
caput FBCK:FB03:TR01:S2USED 0

caput FBCK:FB03:TR01:S3NAME BC1.Current
caput FBCK:FB03:TR01:S3USED 0

caput FBCK:FB03:TR01:S4NAME BC2.Energy
caput FBCK:FB03:TR01:S4USED 0

caput FBCK:FB03:TR01:S5NAME BC2.Current
caput FBCK:FB03:TR01:S5USED 0

caput FBCK:FB03:TR01:S6NAME DL2.Energy
caput FBCK:FB03:TR01:S6USED 0

caput FBCK:FB03:TR01:S7USED 0
caput FBCK:FB03:TR01:S8USED 0
caput FBCK:FB03:TR01:S9USED 0
caput FBCK:FB03:TR01:S10USED 0

# Actuators
caput FBCK:FB03:TR01:A1DEVNAME FILE:/data/a1
caput FBCK:FB03:TR01:A1USED 1
caput FBCK:FB03:TR01:A1CAMODE 0

caput FBCK:FB03:TR01:A2DEVNAME FILE:/data/a2
caput FBCK:FB03:TR01:A2USED 0
caput FBCK:FB03:TR01:A2CAMODE 0

caput FBCK:FB03:TR01:A3DEVNAME FILE:/data/a3
caput FBCK:FB03:TR01:A3USED 0
caput FBCK:FB03:TR01:A3CAMODE 0

caput FBCK:FB03:TR01:A4DEVNAME FILE:/data/a4
caput FBCK:FB03:TR01:A4USED 0
caput FBCK:FB03:TR01:A4CAMODE 0

caput FBCK:FB03:TR01:A5DEVNAME FILE:/data/a6
caput FBCK:FB03:TR01:A5USED 0
caput FBCK:FB03:TR01:A5CAMODE 0

caput FBCK:FB03:TR01:A6DEVNAME FILE:/data/a7
caput FBCK:FB03:TR01:A6USED 0
caput FBCK:FB03:TR01:A6CAMODE 0

caput FBCK:FB03:TR01:A7CAMODE 0
caput FBCK:FB03:TR01:A8CAMODE 0
caput FBCK:FB03:TR01:A9CAMODE 0
caput FBCK:FB03:TR01:A10CAMODE 0

# Energy
caput -a FBCK:FB03:TR01:ACTENERGY 6 4.3 4.3 4.3 4.3 4.3 4.3

# Must set these up in order to have the config app work
caput FBCK:FB03:TR01:MEASNUM 9
caput FBCK:FB03:TR01:ACTNUM 6
caput FBCK:FB03:TR01:STATENUM 6
caput FBCK:FB03:TR01:PGAIN 0.1

# Enable loop
caput FBCK:FB03:TR01:INSTALLED 1
####caput FBCK:FB03:TR01:STATE 1

# Save the configuration
caput FBCK:FB03:TR01:SAVE 0
caput FBCK:FB03:TR01:SAVE 1
