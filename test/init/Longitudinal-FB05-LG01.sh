#!/bin/sh
# Longitudital Feedback

caput FBCK:FB05:LG01:STATE 0

# 10 Hz, pockcel_perm
caput -a FBCK:FB05:LG01:POI1 21 0 0 0 0 0 8 0 0 0 32 0 0 54 0 0 0 0 0 0 0 0

# 120 Hz, 4 Patterns
caput -a FBCK:FB05:LG01:POI1 21 0 0 0 0 0 0 0 16 0 0 55 0 0 0 0 0 0 0 0 0 0
caput -a FBCK:FB05:LG01:POI2 21 0 0 0 0 0 0 0 16 0 0 62 0 0 0 0 0 0 0 0 0 0
caput -a FBCK:FB05:LG01:POI3 21 0 0 0 0 0 0 0 0 0 0 55 0 0 0 0 0 0 16 0 0 0
caput -a FBCK:FB05:LG01:POI4 21 0 0 0 0 0 0 0 0 0 0 62 0 0 0 0 0 0 16 0 0 0

# 0.5 HZ, pockcel_perm
#caput -a FBCK:FB05:LG01:POI1 21 0 0 0 0 0 8 0 0 0 256 0 0 54 0 0 0 0 0 0 0 0

# F matrix
caput -a FBCK:FB05:LG01:FMATRIX 36 1.046 0 0 0 0 0 -1.2184 2.1986 0.011464 0 0 0 -0.080912 -0.72954 -0.026117 0 0 0 -0.13831 -0.60952 -0.01451 0.87485 0.037702 0 -9.5878 203.654 0.022006 -0.45644 -0.057178 0 -0.0005691 0.0041105 0 -1.5381 0 2.4

# G matrix
#caput -a FBCK:FB05:LG01:GMATRIX 36 1 1 1 1 0.5 1 1 12 12 1 0.5 1 1 12 12 1 0.5 1 0 0 0 100 0 1 0 0 0 1 1 1 0 0 0 0 0.5 10
#caput -a FBCK:FB05:LG01:GMATRIX 36 1 1 1 1 0.5 1 1 1 1 1 0.5 1 1 1 1 1 0.5 1 0 0 0 100 0 1 0 0 0 1 1 1 0 0 0 0 0.5 10
caput -a FBCK:FB05:LG01:GMATRIX 36 1 0 0 0 0 0  0 1 0 0 0 0  0 0 1 0 0 0  0 0 0 100 0 0  0 0 0 0 1 0   0 0 0 0 0 10

# Loop configs
caput FBCK:FB05:LG01:NAME Longitudinal
caput FBCK:FB05:LG01:INSTALLED 1
caput FBCK:FB05:LG01:TOTALPOI 4
caput FBCK:FB05:LG01:ITERATEFUNC LongitudinalChirp

# Measurements
caput FBCK:FB05:LG01:M1DEVNAME BPMS:IN20:731:X
caput FBCK:FB05:LG01:M1USED 1
caput FBCK:FB05:LG01:M1CAMODE 0

caput FBCK:FB05:LG01:M2DEVNAME BPMS:IN20:981:X
caput FBCK:FB05:LG01:M2USED 1
caput FBCK:FB05:LG01:M2CAMODE 0

caput FBCK:FB05:LG01:M3DEVNAME BPMS:LI21:233:X
caput FBCK:FB05:LG01:M3USED 1
caput FBCK:FB05:LG01:M3CAMODE 0

caput FBCK:FB05:LG01:M4DEVNAME BLEN:LI21:265:AIMAXF2
caput FBCK:FB05:LG01:M4USED 1
caput FBCK:FB05:LG01:M4CAMODE 0
caput FBCK:FB05:LG01:M4.EGU I

#caput FBCK:FB05:LG01:M5DEVNAME BPMS:LTU1:620:X
#caput FBCK:FB05:LG01:M5DEVNAME BPMS:LI24:801:X
caput FBCK:FB05:LG01:M5DEVNAME BPMS:LI24:701:X
caput FBCK:FB05:LG01:M5USED 1
caput FBCK:FB05:LG01:M5CAMODE 0

caput FBCK:FB05:LG01:M6DEVNAME BLEN:LI24:886:BIMAXF2
caput FBCK:FB05:LG01:M6USED 1
caput FBCK:FB05:LG01:M6CAMODE 0
caput FBCK:FB05:LG01:M6.EGU I

caput FBCK:FB05:LG01:M7DEVNAME BPMS:BSY0:52:X
caput FBCK:FB05:LG01:M7USED 1
caput FBCK:FB05:LG01:M7CAMODE 0

caput FBCK:FB05:LG01:M8DEVNAME BPMS:LTU1:250:X
caput FBCK:FB05:LG01:M8USED 1
caput FBCK:FB05:LG01:M8CAMODE 0

caput FBCK:FB05:LG01:M9DEVNAME BPMS:LTU1:450:X
caput FBCK:FB05:LG01:M9USED 1
caput FBCK:FB05:LG01:M9CAMODE 0

caput FBCK:FB05:LG01:M10DEVNAME BPMS:LTU0:170:Y
caput FBCK:FB05:LG01:M10USED 1
caput FBCK:FB05:LG01:M10CAMODE 0

caput FBCK:FB05:LG01:M11USED 0
caput FBCK:FB05:LG01:M12USED 0
caput FBCK:FB05:LG01:M13USED 0
caput FBCK:FB05:LG01:M14USED 0
caput FBCK:FB05:LG01:M15USED 0
caput FBCK:FB05:LG01:M16USED 0
caput FBCK:FB05:LG01:M17USED 0
caput FBCK:FB05:LG01:M18USED 0
caput FBCK:FB05:LG01:M19USED 0
caput FBCK:FB05:LG01:M20USED 0
caput FBCK:FB05:LG01:M21USED 0
caput FBCK:FB05:LG01:M22USED 0
caput FBCK:FB05:LG01:M23USED 0
caput FBCK:FB05:LG01:M24USED 0
caput FBCK:FB05:LG01:M25USED 0
caput FBCK:FB05:LG01:M26USED 0
caput FBCK:FB05:LG01:M27USED 0
caput FBCK:FB05:LG01:M28USED 0
caput FBCK:FB05:LG01:M29USED 0
caput FBCK:FB05:LG01:M30USED 0
caput FBCK:FB05:LG01:M31USED 0
caput FBCK:FB05:LG01:M32USED 0
caput FBCK:FB05:LG01:M33USED 0
caput FBCK:FB05:LG01:M34USED 0
caput FBCK:FB05:LG01:M35USED 0
caput FBCK:FB05:LG01:M36USED 0
caput FBCK:FB05:LG01:M37USED 0
caput FBCK:FB05:LG01:M38USED 0
caput FBCK:FB05:LG01:M39USED 0
caput FBCK:FB05:LG01:M40USED 0

# Reference orbit
caput -a FBCK:FB05:LG01:REFORBIT 10 0 0 0 0 0 0 0 0 0 0

# States
caput FBCK:FB05:LG01:S1NAME DL1.Energy
caput FBCK:FB05:LG01:S1USED 1
caput FBCK:FB05:LG01:S1.EGU MeV
caput FBCK:FB05:LG01:S1LOLO -10000
caput FBCK:FB05:LG01:S1HIHI 10000
caput FBCK:FB05:LG01:S1.LOLO -1000
caput FBCK:FB05:LG01:S1.LOW -1000
caput FBCK:FB05:LG01:S1.HIGH 10000
caput FBCK:FB05:LG01:S1.HIHI 10000

caput FBCK:FB05:LG01:S2NAME BC1.Energy
caput FBCK:FB05:LG01:S2USED 1
caput FBCK:FB05:LG01:S2.EGU MeV
caput FBCK:FB05:LG01:S2LOLO -1000
caput FBCK:FB05:LG01:S2HIHI 10000
caput FBCK:FB05:LG01:S2.LOLO -1000
caput FBCK:FB05:LG01:S2.LOW -1000
caput FBCK:FB05:LG01:S2.HIGH 10000
caput FBCK:FB05:LG01:S2.HIHI 10000

caput FBCK:FB05:LG01:S3NAME BC1.Current
caput FBCK:FB05:LG01:S3USED 1
caput FBCK:FB05:LG01:S3.EGU amps
caput FBCK:FB05:LG01:S3LOLO -1000
caput FBCK:FB05:LG01:S3HIHI 10000
caput FBCK:FB05:LG01:S3.LOLO -1000
caput FBCK:FB05:LG01:S3.LOW -1000
caput FBCK:FB05:LG01:S3.HIGH 10000
caput FBCK:FB05:LG01:S3.HIHI 10000

caput FBCK:FB05:LG01:S4NAME BC2.Energy
caput FBCK:FB05:LG01:S4USED 1
caput FBCK:FB05:LG01:S4.EGU MeV
caput FBCK:FB05:LG01:S4LOLO -3700
caput FBCK:FB05:LG01:S4HIHI 6000
caput FBCK:FB05:LG01:S4.LOLO -3700
caput FBCK:FB05:LG01:S4.LOW -3700
caput FBCK:FB05:LG01:S4.HIGH 6000
caput FBCK:FB05:LG01:S4.HIHI 6000

caput FBCK:FB05:LG01:S5NAME BC2.Current
caput FBCK:FB05:LG01:S5USED 1
caput FBCK:FB05:LG01:S5.EGU amps
caput FBCK:FB05:LG01:S5LOLO -1000
caput FBCK:FB05:LG01:S5HIHI 6000
caput FBCK:FB05:LG01:S5.LOLO -1000
caput FBCK:FB05:LG01:S5.LOW -1000
caput FBCK:FB05:LG01:S5.HIGH 6000
caput FBCK:FB05:LG01:S5.HIHI 6000

caput FBCK:FB05:LG01:S6NAME DL2.Energy
caput FBCK:FB05:LG01:S6USED 1
caput FBCK:FB05:LG01:S6.EGU MeV
caput FBCK:FB05:LG01:S6LOLO -1500
caput FBCK:FB05:LG01:S6HIHI 15000
caput FBCK:FB05:LG01:S6.LOLO -1500
caput FBCK:FB05:LG01:S6.LOW -1500
caput FBCK:FB05:LG01:S6.HIGH 15000
caput FBCK:FB05:LG01:S6.HIHI 15000

caput FBCK:FB05:LG01:S7USED 0
caput FBCK:FB05:LG01:S8USED 0
caput FBCK:FB05:LG01:S9USED 0
caput FBCK:FB05:LG01:S10USED 0

# Actuators
caput FBCK:FB05:LG01:A1DEVNAME ACCL:IN20:400:L0B_ADES
#caput FBCK:FB05:LG01:A1DEVNAME NULL
caput FBCK:FB05:LG01:A1USED 1
caput FBCK:FB05:LG01:A1.EGU MeV
caput FBCK:FB05:LG01:A1CAMODE 0
caput FBCK:FB05:LG01:A1LOLO -10000
caput FBCK:FB05:LG01:A1HIHI 10000
caput FBCK:FB05:LG01:A1.LOLO -10000
caput FBCK:FB05:LG01:A1.LOW -10000
caput FBCK:FB05:LG01:A1.HIGH 10000
caput FBCK:FB05:LG01:A1.HIHI 10000

caput FBCK:FB05:LG01:A2DEVNAME ACCL:LI21:1:L1S_ADES
#caput FBCK:FB05:LG01:A2DEVNAME NULL
caput FBCK:FB05:LG01:A2USED 1
caput FBCK:FB05:LG01:A2.EGU MeV
caput FBCK:FB05:LG01:A2CAMODE 0
caput FBCK:FB05:LG01:A2LOLO -10000
caput FBCK:FB05:LG01:A2HIHI 10000
caput FBCK:FB05:LG01:A2.LOLO -10000
caput FBCK:FB05:LG01:A2.LOW -10000
caput FBCK:FB05:LG01:A2.HIGH 10000
caput FBCK:FB05:LG01:A2.HIHI 10000

caput FBCK:FB05:LG01:A3DEVNAME ACCL:LI21:1:L1S_PDES
#caput FBCK:FB05:LG01:A3DEVNAME NULL
caput FBCK:FB05:LG01:A3USED 1
caput FBCK:FB05:LG01:A3.EGU degs
caput FBCK:FB05:LG01:A3CAMODE 0
caput FBCK:FB05:LG01:A3LOLO -18000
caput FBCK:FB05:LG01:A3HIHI 18000
caput FBCK:FB05:LG01:A3.LOLO -18000
caput FBCK:FB05:LG01:A3.LOW -18000
caput FBCK:FB05:LG01:A3.HIGH 18000
caput FBCK:FB05:LG01:A3.HIHI 18000

caput FBCK:FB05:LG01:A4DEVNAME ACCL:LI22:1:ADES
#caput FBCK:FB05:LG01:A4DEVNAME ACCL:LI24:L2:ABSTR_ADES
#caput FBCK:FB05:LG01:A4DEVNAME NULL
caput FBCK:FB05:LG01:A4USED 1
caput FBCK:FB05:LG01:A4.EGU MeV
caput FBCK:FB05:LG01:A4CAMODE 0
caput FBCK:FB05:LG01:A4LOLO -20000
caput FBCK:FB05:LG01:A4HIHI 60000
caput FBCK:FB05:LG01:A4.LOLO -20000
caput FBCK:FB05:LG01:A4.LOW -20000
caput FBCK:FB05:LG01:A4.HIGH 60000
caput FBCK:FB05:LG01:A4.HIHI 60000

caput FBCK:FB05:LG01:A5DEVNAME ACCL:LI22:1:PDES 
#caput FBCK:FB05:LG01:A5DEVNAME ACCL:LI24:L2:ABSTR_PDES
#caput FBCK:FB05:LG01:A5DEVNAME NULL
caput FBCK:FB05:LG01:A5USED 1
caput FBCK:FB05:LG01:A5.EGU degs
caput FBCK:FB05:LG01:A5CAMODE 0
caput FBCK:FB05:LG01:A5LOLO -500000
caput FBCK:FB05:LG01:A5HIHI 1000000
caput FBCK:FB05:LG01:A5.LOLO -500000
caput FBCK:FB05:LG01:A5.LOW -500000
caput FBCK:FB05:LG01:A5.HIGH 1000000
caput FBCK:FB05:LG01:A5.HIHI 1000000

caput FBCK:FB05:LG01:A6DEVNAME ACCL:LI25:1:ADES
#caput FBCK:FB05:LG01:A6DEVNAME ACCL:LI29:L3:ABSTR_ADES
#caput FBCK:FB05:LG01:A6DEVNAME NULL
caput FBCK:FB05:LG01:A6USED 1
caput FBCK:FB05:LG01:A6.EGU MeV
caput FBCK:FB05:LG01:A6CAMODE 0
caput FBCK:FB05:LG01:A6LOLO -250000
caput FBCK:FB05:LG01:A6HIHI 1500000
caput FBCK:FB05:LG01:A6.LOLO -250000
caput FBCK:FB05:LG01:A6.LOW -250000
caput FBCK:FB05:LG01:A6.HIGH 1500000
caput FBCK:FB05:LG01:A6.HIHI 1500000

caput FBCK:FB05:LG01:A7CAMODE 0
caput FBCK:FB05:LG01:A8CAMODE 0
caput FBCK:FB05:LG01:A9CAMODE 0
caput FBCK:FB05:LG01:A10CAMODE 0

# Energy
caput -a FBCK:FB05:LG01:ACTENERGY 6 1 1 1 1 1 1

# Must set these up in order to have the config app work
caput FBCK:FB05:LG01:MEASNUM 10
caput FBCK:FB05:LG01:ACTNUM 6
caput FBCK:FB05:LG01:STATENUM 6
caput FBCK:FB05:LG01:PGAIN 0.0
caput FBCK:FB05:LG01:IGAIN 0.1
caput -a FBCK:FB05:LG01:MEASDSPR 10 -262.74 -762.046 -233.372 1 -387.133 1-77.6 97.703 -146.209 -18.7413

# Enable loop
caput FBCK:FB05:LG01:INSTALLED 1
####caput FBCK:FB05:LG01:STATE 1

# Save the configuration
caput FBCK:FB05:LG01:SAVE 0
caput FBCK:FB05:LG01:SAVE 1
