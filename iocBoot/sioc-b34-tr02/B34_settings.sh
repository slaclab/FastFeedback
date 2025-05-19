#!/bin/sh
# === Feedback Configuration for FB16:TR01 ===
caput FBCK:FB16:TR01:NAME            "DEV Launch" > /dev/null
caput FBCK:FB16:TR01:ITERATEFUNC     TrajectoryFitBy1 > /dev/null
#caput FBCK:FB16:TR01:ITERATEFUNC     LongitudinalChirp > /dev/null
caput FBCK:FB16:TR01:TOTALPOI        4  > /dev/null
caput FBCK:FB16:TR01:SELECTALLDEVICES NO  > /dev/null
caput FBCK:FB16:TR01:MEASNUM         6  > /dev/null
caput FBCK:FB16:TR01:ACTNUM          4  > /dev/null
caput FBCK:FB16:TR01:STATENUM        4  > /dev/null
caput FBCK:FB16:TR01:PGAIN           0.03  > /dev/null
caput FBCK:FB16:TR01:IGAIN           0  > /dev/null
caput FBCK:FB16:TR01:M1DEVNAME       SIOC:B34:TR01:HEARTBEAT
#caput FBCK:FB16:TR01:M2DEVNAME       SIOC:B34:TR01:HEARTBEAT
#caput FBCK:FB16:TR01:M3DEVNAME       SIOC:B34:TR01:HEARTBEAT
#caput FBCK:FB16:TR01:M4DEVNAME       SIOC:B34:TR01:HEARTBEAT
#caput FBCK:FB16:TR01:M5DEVNAME       SIOC:B34:TR01:HEARTBEAT
#caput FBCK:FB16:TR01:M6DEVNAME       SIOC:B34:TR01:HEARTBEAT
caput FBCK:FB16:TR01:M2DEVNAME       NULL
caput FBCK:FB16:TR01:M3DEVNAME       NULL
caput FBCK:FB16:TR01:M4DEVNAME       NULL
caput FBCK:FB16:TR01:M5DEVNAME       NULL
caput FBCK:FB16:TR01:M6DEVNAME       NULL
caput FBCK:FB16:TR01:M7DEVNAME       NULL
caput FBCK:FB16:TR01:M8DEVNAME       NULL
caput FBCK:FB16:TR01:M9DEVNAME       NULL
caput FBCK:FB16:TR01:M10DEVNAME      NULL
caput FBCK:FB16:TR01:M11DEVNAME      NULL
caput FBCK:FB16:TR01:M12DEVNAME      NULL
caput FBCK:FB16:TR01:M13DEVNAME      NULL
caput FBCK:FB16:TR01:M14DEVNAME      NULL
caput FBCK:FB16:TR01:M15DEVNAME      NULL
caput FBCK:FB16:TR01:M16DEVNAME      NULL
caput FBCK:FB16:TR01:M17DEVNAME      NULL
caput FBCK:FB16:TR01:M18DEVNAME      NULL
caput FBCK:FB16:TR01:M19DEVNAME      NULL
caput FBCK:FB16:TR01:M20DEVNAME      NULL
caput FBCK:FB16:TR01:M1USED          YES  > /dev/null
#caput FBCK:FB16:TR01:M2USED          YES  > /dev/null
#caput FBCK:FB16:TR01:M3USED          YES  > /dev/null
#caput FBCK:FB16:TR01:M4USED          YES  > /dev/null
#caput FBCK:FB16:TR01:M5USED          YES  > /dev/null
#caput FBCK:FB16:TR01:M6USED          YES  > /dev/null
caput FBCK:FB16:TR01:M2USED          NO  > /dev/null
caput FBCK:FB16:TR01:M3USED          NO  > /dev/null
caput FBCK:FB16:TR01:M4USED          NO  > /dev/null
caput FBCK:FB16:TR01:M5USED          NO  > /dev/null
caput FBCK:FB16:TR01:M6USED          NO  > /dev/null
caput FBCK:FB16:TR01:M7USED          NO  > /dev/null
caput FBCK:FB16:TR01:M8USED          NO  > /dev/null
caput FBCK:FB16:TR01:M9USED          NO  > /dev/null
caput FBCK:FB16:TR01:M10USED         NO  > /dev/null
caput FBCK:FB16:TR01:M11USED         NO  > /dev/null
caput FBCK:FB16:TR01:M12USED         NO  > /dev/null
caput FBCK:FB16:TR01:M13USED         NO  > /dev/null
caput FBCK:FB16:TR01:M14USED         NO  > /dev/null
caput FBCK:FB16:TR01:M15USED         NO  > /dev/null
caput FBCK:FB16:TR01:M16USED         NO  > /dev/null
caput FBCK:FB16:TR01:M17USED         NO  > /dev/null
caput FBCK:FB16:TR01:M18USED         NO  > /dev/null
caput FBCK:FB16:TR01:M19USED         NO  > /dev/null
caput FBCK:FB16:TR01:M20USED         NO  > /dev/null
caput FBCK:FB16:TR01:M1CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:M2CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:M3CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:M4CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:M5CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:M6CAMODE        YES  > /dev/null
caput FBCK:FB16:TR01:M2CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:M3CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:M4CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:M5CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:M6CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:M7CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:M8CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:M9CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:M10CAMODE       NO  > /dev/null
caput FBCK:FB16:TR01:M11CAMODE       NO  > /dev/null
caput FBCK:FB16:TR01:M12CAMODE       NO  > /dev/null
caput FBCK:FB16:TR01:M13CAMODE       NO  > /dev/null
caput FBCK:FB16:TR01:M14CAMODE       NO  > /dev/null
caput FBCK:FB16:TR01:M15CAMODE       NO  > /dev/null
caput FBCK:FB16:TR01:M16CAMODE       NO  > /dev/null
caput FBCK:FB16:TR01:M17CAMODE       NO  > /dev/null
caput FBCK:FB16:TR01:M18CAMODE       NO  > /dev/null
caput FBCK:FB16:TR01:M19CAMODE       NO  > /dev/null
caput FBCK:FB16:TR01:M20CAMODE       NO  > /dev/null
caput FBCK:FB16:TR01:A1DEVNAME       XCOR:LTUH:558:BCTRL > /dev/null
caput FBCK:FB16:TR01:A2DEVNAME       NULL > /dev/null
caput FBCK:FB16:TR01:A3DEVNAME       NULL > /dev/null
caput FBCK:FB16:TR01:A4DEVNAME       NULL > /dev/null
caput FBCK:FB16:TR01:A5DEVNAME       NULL > /dev/null
caput FBCK:FB16:TR01:A6DEVNAME       NULL > /dev/null
caput FBCK:FB16:TR01:A7DEVNAME       NULL  > /dev/null
caput FBCK:FB16:TR01:A8DEVNAME       NULL  > /dev/null
caput FBCK:FB16:TR01:A9DEVNAME       NULL  > /dev/null
caput FBCK:FB16:TR01:A10DEVNAME      NULL  > /dev/null
caput FBCK:FB16:TR01:A1USED          NO  > /dev/null
caput FBCK:FB16:TR01:A2USED          NO  > /dev/null
caput FBCK:FB16:TR01:A3USED          NO  > /dev/null
caput FBCK:FB16:TR01:A4USED          NO  > /dev/null
caput FBCK:FB16:TR01:A5USED          NO  > /dev/null
caput FBCK:FB16:TR01:A6USED          NO  > /dev/null
caput FBCK:FB16:TR01:A7USED          NO  > /dev/null
caput FBCK:FB16:TR01:A8USED          NO  > /dev/null
caput FBCK:FB16:TR01:A9USED          NO  > /dev/null
caput FBCK:FB16:TR01:A10USED         NO  > /dev/null
#caput FBCK:FB16:TR01:A1CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:A2CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:A3CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:A4CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:A5CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:A6CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:A7CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:A8CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:A9CAMODE        YES  > /dev/null
#caput FBCK:FB16:TR01:A10CAMODE       YES  > /dev/null
caput FBCK:FB16:TR01:A1CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:A2CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:A3CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:A4CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:A5CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:A6CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:A7CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:A8CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:A9CAMODE        NO  > /dev/null
caput FBCK:FB16:TR01:A10CAMODE       NO  > /dev/null
caput FBCK:FB16:TR01:S1NAME          "X.Pos"  > /dev/null
caput FBCK:FB16:TR01:S2NAME          "X.Ang" > /dev/null
caput FBCK:FB16:TR01:S3NAME          "Y.Pos"  > /dev/null
caput FBCK:FB16:TR01:S4NAME          "Y.Ang"  > /dev/null
caput FBCK:FB16:TR01:S5NAME          "" > /dev/null
caput FBCK:FB16:TR01:S6NAME          "" > /dev/null
caput FBCK:FB16:TR01:S7NAME          "" > /dev/null
caput FBCK:FB16:TR01:S8NAME          "" > /dev/null
caput FBCK:FB16:TR01:S9NAME          "" > /dev/null
caput FBCK:FB16:TR01:S10NAME         "" > /dev/null
caput FBCK:FB16:TR01:S1USED          YES  > /dev/null
caput FBCK:FB16:TR01:S2USED          YES  > /dev/null
caput FBCK:FB16:TR01:S3USED          YES  > /dev/null
caput FBCK:FB16:TR01:S4USED          YES  > /dev/null
caput FBCK:FB16:TR01:S5USED          NO  > /dev/null
caput FBCK:FB16:TR01:S6USED          NO  > /dev/null
caput FBCK:FB16:TR01:S7USED          NO  > /dev/null
caput FBCK:FB16:TR01:S8USED          NO  > /dev/null
caput FBCK:FB16:TR01:S9USED          NO  > /dev/null
caput FBCK:FB16:TR01:S10USED         NO  > /dev/null
caput FBCK:FB16:TR01:M1HIHI          12  > /dev/null
caput FBCK:FB16:TR01:M2HIHI          12  > /dev/null
caput FBCK:FB16:TR01:M3HIHI          12  > /dev/null
caput FBCK:FB16:TR01:M4HIHI          12  > /dev/null
caput FBCK:FB16:TR01:M5HIHI          12  > /dev/null
caput FBCK:FB16:TR01:M6HIHI          12  > /dev/null
caput FBCK:FB16:TR01:M7HIHI          12  > /dev/null
caput FBCK:FB16:TR01:M8HIHI          12  > /dev/null
caput FBCK:FB16:TR01:M9HIHI          12  > /dev/null
caput FBCK:FB16:TR01:M10HIHI         12  > /dev/null
caput FBCK:FB16:TR01:M11HIHI         12  > /dev/null
caput FBCK:FB16:TR01:M12HIHI         12  > /dev/null
caput FBCK:FB16:TR01:M13HIHI         12  > /dev/null
caput FBCK:FB16:TR01:M14HIHI         12  > /dev/null
caput FBCK:FB16:TR01:M15HIHI         12  > /dev/null
caput FBCK:FB16:TR01:M16HIHI         12  > /dev/null
caput FBCK:FB16:TR01:M17HIHI         12  > /dev/null
caput FBCK:FB16:TR01:M18HIHI         12  > /dev/null
caput FBCK:FB16:TR01:M19HIHI         12  > /dev/null
caput FBCK:FB16:TR01:M20HIHI         12  > /dev/null
caput FBCK:FB16:TR01:M1LOLO          -12  > /dev/null
caput FBCK:FB16:TR01:M2LOLO          -12  > /dev/null
caput FBCK:FB16:TR01:M3LOLO          -12  > /dev/null
caput FBCK:FB16:TR01:M4LOLO          -12  > /dev/null
caput FBCK:FB16:TR01:M5LOLO          -12  > /dev/null
caput FBCK:FB16:TR01:M6LOLO          -12  > /dev/null
caput FBCK:FB16:TR01:M7LOLO          -12  > /dev/null
caput FBCK:FB16:TR01:M8LOLO          -12  > /dev/null
caput FBCK:FB16:TR01:M9LOLO          -12  > /dev/null
caput FBCK:FB16:TR01:M10LOLO         -12  > /dev/null
caput FBCK:FB16:TR01:M11LOLO         -12  > /dev/null
caput FBCK:FB16:TR01:M12LOLO         -12  > /dev/null
caput FBCK:FB16:TR01:M13LOLO         -12  > /dev/null
caput FBCK:FB16:TR01:M14LOLO         -12  > /dev/null
caput FBCK:FB16:TR01:M15LOLO         -12  > /dev/null
caput FBCK:FB16:TR01:M16LOLO         -12  > /dev/null
caput FBCK:FB16:TR01:M17LOLO         -12  > /dev/null
caput FBCK:FB16:TR01:M18LOLO         -12  > /dev/null
caput FBCK:FB16:TR01:M19LOLO         -12  > /dev/null
caput FBCK:FB16:TR01:M20LOLO         -12  > /dev/null
caput FBCK:FB16:TR01:A1HIHI          0.048  > /dev/null
caput FBCK:FB16:TR01:A2HIHI          0.048  > /dev/null
caput FBCK:FB16:TR01:A3HIHI          0.048  > /dev/null
caput FBCK:FB16:TR01:A4HIHI          0.048  > /dev/null
caput FBCK:FB16:TR01:A5HIHI          0.021  > /dev/null
caput FBCK:FB16:TR01:A6HIHI          0.021  > /dev/null
caput FBCK:FB16:TR01:A7HIHI          0.021  > /dev/null
caput FBCK:FB16:TR01:A8HIHI          0.021  > /dev/null
caput FBCK:FB16:TR01:A9HIHI          0.021  > /dev/null
caput FBCK:FB16:TR01:A10HIHI         0.021  > /dev/null
caput FBCK:FB16:TR01:A1LOLO          -0.048  > /dev/null
caput FBCK:FB16:TR01:A2LOLO          -0.048  > /dev/null
caput FBCK:FB16:TR01:A3LOLO          -0.048  > /dev/null
caput FBCK:FB16:TR01:A4LOLO          -0.048  > /dev/null
caput FBCK:FB16:TR01:A5LOLO          -0.021  > /dev/null
caput FBCK:FB16:TR01:A6LOLO          -0.021  > /dev/null
caput FBCK:FB16:TR01:A7LOLO          -0.021  > /dev/null
caput FBCK:FB16:TR01:A8LOLO          -0.021  > /dev/null
caput FBCK:FB16:TR01:A9LOLO          -0.021  > /dev/null
caput FBCK:FB16:TR01:A10LOLO         -0.021  > /dev/null
caput FBCK:FB16:TR01:S1HIHI          10  > /dev/null
caput FBCK:FB16:TR01:S2HIHI          10  > /dev/null
caput FBCK:FB16:TR01:S3HIHI          10  > /dev/null
caput FBCK:FB16:TR01:S4HIHI          10  > /dev/null
caput FBCK:FB16:TR01:S5HIHI          3  > /dev/null
caput FBCK:FB16:TR01:S6HIHI          3  > /dev/null
caput FBCK:FB16:TR01:S7HIHI          3  > /dev/null
caput FBCK:FB16:TR01:S8HIHI          3  > /dev/null
caput FBCK:FB16:TR01:S9HIHI          3  > /dev/null
caput FBCK:FB16:TR01:S10HIHI         3  > /dev/null
caput FBCK:FB16:TR01:S1LOLO          -10  > /dev/null
caput FBCK:FB16:TR01:S2LOLO          -10  > /dev/null
caput FBCK:FB16:TR01:S3LOLO          -10  > /dev/null
caput FBCK:FB16:TR01:S4LOLO          -10  > /dev/null
caput FBCK:FB16:TR01:S5LOLO          -3  > /dev/null
caput FBCK:FB16:TR01:S6LOLO          -3  > /dev/null
caput FBCK:FB16:TR01:S7LOLO          -3  > /dev/null
caput FBCK:FB16:TR01:S8LOLO          -3  > /dev/null
caput FBCK:FB16:TR01:S9LOLO          -3  > /dev/null
caput FBCK:FB16:TR01:S10LOLO         -3  > /dev/null
caput -a FBCK:FB16:TR01:FMATRIX 200 1 -6.19215e-17 0 0 -1.73472e-18 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0  > /dev/null
caput -a FBCK:FB16:TR01:GMATRIX 200 8.02303 0 0 8.12022 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0  > /dev/null
caput -a FBCK:FB16:TR01:MEASDSPR 40 1 1 1 1 1 1 1 1 1 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0  > /dev/null
caput -a FBCK:FB16:TR01:ACTENERGY 10 0.08 0.08 0.08 0.08 0 0 0 0 0 0  > /dev/null
caput -a FBCK:FB16:TR01:REFORBIT 40 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0  > /dev/null
# FOR LONGITUDINAL TEST:
#caput -a FBCK:FB16:TR01:POI1 21 0 0 0 0 0 0 0 0 0 0 55 0 0 128 0 0 0 0 0 0 0  > /dev/null
#caput -a FBCK:FB16:TR01:POI2 21 0 0 0 128 0 0 0 0 0 0 62 0 0 0 0 0 0 0 0 0 0  > /dev/null
#caput -a FBCK:FB16:TR01:POI3 21 0 0 0 128 0 0 0 0 0 0 55 0 0 0 0 0 0 0 0 0 0  > /dev/null
#caput -a FBCK:FB16:TR01:POI4 21 0 0 0 0 0 0 0 0 0 0 62 0 0 128 0 0 0 0 0 0 0  > /dev/null
# FOR LTUH MAGNET TEST:
caput -a FBCK:FB16:TR01:POI1 21 0 0 0 8 0 0 0 16 0 0 55 16384 0 128 0 0 0 0 0 0 0
caput -a FBCK:FB16:TR01:POI2 21 0 0 0 0 0 0 0 16 0 0 62 9728 0 129 0 0 0 0 0 0 0
caput -a FBCK:FB16:TR01:POI3 21 0 0 0 0 0 0 0 0 0 0 55 26112 0 128 0 0 0 16 0 0 0
caput -a FBCK:FB16:TR01:POI4 21 0 0 0 0 0 0 0 0 0 0 62 9728 0 129 0 0 0 16 0 0 0

#caput -a FBCK:FB16:TR01:POI1 21 0 0 0 8 0 0 0 32 0 0 55 0 0 0 0 0 0 0 0 0 0  > /dev/null
#caput -a FBCK:FB16:TR01:POI2 21 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0  > /dev/null
#caput -a FBCK:FB16:TR01:POI3 21 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0  > /dev/null
#caput -a FBCK:FB16:TR01:POI4 21 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0  > /dev/null
caput FBCK:FB16:TR01:INSTALLED 1 > /dev/null
echo === Restored configuration for FB16:TR01 ===
