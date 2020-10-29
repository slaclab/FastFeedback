# FastFeedback
## The NC LINAC FastFeedback EPICS Application

# Contents
1. [Documentation](#documentation)
2. [Directory Tree](#directory-tree)
3. [List of IOCs](#list-of-iocs)


## Documentation
Links to various bits of information about the FastFeedback system are below. A unified FastFeedback documentation source is in progress.

[Assorted Documentation](https://confluence.slac.stanford.edu/display/~rreno/Fast+Feedback+assorted+Documentation?flashId=-195444716) PowerPoints, mostly

## Directory Tree
```
. Build system <TOP>
+-- bin
|   +-- linuxRT-x86_64     # fastFeedback and ffUnitTest binaries
+-- configure              # Build system configuration files
+-- cpuBoot                # linuxRT CPU configuration files
|    +-- dev               # dev CPUs
|    +-- lcls              # prod LCLS CPUs
+-- fastFeedbackApp
|   +-- Db
|   |   +-- common         # Database files common to all Feedback IOCs
|   |   +-- iocs           # Database files specific to a Feedback IOC
|   +-- src                # Source directory. Contains state notation files, database definitions, and the main entry point
|   |   +-- algo           # C++ sources for feedback algorithms
|   |   +-- exec__         # C sources and database definitions to register EPICS interface functions
|   |   +-- framework      # C++ sources for the Fast Feedback framework
|   |   +-- test           # Unit Test sources
|   |   +-- util           # Utility functions - logging, time, and EPICS device support
+-- iocBoot                # Common and IOC specific startup scripts
|   +-- common
|   +-- sioc-AREA-UNIT
|   ...
+-- test                   # integration testing development support files
+-- tools                  # misc. support scripts
```


## List of IOCs

### Global
| Name              | CPU               | Function         | FB      |Loop   |VEVR # | Notes                                            |
|:-----------------:|:-----------------:|:----------------:|:-------:|:-----:|:-----:|:-------------------------------------------------|
| sioc-sys0-fb01    | cpu-sys0-fb01     | Watchdog         | N/A     | N/A   | N/A   | Moved from lcls-daemon1 in R3.2.0 (Aug 25, 2020) |
| sioc-sys0-lg01    | cpu-sys0-fb02     | HXR Longitudinal | FB04    | LG01  | 3     |                                                  |
| sioc-sys0-lg02    | cpu-sys0-fb01     | SXR Longitudinal | FB03    | LG01  | 10    | Not yet commissioned                             |
| sioc-sys0-gn01    | cpu-sys0-fb02     | BunchCharge      | FB02    | GN01  | 2     |                                                  |

### S20                                                               
| Name              | CPU               | Function         | FB      |Loop   |VEVR # | Notes                                            |
|:-----------------:|:-----------------:|:----------------:|:-------:|:-----:|:-----:|:-------------------------------------------------|
| sioc-in20-tr01    | cpu-sys0-fb02     | Gun Launch       | FB01    | TR01  | 0     |                                                  |
| sioc-in20-tr02    | cpu-sys0-fb02     | Injector Launch  | FB01    | TR02  | 1     |                                                  |
| sioc-in20-tr03    | cpu-sys0-fb01     | Injector Launch1 | FB02    | TR05  | 8     |                                                  |
| sioc-in20-tr04    | cpu-sys0-fb01     | Injector Launch2 | FB04    | TR04  | 9     |                                                  |
| sioc-in20-tr05    | cpu-sys0-fb02     | Injector Launch3 | FB03    | TR03  | 7     |                                                  |

### S21                                                               
| Name              | CPU               | Function         | FB      |Loop   |VEVR # | Notes                                            |
|:-----------------:|:-----------------:|:----------------:|:-------:|:-----:|:-----:|:-------------------------------------------------|
| sioc-li21-tr01    | cpu-sys0-fb01     | XCAV Launch      | FB01    | TR03  |  1    |                                                  |
| sioc-li21-tr02    | cpu-sys0-fb01     | L2 Launch        | FB01    | TR04  |  2    |                                                  |

### S24                                                               
| Name              | CPU               | Function         | FB      |Loop   |VEVR # | Notes                                            |
|:-----------------:|:-----------------:|:----------------:|:-------:|:-----:|:-----:|:-------------------------------------------------|
| sioc-li24-tr01    | cpu-sys0-fb01     | L3 Launch        | FB02    | TR01  | 3     |                                                  |

### S28                                                               
| Name              | CPU               | Function         | FB      |Loop   |VEVR # | Notes                                            |
|:-----------------:|:-----------------:|:----------------:|:-------:|:-----:|:-----:|:-------------------------------------------------|
| sioc-li28-tr01    | cpu-sys0-fb01     | LI28 Launch      | FB02    | TR02  | 4     |                                                  |

### BSY
| Name              | CPU               | Function         | FB      |Loop   |VEVR # | Notes                                            |
|:-----------------:|:-----------------:|:----------------:|:-------:|:-----:|:-----:|:-------------------------------------------------|
| sioc-bsy0-tr01    | cpu-sys0-fb01     | BSY X            | FB01    | TR05  | 0     |                                                  |
| sioc-bsy0-tr01    | cpu-sys0-fb02     | BSY Y            | FB05    | TR01  | 6     |                                                  |

### LTU
| Name              | CPU               | Function         | FB      |Loop   |VEVR # | Notes                                            |
|:-----------------:|:-----------------:|:----------------:|:-------:|:-----:|:-----:|:-------------------------------------------------|
| sioc-ltuh-tr01    | cpu-sys0-fb01     | Slow LTUH1       | FB02    | TR03  | 5     |                                                  |
| sioc-ltuh-tr02    | cpu-sys0-fb01     | Slow LTUH2       | FB02    | TR04  | 6     |                                                  |
| sioc-ltuh-tr03    | cpu-sys0-fb01     | LTUH Launch      | FB03    | TR01  | 7     |                                                  |
| sioc-ltus-tr01    | cpu-sys0-fb01     | LTUS Launch      | FB04    | TR01  | 11    |                                                  |

### Undulator Hall
| Name              | CPU               | Function         | FB      |Loop   |VEVR # | Notes                                            |
|:-----------------:|:-----------------:|:----------------:|:-------:|:-----:|:-----:|:-------------------------------------------------|
| sioc-undh-tr01    | cpu-sys0-fb02     | UNDH Launch      | FB03    | TR04  | 4     |                                                  |
| sioc-undh-tr02    | cpu-sys0-fb02     | UNDH BLD         | FB05    | TR05  | 6     |                                                  |
| sioc-unds-tr01    | cpu-sys0-fb01     | UNDS Launch      | FB04    | TR02  | 12    |                                                  |
| sioc-unds-tr02    | cpu-sys0-fb01     | UNDS BLD         | FB03    | TR02  | 13    | Not yet commissioned                             |
