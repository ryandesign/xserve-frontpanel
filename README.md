# xserve-frontpanel
This project contains a small program that demonstrates how to drive the Intel Xserve's front panel CPU activity LEDs. I've only tested it on a 2009 Dual Xeon Xserve, but it should work on any Intel-based Xserve.

It's a work in progress. Currently, only the CPU activity LEDs are working.

Eventually, I hope to be able to drive all features of the Xserve (IPMI interaction over BMC, CPU-activity linked front panel LEDs).

On OS X Lion Server-era systems, all of this was done by the hwmond daemon, which made use of the _PlatformHardwareManagement_ framework. These binaries cannot (easily) be ported to newer OSes, so I started work on a re-implementation.

---
## Building
Currently, this project should build & run under macOS and Linux (but I might have broken something – please let me know!). Make sure you have `libusb` installed.

```bash
cmake . && make
```

## Running
```bash
./hwmond
```

### Running on Linux
On modern Linux systems that use udev, the front panel might be attached read-only:

```
libusb: error [_get_usbfs_fd] libusb couldn't open USB device /dev/bus/usb/002/004: Permission denied
libusb: error [_get_usbfs_fd] libusb requires write access to USB device nodes.
Couldn't connect to front panel! (Are you running this on an Intel Xserve?)
```

To fix this, copy the provided rules file into rules.d directory:

```bash
sudo cp udev/98-apple-xserve-frontpanel.rules /lib/udev/rules.d/
```

Then reload the udev rules:

```bash
sudo udevadm control --reload-rules
```

And tell udev to trigger the rule for the front panel:

```bash
sudo udevadm trigger --attr-match=idVendor=05ac --attr-match=idProduct=8261
```

On subsequent system startups, the rule will be triggered automatically.
