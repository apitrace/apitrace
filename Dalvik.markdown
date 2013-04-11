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
    adb shell setprop wrap.$PROCNAME LD_PRELOAD=/data/egltrace.so
    adb shell setprop debug.apitrace.procname $PROCNAME

(the former is read by Dalvik and specifies wrapping prefix, the latter is
read by apitrace itself and used in case apitrace is preloaded into Java VM
globally to specify which process should be traced).

Make sure the process is not loaded before starting to trace it, for example
use `-S` flag to `am start`:

    adb shell am start -S $PROCNAME

Use `adb logcat \*:S apitrace` to examine apitrace debug output.  Trace files
are saved into '/data/' directory by default:

    adb pull /data/$PROCNAME.trace
    adb shell rm /data/$PROCNAME.trace

(you need to `chmod 0777 /data` beforehand).


Injecting tracing library globally
----------------------------------

If `LD_PRELOAD` is supported (Android 2.3 "Gingerbread" and newer), it is
possible to inject `egltrace.so` into the resident Java VM, in which case
`debug.apitrace.procname` system propery is used to control for which process
tracing will be active.

Restarting 'zygote' (Java VM) service is not straightforward, since '/init.rc'
is read only once at system bootup, and restored from the recovery image on
reboots.   Thus, you either need to augment '/init.rc' in the recovery image
with `setenv LD_PRELOAD /data/egltrace.so` in `service zygote` section, or you
can use a tool such as
[adjust-child-env](https://github.com/amonakov/adjust-child-env) to restart
the service with modified environment.

Put `adjust-child-env` and a script with the following contents into `/data`:

    stop zygote
    /data/adjust-child-env 1 /system/bin/app_process LD_PRELOAD=/data/egltrace.so &
    sleep 1
    start zygote

The scripts restarts the Java VM ('zygote') with modified environment.

Invoke the script with `adb shell` to prepare for tracing, and then follow the
Android 4.0 directions.
