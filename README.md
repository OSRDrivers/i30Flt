# i30Flt #

@jonasLyk reported a REALLY interesting corruption error reported by NTFS:

https://twitter.com/jonasLyk/status/1347900440000811010

Triggering the notification only requires that you visit a particular path on an NTFS volume.

Our research indicates that the “file corrupt” error bubbles up from a network query open, so it’s sufficient to just call GetFileAttributes to see the behavior. 
We think the bug is in all the changes around case sensitivity...There’s a memory compare of “$i30” with “$I30” before the descent into chaos. Also if you use “$I30” 
in the offending command you don’t get the problem.

The directory is not really corrupt at this point and the volume is not immediately corrupted by this change. The result is ugly though and we have anecdotal evidence of a system here at OSR failing to boot after multiple attemps to chkdsk, so we though we'd mitigate the problem while we wait for the real fix to arrive.

This filter blocks any attempts to open a stream that begins with ":$i30:". This blocks more than just the intended path (e.g. ":$i30:$index_allocation") but we believe 
the impact of this to be minimal.

# Downloads #

We have signed binaries available for immediate install on x86 and x64 platforms.

[Download the latest i30Flt release](https://github.com/OSRDrivers/i30Flt/releases/latest)

# Installation #
Open an elevated command prompt and execute the following commands:

	RUNDLL32.EXE SETUPAPI.DLL,InstallHinfSection DefaultInstall 132 .\i30flt.inf

	wevtutil im i30flt.man

	fltmc load i30flt

To uninstall the filter execute the following:

	RUNDLL32.EXE SETUPAPI.DLL,InstallHinfSection DefaultUninstall 132 .\i30flt.inf

# Building #
The provided solution builds using the 2004 WDK. 

