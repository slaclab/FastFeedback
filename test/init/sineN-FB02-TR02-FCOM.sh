#!/bin/sh

caput FBCK:FB02:TR02:STATE 0

# 10 Hz, pockcel_perm
#caput -a FBCK:FB02:TR02:POI1 21 0 0 0 0 0 8 0 0 0 32 0 0 54 0 0 0 0 0 0 0 0

# 1 Hz
caput -a FBCK:FB02:TR02:POI1 21 0 0 0 0 0 8 0 0 0 128 0 0 54 0 0 0 0 0 0 0 0

# Loop configs
caput FBCK:FB02:TR02:NAME SineN
caput FBCK:FB02:TR02:INSTALLED 1
caput FBCK:FB02:TR02:TOTALPOI 1
caput FBCK:FB02:TR02:ITERATEFUNC SineN

# Measurements
caput FBCK:FB02:TR02:M1DEVNAME BPMS:IN20:651:X
caput FBCK:FB02:TR02:M1USED 1
caput FBCK:FB02:TR02:M1CAMODE 0

caput FBCK:FB02:TR02:M2DEVNAME BPMS:IN20:771:X
caput FBCK:FB02:TR02:M2USED 1
caput FBCK:FB02:TR02:M2CAMODE 0

caput FBCK:FB02:TR02:M3DEVNAME BPMS:IN20:651:Y
caput FBCK:FB02:TR02:M3USED 1
caput FBCK:FB02:TR02:M3CAMODE 0

caput FBCK:FB02:TR02:M4DEVNAME BPMS:IN20:771:Y
caput FBCK:FB02:TR02:M4USED 1
caput FBCK:FB02:TR02:M4CAMODE 0

caput FBCK:FB02:TR02:M5USED 0
caput FBCK:FB02:TR02:M6USED 0
caput FBCK:FB02:TR02:M7USED 0
caput FBCK:FB02:TR02:M8USED 0
caput FBCK:FB02:TR02:M9USED 0
caput FBCK:FB02:TR02:M10USED 0
caput FBCK:FB02:TR02:M11USED 0
caput FBCK:FB02:TR02:M12USED 0
caput FBCK:FB02:TR02:M13USED 0
caput FBCK:FB02:TR02:M14USED 0
caput FBCK:FB02:TR02:M15USED 0
caput FBCK:FB02:TR02:M16USED 0
caput FBCK:FB02:TR02:M17USED 0
caput FBCK:FB02:TR02:M18USED 0
caput FBCK:FB02:TR02:M19USED 0
caput FBCK:FB02:TR02:M20USED 0
caput FBCK:FB02:TR02:M21USED 0
caput FBCK:FB02:TR02:M22USED 0
caput FBCK:FB02:TR02:M23USED 0
caput FBCK:FB02:TR02:M24USED 0
caput FBCK:FB02:TR02:M25USED 0
caput FBCK:FB02:TR02:M26USED 0
caput FBCK:FB02:TR02:M27USED 0
caput FBCK:FB02:TR02:M28USED 0
caput FBCK:FB02:TR02:M29USED 0
caput FBCK:FB02:TR02:M30USED 0
caput FBCK:FB02:TR02:M31USED 0
caput FBCK:FB02:TR02:M32USED 0
caput FBCK:FB02:TR02:M33USED 0
caput FBCK:FB02:TR02:M34USED 0
caput FBCK:FB02:TR02:M35USED 0
caput FBCK:FB02:TR02:M36USED 0
caput FBCK:FB02:TR02:M37USED 0
caput FBCK:FB02:TR02:M38USED 0
caput FBCK:FB02:TR02:M39USED 0
caput FBCK:FB02:TR02:M40USED 0

# States
caput FBCK:FB02:TR02:S1USED 0
caput FBCK:FB02:TR02:S2USED 0
caput FBCK:FB02:TR02:S3USED 0
caput FBCK:FB02:TR02:S4USED 0
caput FBCK:FB02:TR02:S5USED 0
caput FBCK:FB02:TR02:S6USED 0
caput FBCK:FB02:TR02:S7USED 0
caput FBCK:FB02:TR02:S8USED 0
caput FBCK:FB02:TR02:S9USED 0
caput FBCK:FB02:TR02:S10USED 0

caput FBCK:FB02:TR02:S1NAME XPos
caput FBCK:FB02:TR02:S1USED 1

caput FBCK:FB02:TR02:S2NAME YAng
caput FBCK:FB02:TR02:S2USED 1

caput FBCK:FB02:TR02:S3NAME YPos
caput FBCK:FB02:TR02:S3USED 1

caput FBCK:FB02:TR02:S4NAME YAng
caput FBCK:FB02:TR02:S4USED 1

# Actuators
caput FBCK:FB02:TR02:A1DEVNAME NULL
caput FBCK:FB02:TR02:A1USED 1
caput FBCK:FB02:TR02:A1CAMODE 1

caput FBCK:FB02:TR02:A2DEVNAME NULL
caput FBCK:FB02:TR02:A2USED 1
caput FBCK:FB02:TR02:A2CAMODE 1

caput FBCK:FB02:TR02:A3DEVNAME NULL
caput FBCK:FB02:TR02:A3USED 1
caput FBCK:FB02:TR02:A3CAMODE 1

caput FBCK:FB02:TR02:A4DEVNAME NULL
caput FBCK:FB02:TR02:A4USED 1
caput FBCK:FB02:TR02:A4CAMODE 1

caput FBCK:FB02:TR02:A5USED 0
caput FBCK:FB02:TR02:A6USED 0
caput FBCK:FB02:TR02:A7USED 0
caput FBCK:FB02:TR02:A8USED 0
caput FBCK:FB02:TR02:A9USED 0
caput FBCK:FB02:TR02:A10USED 0

# Energy
caput -a FBCK:FB02:TR02:ACTENERGY 4 0.073564 0.135 0.073564 0.135

# Must set these up in order to have the config app work
caput FBCK:FB02:TR02:MEASNUM 4
caput FBCK:FB02:TR02:ACTNUM 4
caput FBCK:FB02:TR02:STATENUM 4
caput FBCK:FB02:TR02:PGAIN 0.1

# Enable loop
caput FBCK:FB02:TR02:INSTALLED 1
#caput FBCK:FB02:TR02:STATE 1

