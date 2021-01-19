# i30Flt #

@jonasLyk reported a REALLY interesting corruption error reported by NTFS:

https://twitter.com/jonasLyk/status/1347900440000811010

Triggering the notification only requires that you visit a particular path on an NTFS volume.

Our research indicates that the “file corrupt” error bubbles up from a network query open, so it’s sufficient to just call GetFileAttributes to see the behavior. 
We think the bug is in all the changes around case sensitivity...There’s a memory compare of “$i30” with “$I30” before the descent into chaos. Also if you use “$I30” 
in the offending command you don’t get the problem.

The directory is not really corrupt at this point and the volume is not immediately corrupted by this change. The result is ugly though, so we though we'd mitigate the
problem while we wait for the real fix to arrive.

This filter blocks any attempts to open a stream that begins with ":$i30:". This blocks more than just the intended path (e.g. ":$i30:$index_allocation") but we believe 
the mpact of this to be minimal.

# Building the sample #
The provided solution builds using the 2004 WDK. 

# Installing the sample #
You can install the filter with the following command line:

	RUNDLL32.EXE SETUPAPI.DLL,InstallHinfSection DefaultInstall 132 i30flt.inf

You'll also need to import the manifest in order to decode the events sent to the event log:

	wevtuil im i30flt.man

The filter will automatically load and do its thing on subsequent reboots. To uninstall the filter execute the following:


	RUNDLL32.EXE SETUPAPI.DLL,InstallHinfSection DefaultUninstall 132 i30flt.inf
