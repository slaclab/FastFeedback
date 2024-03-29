#!/bin/sh

caput FBCK:FB02:TR01:STATE 0

# 10 Hz, pockcel_perm
caput -a FBCK:FB02:TR01:POI1 21 0 0 0 8 0 0 0 32 0 0 55 0 0 0 0 0 0 0 0 0 0

# 10 Hz
#caput -a FBCK:FB02:TR01:POI1 21 0 0 0 0 0 0 0 32 0 0 55 0 0 0 0 0 0 0 0 0 0


# F matrix
caput -a FBCK:FB02:TR01:FMATRIX 50 1 0 0 0 3 1.2594 12.3435 0 0 3 0.83855 18.2112 0 0 3 0.24137 47.0553 0 0 3 -0.079052 36.5814 0 0 3 0 0 1 0 3 0 0 0.74043 12.3423 3 0 0 0.87842 31.6382 3 0 0 0.14924 47.0399 3 0 0 -0.068908 62.3686 3

# G matrix
caput -a FBCK:FB02:TR01:GMATRIX 16 45.9045 11.944 0 0 0.34154 1.2517 0 0 0 0 61.7756 29.9152 0 0 -0.080604 0.85751

# Loop configs
caput FBCK:FB02:TR01:NAME LI28
caput FBCK:FB02:TR01:INSTALLED 1
caput FBCK:FB02:TR01:TOTALPOI 1
caput FBCK:FB02:TR01:ITERATEFUNC TrajectoryFit

# Measurements
caput FBCK:FB02:TR01:M1DEVNAME BPMS:LI28:601:X
caput FBCK:FB02:TR01:M1USED 0
caput FBCK:FB02:TR01:M1CAMODE 1

caput FBCK:FB02:TR01:M2DEVNAME BPMS:LI28:701:X
caput FBCK:FB02:TR01:M2USED 1
caput FBCK:FB02:TR01:M2CAMODE 1

caput FBCK:FB02:TR01:M3DEVNAME BPMS:LI28:801:X
caput FBCK:FB02:TR01:M3USED 1
caput FBCK:FB02:TR01:M3CAMODE 1

caput FBCK:FB02:TR01:M4DEVNAME BPMS:LI28:901:X
caput FBCK:FB02:TR01:M4USED 1
caput FBCK:FB02:TR01:M4CAMODE 1

caput FBCK:FB02:TR01:M5DEVNAME BPMS:LI29:201:X
caput FBCK:FB02:TR01:M5USED 0
caput FBCK:FB02:TR01:M5CAMODE 1

caput FBCK:FB02:TR01:M6DEVNAME BPMS:LI29:301:X
caput FBCK:FB02:TR01:M6USED 0
caput FBCK:FB02:TR01:M6CAMODE 1

caput FBCK:FB02:TR01:M7DEVNAME BPMS:LI29:401:X
caput FBCK:FB02:TR01:M7USED 1
caput FBCK:FB02:TR01:M7CAMODE 1

caput FBCK:FB02:TR01:M8DEVNAME BPMS:LI29:501:X
caput FBCK:FB02:TR01:M8USED 1
caput FBCK:FB02:TR01:M8CAMODE 1

caput FBCK:FB02:TR01:M9DEVNAME BPMS:LI28:601:Y
caput FBCK:FB02:TR01:M9USED 0
caput FBCK:FB02:TR01:M9CAMODE 1

caput FBCK:FB02:TR01:M10DEVNAME BPMS:LI28:701:Y
caput FBCK:FB02:TR01:M10USED 1
caput FBCK:FB02:TR01:M10CAMODE 1

caput FBCK:FB02:TR01:M11DEVNAME BPMS:LI28:801:Y
caput FBCK:FB02:TR01:M11USED 1
caput FBCK:FB02:TR01:M11CAMODE 1

caput FBCK:FB02:TR01:M12DEVNAME BPMS:LI28:901:Y
caput FBCK:FB02:TR01:M12USED 1
caput FBCK:FB02:TR01:M12CAMODE 1

caput FBCK:FB02:TR01:M13DEVNAME BPMS:LI29:201:Y
caput FBCK:FB02:TR01:M13USED 0
caput FBCK:FB02:TR01:M13CAMODE 1

caput FBCK:FB02:TR01:M14DEVNAME BPMS:LI29:301:Y
caput FBCK:FB02:TR01:M14USED 0
caput FBCK:FB02:TR01:M14CAMODE 1

caput FBCK:FB02:TR01:M15DEVNAME BPMS:LI29:401:Y
caput FBCK:FB02:TR01:M15USED 1
caput FBCK:FB02:TR01:M15CAMODE 1

caput FBCK:FB02:TR01:M16DEVNAME BPMS:LI29:501:Y
caput FBCK:FB02:TR01:M16USED 1
caput FBCK:FB02:TR01:M16CAMODE 1

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

# Reference orbit
caput -a FBCK:FB02:TR01:REFORBIT 18 -0.0059313 -0.12648 -0.13236 0.051098 -0.082934 -0.10219 -0.0022403 -0.03867 0.10587 -0.16693 0 0 0 0 0 0 0 0 

# States
caput FBCK:FB02:TR01:S1NAME X Position
caput FBCK:FB02:TR01:S1USED 1

caput FBCK:FB02:TR01:S2NAME X Angle
caput FBCK:FB02:TR01:S2USED 1

caput FBCK:FB02:TR01:S3NAME Y Position
caput FBCK:FB02:TR01:S3USED 1

caput FBCK:FB02:TR01:S4NAME Y Angle
caput FBCK:FB02:TR01:S4USED 1

caput FBCK:FB02:TR01:S5USED 0
caput FBCK:FB02:TR01:S6USED 0
caput FBCK:FB02:TR01:S7USED 0
caput FBCK:FB02:TR01:S8USED 0
caput FBCK:FB02:TR01:S9USED 0
caput FBCK:FB02:TR01:S10USED 0

# Actuators
caput FBCK:FB02:TR01:A1DEVNAME XCOR:LI28:202:BCTRL
caput FBCK:FB02:TR01:A1USED 1
caput FBCK:FB02:TR01:A1CAMODE 1

caput FBCK:FB02:TR01:A2DEVNAME XCOR:LI28:602:BCTRL
caput FBCK:FB02:TR01:A2USED 1
caput FBCK:FB02:TR01:A2CAMODE 1

caput FBCK:FB02:TR01:A3DEVNAME YCOR:LI27:900:BCTRL
caput FBCK:FB02:TR01:A3USED 1
caput FBCK:FB02:TR01:A3CAMODE 1

caput FBCK:FB02:TR01:A4DEVNAME YCOR:LI28:503:BCTRL
caput FBCK:FB02:TR01:A4USED 1
caput FBCK:FB02:TR01:A4CAMODE 1

caput FBCK:FB02:TR01:A5CAMODE 0
caput FBCK:FB02:TR01:A6CAMODE 0
caput FBCK:FB02:TR01:A7CAMODE 0
caput FBCK:FB02:TR01:A8CAMODE 0
caput FBCK:FB02:TR01:A9CAMODE 0
caput FBCK:FB02:TR01:A10CAMODE 0

# Energy
caput -a FBCK:FB02:TR01:ACTENERGY 4 10.6207 11.5319 10.4108 11.3436

# Must set these up in order to have the config app work
caput FBCK:FB02:TR01:MEASNUM 16
caput FBCK:FB02:TR01:ACTNUM 4
caput FBCK:FB02:TR01:STATENUM 4
caput FBCK:FB02:TR01:PGAIN 0.1

# Enable loop
caput FBCK:FB02:TR01:INSTALLED 1
#caput FBCK:FB02:TR01:STATE 1

