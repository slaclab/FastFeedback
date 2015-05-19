#!/bin/sh
#
# Script to write the correct EGU/DESC fields for the
# Longitudinal feedback running on FB04:LG01
#
# Default EGU/DESC fields are generic and may not reflect
# the actual feedback configuration
#

caput FBCK:FB04:LG01:S1P1_S.EGU MeV
caput FBCK:FB04:LG01:S1P2_S.EGU MeV
caput FBCK:FB04:LG01:S1P3_S.EGU MeV
caput FBCK:FB04:LG01:S1P4_S.EGU MeV

caput FBCK:FB04:LG01:S1P1_S.DESC "DL1 Energy State (P1)"
caput FBCK:FB04:LG01:S1P2_S.DESC "DL1 Energy State (P2)"
caput FBCK:FB04:LG01:S1P3_S.DESC "DL1 Energy State (P3)"
caput FBCK:FB04:LG01:S1P4_S.DESC "DL1 Energy State (P4)"

caput FBCK:FB04:LG01:S2P1_S.EGU MeV
caput FBCK:FB04:LG01:S2P2_S.EGU MeV
caput FBCK:FB04:LG01:S2P3_S.EGU MeV
caput FBCK:FB04:LG01:S2P4_S.EGU MeV

caput FBCK:FB04:LG01:S2P1_S.DESC "BC1 Energy State (P1)"
caput FBCK:FB04:LG01:S2P2_S.DESC "BC1 Energy State (P2)"
caput FBCK:FB04:LG01:S2P3_S.DESC "BC1 Energy State (P3)"
caput FBCK:FB04:LG01:S2P4_S.DESC "BC1 Energy State (P4)"

caput FBCK:FB04:LG01:S3P1_S.EGU amps
caput FBCK:FB04:LG01:S3P2_S.EGU amps
caput FBCK:FB04:LG01:S3P3_S.EGU amps
caput FBCK:FB04:LG01:S3P4_S.EGU amps

caput FBCK:FB04:LG01:S3P1_S.DESC "BC1 Current State (P1)"
caput FBCK:FB04:LG01:S3P2_S.DESC "BC1 Current State (P2)"
caput FBCK:FB04:LG01:S3P3_S.DESC "BC1 Current State (P3)"
caput FBCK:FB04:LG01:S3P4_S.DESC "BC1 Current State (P4)"

caput FBCK:FB04:LG01:S4P1_S.EGU MeV
caput FBCK:FB04:LG01:S4P2_S.EGU MeV
caput FBCK:FB04:LG01:S4P3_S.EGU MeV
caput FBCK:FB04:LG01:S4P4_S.EGU MeV

caput FBCK:FB04:LG01:S4P1_S.DESC "BC2 Energy State (P1)"
caput FBCK:FB04:LG01:S4P2_S.DESC "BC2 Energy State (P2)"
caput FBCK:FB04:LG01:S4P3_S.DESC "BC2 Energy State (P3)"
caput FBCK:FB04:LG01:S4P4_S.DESC "BC2 Energy State (P4)"

caput FBCK:FB04:LG01:S5P1_S.EGU amps
caput FBCK:FB04:LG01:S5P2_S.EGU amps
caput FBCK:FB04:LG01:S5P3_S.EGU amps
caput FBCK:FB04:LG01:S5P4_S.EGU amps

caput FBCK:FB04:LG01:S5P1_S.DESC "BC2 Current State (P1)"
caput FBCK:FB04:LG01:S5P2_S.DESC "BC2 Current State (P2)"
caput FBCK:FB04:LG01:S5P3_S.DESC "BC2 Current State (P3)"
caput FBCK:FB04:LG01:S5P4_S.DESC "BC2 Current State (P4)"

caput FBCK:FB04:LG01:S6P1_S.EGU MeV
caput FBCK:FB04:LG01:S6P2_S.EGU MeV
caput FBCK:FB04:LG01:S6P3_S.EGU MeV
caput FBCK:FB04:LG01:S6P4_S.EGU MeV

caput FBCK:FB04:LG01:S6P1_S.DESC "DL2 Energy State (P1)"
caput FBCK:FB04:LG01:S6P2_S.DESC "DL2 Energy State (P2)"
caput FBCK:FB04:LG01:S6P3_S.DESC "DL2 Energy State (P3)"
caput FBCK:FB04:LG01:S6P4_S.DESC "DL2 Energy State (P4)"

caput FBCK:FB04:LG01:S1LOLO.EGU MeV
caput FBCK:FB04:LG01:S1HIHI.EGU MeV
caput FBCK:FB04:LG01:S1LOLO.DESC "DL1 Energy LOLO State"
caput FBCK:FB04:LG01:S1HIHI.DESC "DL1 Energy HIHI State"

caput FBCK:FB04:LG01:S2LOLO.EGU MeV
caput FBCK:FB04:LG01:S2HIHI.EGU MeV
caput FBCK:FB04:LG01:S2LOLO.DESC "BC1 Energy LOLO State"
caput FBCK:FB04:LG01:S2HIHI.DESC "BC1 Energy HIHI State"

caput FBCK:FB04:LG01:S3LOLO.EGU amps
caput FBCK:FB04:LG01:S3HIHI.EGU amps
caput FBCK:FB04:LG01:S3LOLO.DESC "BC1 Current LOLO State"
caput FBCK:FB04:LG01:S3HIHI.DESC "BC1 Current HIHI State"

caput FBCK:FB04:LG01:S4LOLO.EGU MeV
caput FBCK:FB04:LG01:S4HIHI.EGU MeV
caput FBCK:FB04:LG01:S4LOLO.DESC "BC2 Energy LOLO State"
caput FBCK:FB04:LG01:S4HIHI.DESC "BC2 Energy HIHI State"

caput FBCK:FB04:LG01:S5LOLO.EGU amps
caput FBCK:FB04:LG01:S5HIHI.EGU amps
caput FBCK:FB04:LG01:S5LOLO.DESC "BC2 Current LOLO State"
caput FBCK:FB04:LG01:S5HIHI.DESC "BC2 Current HIHI State"

caput FBCK:FB04:LG01:S6LOLO.EGU MeV
caput FBCK:FB04:LG01:S6HIHI.EGU MeV
caput FBCK:FB04:LG01:S6LOLO.DESC "DL2 Energy LOLO State"
caput FBCK:FB04:LG01:S6HIHI.DESC "DL2 Energy HIHI State"

caput FBCK:FB04:LG01:A1P1_S.EGU MeV
caput FBCK:FB04:LG01:A1P2_S.EGU MeV
caput FBCK:FB04:LG01:A1P3_S.EGU MeV
caput FBCK:FB04:LG01:A1P4_S.EGU MeV

caput FBCK:FB04:LG01:A1P1_S.DESC "L0B Amplitude (P1)"
caput FBCK:FB04:LG01:A1P2_S.DESC "L0B Amplitude (P2)"
caput FBCK:FB04:LG01:A1P3_S.DESC "L0B Amplitude (P3)"
caput FBCK:FB04:LG01:A1P4_S.DESC "L0B Amplitude (P4)"

caput FBCK:FB04:LG01:A2P1_S.EGU MeV
caput FBCK:FB04:LG01:A2P2_S.EGU MeV
caput FBCK:FB04:LG01:A2P3_S.EGU MeV
caput FBCK:FB04:LG01:A2P4_S.EGU MeV

caput FBCK:FB04:LG01:A2P1_S.DESC "L1S Amplitude (P1)"
caput FBCK:FB04:LG01:A2P2_S.DESC "L1S Amplitude (P2)"
caput FBCK:FB04:LG01:A2P3_S.DESC "L1S Amplitude (P3)"
caput FBCK:FB04:LG01:A2P4_S.DESC "L1S Amplitude (P4)"

caput FBCK:FB04:LG01:A3P1_S.EGU degs
caput FBCK:FB04:LG01:A3P2_S.EGU degs
caput FBCK:FB04:LG01:A3P3_S.EGU degs
caput FBCK:FB04:LG01:A3P4_S.EGU degs

caput FBCK:FB04:LG01:A3P1_S.DESC "L1S Phase (P1)"
caput FBCK:FB04:LG01:A3P2_S.DESC "L1S Phase (P2)"
caput FBCK:FB04:LG01:A3P3_S.DESC "L1S Phase (P3)"
caput FBCK:FB04:LG01:A3P4_S.DESC "L1S Phase (P4)"

caput FBCK:FB04:LG01:A4P1_S.EGU MeV
caput FBCK:FB04:LG01:A4P2_S.EGU MeV
caput FBCK:FB04:LG01:A4P3_S.EGU MeV
caput FBCK:FB04:LG01:A4P4_S.EGU MeV

caput FBCK:FB04:LG01:A4P1_S.DESC "L2 Amplitude (P1)"
caput FBCK:FB04:LG01:A4P2_S.DESC "L2 Amplitude (P2)"
caput FBCK:FB04:LG01:A4P3_S.DESC "L2 Amplitude (P3)"
caput FBCK:FB04:LG01:A4P4_S.DESC "L2 Amplitude (P4)"

caput FBCK:FB04:LG01:A5P1_S.EGU degs
caput FBCK:FB04:LG01:A5P2_S.EGU degs
caput FBCK:FB04:LG01:A5P3_S.EGU degs
caput FBCK:FB04:LG01:A5P4_S.EGU degs

caput FBCK:FB04:LG01:A5P1_S.DESC "L2 Phase (P1)"
caput FBCK:FB04:LG01:A5P2_S.DESC "L2 Phase (P2)"
caput FBCK:FB04:LG01:A5P3_S.DESC "L2 Phase (P3)"
caput FBCK:FB04:LG01:A5P4_S.DESC "L2 Phase (P4)"

caput FBCK:FB04:LG01:A6P1_S.EGU MeV
caput FBCK:FB04:LG01:A6P2_S.EGU MeV
caput FBCK:FB04:LG01:A6P3_S.EGU MeV
caput FBCK:FB04:LG01:A6P4_S.EGU MeV

caput FBCK:FB04:LG01:A6P1_S.DESC "L3 Amplitude (P1)"
caput FBCK:FB04:LG01:A6P2_S.DESC "L3 Amplitude (P2)"
caput FBCK:FB04:LG01:A6P3_S.DESC "L3 Amplitude (P3)"
caput FBCK:FB04:LG01:A6P4_S.DESC "L3 Amplitude (P4)"

caput FBCK:FB04:LG01:A1LOLO.EGU MeV
caput FBCK:FB04:LG01:A1HIHI.EGU MeV
caput FBCK:FB04:LG01:A1LOLO.DESC "L0B Amplitude LOLO"
caput FBCK:FB04:LG01:A1HIHI.DESC "L0B Amplitude HIHI"

caput FBCK:FB04:LG01:A2LOLO.EGU MeV
caput FBCK:FB04:LG01:A2HIHI.EGU MeV
caput FBCK:FB04:LG01:A2LOLO.DESC "L1S Amplitude LOLO"
caput FBCK:FB04:LG01:A2HIHI.DESC "L1S Amplitude HIHI"

caput FBCK:FB04:LG01:A3LOLO.EGU degs
caput FBCK:FB04:LG01:A3HIHI.EGU degs
caput FBCK:FB04:LG01:A3LOLO.DESC "L1S Phase LOLO"
caput FBCK:FB04:LG01:A3HIHI.DESC "L1S Phase HIHI"

caput FBCK:FB04:LG01:A4LOLO.EGU MeV
caput FBCK:FB04:LG01:A4HIHI.EGU MeV
caput FBCK:FB04:LG01:A4LOLO.DESC "L2 Amplitude LOLO"
caput FBCK:FB04:LG01:A4HIHI.DESC "L2 Amplitude HIHI"

caput FBCK:FB04:LG01:A5LOLO.EGU degs
caput FBCK:FB04:LG01:A5HIHI.EGU degs
caput FBCK:FB04:LG01:A5LOLO.DESC "L2 Phase LOLO"
caput FBCK:FB04:LG01:A5HIHI.DESC "L2 Phase HIHI"

caput FBCK:FB04:LG01:A6LOLO.EGU MeV
caput FBCK:FB04:LG01:A6HIHI.EGU MeV
caput FBCK:FB04:LG01:A6LOLO.DESC "L3 Amplitude LOLO"
caput FBCK:FB04:LG01:A6HIHI.DESC "L3 Amplitude HIHI"

caput FBCK:FB04:LG01:M1P1_S.EGU mm
caput FBCK:FB04:LG01:M1P2_S.EGU mm
caput FBCK:FB04:LG01:M1P3_S.EGU mm
caput FBCK:FB04:LG01:M1P4_S.EGU mm
caput FBCK:FB04:LG01:M1LOLO.EGU mm
caput FBCK:FB04:LG01:M1HIHI.EGU mm

caput FBCK:FB04:LG01:M1P1_S.DESC "BPMS:IN20:731:X (P1)"
caput FBCK:FB04:LG01:M1P2_S.DESC "BPMS:IN20:731:X (P2)"
caput FBCK:FB04:LG01:M1P3_S.DESC "BPMS:IN20:731:X (P3)"
caput FBCK:FB04:LG01:M1P4_S.DESC "BPMS:IN20:731:X (P4)"
caput FBCK:FB04:LG01:M1LOLO.DESC "BPMS:IN20:731:X LOLO"
caput FBCK:FB04:LG01:M1HIHI.DESC "BPMS:IN20:731:X HIHI"

caput FBCK:FB04:LG01:M2P1_S.EGU mm
caput FBCK:FB04:LG01:M2P2_S.EGU mm
caput FBCK:FB04:LG01:M2P3_S.EGU mm
caput FBCK:FB04:LG01:M2P4_S.EGU mm
caput FBCK:FB04:LG01:M2LOLO.EGU mm
caput FBCK:FB04:LG01:M2HIHI.EGU mm

caput FBCK:FB04:LG01:M2P1_S.DESC "BPMS:IN20:981:X (P1)"
caput FBCK:FB04:LG01:M2P2_S.DESC "BPMS:IN20:981:X (P2)"
caput FBCK:FB04:LG01:M2P3_S.DESC "BPMS:IN20:981:X (P3)"
caput FBCK:FB04:LG01:M2P4_S.DESC "BPMS:IN20:981:X (P4)"
caput FBCK:FB04:LG01:M2LOLO.DESC "BPMS:IN20:981:X LOLO"
caput FBCK:FB04:LG01:M2HIHI.DESC "BPMS:IN20:981:X HIHI"

caput FBCK:FB04:LG01:M3P1_S.EGU mm
caput FBCK:FB04:LG01:M3P2_S.EGU mm
caput FBCK:FB04:LG01:M3P3_S.EGU mm
caput FBCK:FB04:LG01:M3P4_S.EGU mm
caput FBCK:FB04:LG01:M3LOLO.EGU mm
caput FBCK:FB04:LG01:M3HIHI.EGU mm

caput FBCK:FB04:LG01:M3P1_S.DESC "BPMS:LI20:233:X (P1)"
caput FBCK:FB04:LG01:M3P2_S.DESC "BPMS:LI20:233:X (P2)"
caput FBCK:FB04:LG01:M3P3_S.DESC "BPMS:LI20:233:X (P3)"
caput FBCK:FB04:LG01:M3P4_S.DESC "BPMS:LI20:233:X (P4)"
caput FBCK:FB04:LG01:M3LOLO.DESC "BPMS:LI20:233:X LOLO"
caput FBCK:FB04:LG01:M3HIHI.DESC "BPMS:LI20:233:X HIHI"

caput FBCK:FB04:LG01:M4P1_S.EGU l
caput FBCK:FB04:LG01:M4P2_S.EGU l
caput FBCK:FB04:LG01:M4P3_S.EGU l
caput FBCK:FB04:LG01:M4P4_S.EGU l
caput FBCK:FB04:LG01:M4LOLO.EGU l
caput FBCK:FB04:LG01:M4HIHI.EGU l

caput FBCK:FB04:LG01:M4P1_S.DESC "BLEN:LI21:265:AIMAX (P1)"
caput FBCK:FB04:LG01:M4P2_S.DESC "BLEN:LI21:265:AIMAX (P2)"
caput FBCK:FB04:LG01:M4P3_S.DESC "BLEN:LI21:265:AIMAX (P3)"
caput FBCK:FB04:LG01:M4P4_S.DESC "BLEN:LI21:265:AIMAX (P4)"
caput FBCK:FB04:LG01:M4LOLO.DESC "BLEN:LI21:265:AIMAX LOLO"
caput FBCK:FB04:LG01:M4HIHI.DESC "BLEN:LI21:265:AIMAX HIHI"

caput FBCK:FB04:LG01:M5P1_S.EGU mm
caput FBCK:FB04:LG01:M5P2_S.EGU mm
caput FBCK:FB04:LG01:M5P3_S.EGU mm
caput FBCK:FB04:LG01:M5P4_S.EGU mm
caput FBCK:FB04:LG01:M5LOLO.EGU mm
caput FBCK:FB04:LG01:M5HIHI.EGU mm

caput FBCK:FB04:LG01:M5P1_S.DESC "BPMS:LI24:801:X (P1)"
caput FBCK:FB04:LG01:M5P2_S.DESC "BPMS:LI24:801:X (P2)"
caput FBCK:FB04:LG01:M5P3_S.DESC "BPMS:LI24:801:X (P3)"
caput FBCK:FB04:LG01:M5P4_S.DESC "BPMS:LI24:801:X (P4)"
caput FBCK:FB04:LG01:M5LOLO.DESC "BPMS:LI24:801:X LOLO"
caput FBCK:FB04:LG01:M5HIHI.DESC "BPMS:LI24:801:X HIHI"

caput FBCK:FB04:LG01:M6P1_S.EGU l
caput FBCK:FB04:LG01:M6P2_S.EGU l
caput FBCK:FB04:LG01:M6P3_S.EGU l
caput FBCK:FB04:LG01:M6P4_S.EGU l
caput FBCK:FB04:LG01:M6LOLO.EGU l
caput FBCK:FB04:LG01:M6HIHI.EGU l

caput FBCK:FB04:LG01:M6P1_S.DESC "BLEN:LI24:866:BIMAX (P1)"
caput FBCK:FB04:LG01:M6P2_S.DESC "BLEN:LI24:866:BIMAX (P2)"
caput FBCK:FB04:LG01:M6P3_S.DESC "BLEN:LI24:866:BIMAX (P3)"
caput FBCK:FB04:LG01:M6P4_S.DESC "BLEN:LI24:866:BIMAX (P4)"
caput FBCK:FB04:LG01:M6LOLO.DESC "BLEN:LI24:866:BIMAX LOLO"
caput FBCK:FB04:LG01:M6HIHI.DESC "BLEN:LI24:866:BIMAX HIHI"

caput FBCK:FB04:LG01:M7P1_S.EGU mm
caput FBCK:FB04:LG01:M7P2_S.EGU mm
caput FBCK:FB04:LG01:M7P3_S.EGU mm
caput FBCK:FB04:LG01:M7P4_S.EGU mm
caput FBCK:FB04:LG01:M7LOLO.EGU mm
caput FBCK:FB04:LG01:M7HIHI.EGU mm

caput FBCK:FB04:LG01:M7P1_S.DESC "BPMS:BSY0:52:X (P1)"
caput FBCK:FB04:LG01:M7P2_S.DESC "BPMS:BSY0:52:X (P2)"
caput FBCK:FB04:LG01:M7P3_S.DESC "BPMS:BSY0:52:X (P3)"
caput FBCK:FB04:LG01:M7P4_S.DESC "BPMS:BSY0:52:X (P4)"
caput FBCK:FB04:LG01:M7LOLO.DESC "BPMS:BSY0:52:X LOLO"
caput FBCK:FB04:LG01:M7HIHI.DESC "BPMS:BSY0:52:X HIHI"

caput FBCK:FB04:LG01:M8P1_S.EGU mm
caput FBCK:FB04:LG01:M8P2_S.EGU mm
caput FBCK:FB04:LG01:M8P3_S.EGU mm
caput FBCK:FB04:LG01:M8P4_S.EGU mm
caput FBCK:FB04:LG01:M8LOLO.EGU mm
caput FBCK:FB04:LG01:M8HIHI.EGU mm

caput FBCK:FB04:LG01:M8P1_S.DESC "BPMS:LTU1:170:X (P1)"
caput FBCK:FB04:LG01:M8P2_S.DESC "BPMS:LTU1:170:X (P2)"
caput FBCK:FB04:LG01:M8P3_S.DESC "BPMS:LTU1:170:X (P3)"
caput FBCK:FB04:LG01:M8P4_S.DESC "BPMS:LTU1:170:X (P4)"
caput FBCK:FB04:LG01:M8LOLO.DESC "BPMS:LTU1:170:X LOLO"
caput FBCK:FB04:LG01:M8HIHI.DESC "BPMS:LTU1:170:X HIHI"

caput FBCK:FB04:LG01:M9P1_S.EGU mm
caput FBCK:FB04:LG01:M9P2_S.EGU mm
caput FBCK:FB04:LG01:M9P3_S.EGU mm
caput FBCK:FB04:LG01:M9P4_S.EGU mm
caput FBCK:FB04:LG01:M9LOLO.EGU mm
caput FBCK:FB04:LG01:M9HIHI.EGU mm

caput FBCK:FB04:LG01:M9P1_S.DESC "BPMS:LTU1:250:X (P1)"
caput FBCK:FB04:LG01:M9P2_S.DESC "BPMS:LTU1:250:X (P2)"
caput FBCK:FB04:LG01:M9P3_S.DESC "BPMS:LTU1:250:X (P3)"
caput FBCK:FB04:LG01:M9P4_S.DESC "BPMS:LTU1:250:X (P4)"
caput FBCK:FB04:LG01:M9LOLO.DESC "BPMS:LTU1:250:X LOLO"
caput FBCK:FB04:LG01:M9HIHI.DESC "BPMS:LTU1:250:X HIHI"

caput FBCK:FB04:LG01:M10P1_S.EGU mm
caput FBCK:FB04:LG01:M10P2_S.EGU mm
caput FBCK:FB04:LG01:M10P3_S.EGU mm
caput FBCK:FB04:LG01:M10P4_S.EGU mm
caput FBCK:FB04:LG01:M10LOLO.EGU mm
caput FBCK:FB04:LG01:M10HIHI.EGU mm

caput FBCK:FB04:LG01:M10P1_S.DESC "BPMS:LTU1:450:X (P1)"
caput FBCK:FB04:LG01:M10P2_S.DESC "BPMS:LTU1:450:X (P2)"
caput FBCK:FB04:LG01:M10P3_S.DESC "BPMS:LTU1:450:X (P3)"
caput FBCK:FB04:LG01:M10P4_S.DESC "BPMS:LTU1:450:X (P4)"
caput FBCK:FB04:LG01:M10LOLO.DESC "BPMS:LTU1:450:X LOLO"
caput FBCK:FB04:LG01:M10HIHI.DESC "BPMS:LTU1:450:X HIHI"

