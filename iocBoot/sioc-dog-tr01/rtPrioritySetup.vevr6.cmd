export PATH=/usr/bin:$PATH

### The current properties of the thread can be viewed with this:
###    ps  -Leo pid,tid,rtprio,comm | grep vevr6

### set kernel read thread priority for the vevr6

/usr/bin/chrt -pf 95  `ps  -Leo tid,comm | /usr/bin/awk '/evrma_vevr6/'`
/usr/bin/chrt -pf 96  `ps  -Leo pid,comm | /usr/bin/awk '/irq.*pci-evrm/'`

