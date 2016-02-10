# Tracing Java applications on Android #

Android's Java virtual machine, runs as a system service (started at
bootup by `init`) and Java applications are run by forks of the initial
resident process.  Thus, injecting apitrace's tracing library is different from
other operating systems.

The following discussion assumes that tracing library is copied to `/data`:

    adb push /path/to/apitrace/libs/[arch]/egltrace.so /data


## Tracing on Android 4.0 and newer ##

Starting from Android 4.0 (Ice Cream Sandwich) release, the system supports
running designated processes with wrappers, in which case a new Java VM is
started with 'system()' library call for that process.

Obtain the process name of the application to be traced (the one reported in
`ps` output, such as `com.android.settings`), and set two system properties:

    PROCNAME=com.android.settings
    adb root
    adb shell ln -s /data/data/apitrace.github.io.eglretrace/libgnustl.so /data/libgnustl.so
    adb shell setprop wrap.$PROCNAME LD_PRELOAD="/data/libgnustl_shared.so:/data/egltrace.so"
    adb shell setprop debug.apitrace.procname $PROCNAME

(the former is read by Dalvik and specifies wrapping prefix, the latter is
read by apitrace itself and used in case apitrace is preloaded into Java VM
globally to specify which process should be traced).  Elevating privileges
via `adb root` is required to set the first property.

If the `wrap.$PROCNAME` property name is longer than 31 characters [you'll need
to truncate it](https://github.com/apitrace/apitrace/issues/296).

Make sure the process is not loaded before starting to trace it, for example
use `-S` flag to `am start`:

    adb shell am start -S $PROCNAME

Use `adb logcat \*:S apitrace` to examine apitrace debug output.  Trace files
are saved into `/data/data/$PROCNAME` directory by default:

    adb pull /data/data/$PROCNAME/$PROCNAME.trace
    adb shell rm /data/data/$PROCNAME/$PROCNAME.trace
