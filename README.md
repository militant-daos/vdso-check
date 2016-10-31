# vdso-check
vDSO usage checker for Android.

The main purpose of this app is to find programs which use vDSO mechanism and have [vdso] section mapped into their address spaces (...to be used with one of Dirty COW exploits - https://github.com/scumjr/dirtycow-vdso ; we have the special interest in ones invoked by root account).

Usage:
- compile: ndk-build
- push to the device: adb push maps /data/local/tmp
- run: /data/local/tmp/maps

The app prints ps output for each program which is currently running and has vDSO mapping.
