Tracing Dalvik VM (Java) applications on Android
================================================

Android's Java virtual machine, Dalvik, runs as a system service (started at
bootup by `init`) and Java applications are run by forks of the initial
resident process.  Thus, injecting apitrace's tracing library is different from
other operating systems.

The following discussion assumes that tracing library is copied to '/data':

    adb push /path/to/apitrace/build/wrappers/egltrace.so /data


Tracing on Android 4.0 and newer
--------------------------------

Starting from Android 4.0 (Ice Cream Sandwich) release, Dalvik supports
running designated processes with wrappers, in which case a new Java VM is
started with 'system()' library call for that process.

Obtain the process name of the application to be traced (the one reported in
`ps` output, such as `com.android.settings`), and set two system properties:

    PROCNAME=com.android.settings
    adb root
    adb shell setprop wrap.$PROCNAME LD_PRELOAD=/data/egltrace.so
    adb shell setprop debug.apitrace.procname $PROCNAME

(the former is read by Dalvik and specifies wrapping prefix, the latter is
read by apitrace itself and used in case apitrace is preloaded into Java VM
globally to specify which process should be traced).  Elevating priviliges
via `adb root` is required to set the first property.

Make sure the process is not loaded before starting to trace it, for example
use `-S` flag to `am start`:

    adb shell am start -S $PROCNAME

Use `adb logcat \*:S apitrace` to examine apitrace debug output.  Trace files
are saved into '/data/data/$PROCNAME' directory by default:

    adb pull /data/data/$PROCNAME/$PROCNAME.trace
    adb shell rm /data/data/$PROCNAME/$PROCNAME.trace


Tracing on Android pre-4.0
--------------------------

`LD_PRELOAD` is supported since Android 2.3 "Gingerbread" and newer, but
injecting tracing library globally is no longer supported, as the
`debug.apitrace.procname` system propery is no longer honored.

Consider checking out an
[older commit](https://github.com/apitrace/apitrace/commit/888112983ef9564b3a9d15699faa17c337d3942b)
if you need to trace on Android pre-4.0.
