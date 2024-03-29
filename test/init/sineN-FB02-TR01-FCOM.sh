#!/bin/sh

caput FBCK:FB02:TR01:STATE 0

# 10 Hz, pockcel_perm
#caput -a FBCK:FB02:TR01:POI1 21 0 0 0 0 0 8 0 0 0 32 0 0 54 0 0 0 0 0 0 0 0

# 1 Hz
caput -a FBCK:FB02:TR01:POI1 21 0 0 0 0 0 8 0 0 0 128 0 0 54 0 0 0 0 0 0 0 0

# Loop configs
caput FBCK:FB02:TR01:NAME SineN
caput FBCK:FB02:TR01:INSTALLED 1
caput FBCK:FB02:TR01:TOTALPOI 1
caput FBCK:FB02:TR01:ITERATEFUNC SineN

# Measurements
caput FBCK:FB02:TR01:M1DEVNAME BPMS:IN20:581:X
caput FBCK:FB02:TR01:M1USED 1
caput FBCK:FB02:TR01:M1CAMODE 0

caput FBCK:FB02:TR01:M2DEVNAME BPMS:IN20:631:X
caput FBCK:FB02:TR01:M2USED 1
caput FBCK:FB02:TR01:M2CAMODE 0

caput FBCK:FB02:TR01:M3DEVNAME BPMS:IN20:581:Y
caput FBCK:FB02:TR01:M3USED 1
caput FBCK:FB02:TR01:M3CAMODE 0

caput FBCK:FB02:TR01:M4DEVNAME BPMS:IN20:631:Y
caput FBCK:FB02:TR01:M4USED 1
caput FBCK:FB02:TR01:M4CAMODE 0

caput FBCK:FB02:TR01:M5USED 0
caput FBCK:FB02:TR01:M6USED 0
caput FBCK:FB02:TR01:M7USED 0
caput FBCK:FB02:TR01:M8USED 0
caput FBCK:FB02:TR01:M9USED 0
caput FBCK:FB02:TR01:M10USED 0
caput FBCK:FB02:TR01:M11USED 0
caput FBCK:FB02:TR01:M12USED 0
caput FBCK:FB02:TR01:M13USED 0
caput FBCK:FB02:TR01:M14USED 0
caput FBCK:FB02:TR01:M15USED 0
caput FBCK:FB02:TR01:M16USED 0
caput FBCK:FB02:TR01:M17USED 0
caput FBCK:FB02:TR01:M18USED 0
caput FBCK:FB02:TR01:M19USED 0
caput FBCK:FB02:TR01:M20USED 0
caput FBCK:FB02:TR01:M21USED 0
caput FBCK:FB02:TR01:M22USED 0
caput FBCK:FB02:TR01:M23USED 0
caput FBCK:FB02:TR01:M24USED 0
caput FBCK:FB02:TR01:M25USED 0
caput FBCK:FB02:TR01:M26USED 0
caput FBCK:FB02:TR01:M27USED 0
caput FBCK:FB02:TR01:M28USED 0
caput FBCK:FB02:TR01:M29USED 0
caput FBCK:FB02:TR01:M30USED 0
caput FBCK:FB02:TR01:M31USED 0
caput FBCK:FB02:TR01:M32USED 0
caput FBCK:FB02:TR01:M33USED 0
caput FBCK:FB02:TR01:M34USED 0
caput FBCK:FB02:TR01:M35USED 0
caput FBCK:FB02:TR01:M36USED 0
caput FBCK:FB02:TR01:M37USED 0
caput FBCK:FB02:TR01:M38USED 0
caput FBCK:FB02:TR01:M39USED 0
caput FBCK:FB02:TR01:M40USED 0

# States
caput FBCK:FB02:TR01:S1NAME XPos
caput FBCK:FB02:TR01:S1USED 1

caput FBCK:FB02:TR01:S2NAME YAng
caput FBCK:FB02:TR01:S2USED 1

caput FBCK:FB02:TR01:S3NAME YPos
caput FBCK:FB02:TR01:S3USED 1

caput FBCK:FB02:TR01:S4NAME YAng
caput FBCK:FB02:TR01:S4USED 1

# Actuators
caput FBCK:FB02:TR01:A1DEVNAME NULL
caput FBCK:FB02:TR01:A1USED 1
caput FBCK:FB02:TR01:A1CAMODE 1

caput FBCK:FB02:TR01:A2DEVNAME NULL
caput FBCK:FB02:TR01:A2USED 1
caput FBCK:FB02:TR01:A2CAMODE 1

caput FBCK:FB02:TR01:A3DEVNAME NULL
caput FBCK:FB02:TR01:A3USED 1
caput FBCK:FB02:TR01:A3CAMODE 1

caput FBCK:FB02:TR01:A4DEVNAME NULL
caput FBCK:FB02:TR01:A4USED 1
caput FBCK:FB02:TR01:A4CAMODE 1

# Energy
caput -a FBCK:FB02:TR01:ACTENERGY 4 0.073564 0.135 0.073564 0.135

# Must set these up in order to have the config app work
caput FBCK:FB02:TR01:MEASNUM 4
caput FBCK:FB02:TR01:ACTNUM 4
caput FBCK:FB02:TR01:STATENUM 4
caput FBCK:FB02:TR01:PGAIN 0.1

# Enable loop
caput FBCK:FB02:TR01:INSTALLED 1
#caput FBCK:FB02:TR01:STATE 1

