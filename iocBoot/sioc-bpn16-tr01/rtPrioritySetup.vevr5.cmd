export PATH=/usr/bin:$PATH

### The current properties of the thread can be viewed with this:
###    ps  -Leo pid,tid,rtprio,comm | grep vevr5

### set kernel read thread priority for the vevr5

/usr/bin/chrt -pf 95  `ps  -Leo tid,comm | /usr/bin/awk '/evrma_vevr5/'`
/usr/bin/chrt -pf 96  `ps  -Leo pid,comm | /usr/bin/awk '/irq.*pci-evrm/'`

