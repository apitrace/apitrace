#
# This file helps integrate apitrace into FirefoxOS - when apitrace
# sources are put in B2GROOT/external/apitrace (including this Android.mk
# file), then the B2G build system will pick apitrace automatically and
# compile and install it into the system images seamlessly.
#
# This may work in other than FirefoxOS environments, but has not been tested.
#

NDK := prebuilt/ndk/android-ndk-r7

ifeq ($(shell which cmake),)
$(shell echo "CMake not found, will not compile apitrace" >&2)
else # cmake
ifeq ($(wildcard $(NDK)),)
$(shell echo "CMake present but NDK not found at $(abspath $(NDK)), will not compile apitrace" >&2)
else # NDK
$(shell echo "CMake and NDK ($(abspath $(NDK))) found, will compile apitrace" >&2)

ifeq ($(TARGET_ARCH),arm)
TOOLCHAIN := arm-linux-androideabi-4.4.x
endif

ifeq ($(TARGET_ARCH),x86)
TOOLCHAIN := i686-android-linux-4.4.3
endif

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := egltrace
LOCAL_MODULE_TAGS := debug eng

include $(BUILD_SHARED_LIBRARY)

# Below we hook the process of configuring and compiling apitrace,
# described in INSTALL.markdown (but we use the FirefoxOS's NDK). We override
# the $(linked_module): targed, which is already defined by
# $(BUILD_SHARED_LIBRARY) - by default it would want to compile the
# library out of some source files.
# We also override the target $(LOCAL_INSTALLED_MODULE): which installs
# the shared library because we want it installed in
# /lib/apitrace/wrappers/egltrace.so instead of /lib/egltrace.so because
# /bin/apitrace searches for the library in that directory.
# The rules will end up with /lib/apitrace/wrappers/egltrace.so and
# /bin/apitrace inside system.img.
MY_APITRACE_ROOT := $(TOPDIR)external/apitrace
MY_APITRACE_BUILD_ROOT_HOST := out/host/apitrace
MY_APITRACE_BUILD_ROOT_TARGET := out/target/apitrace

MY_ANDROID_CMAKE_COMMIT := 556cc14296c226f753a3778d99d8b60778b7df4f

android.toolchain.cmake:
	curl -s -O https://raw.githubusercontent.com/taka-no-me/android-cmake/$(MY_ANDROID_CMAKE_COMMIT)/android.toolchain.cmake

apitrace_private_target: android.toolchain.cmake
	$(hide) # apitrace: run cmake for the host if it has not been run
	$(hide) if [ ! -e $(MY_APITRACE_BUILD_ROOT_HOST)/Makefile ] ; then \
		cd $(MY_APITRACE_ROOT) && \
		cmake -H. -B../../$(MY_APITRACE_BUILD_ROOT_HOST) ; \
	fi
	$(hide) # apitrace: compile for the host
	$(hide) make -C $(MY_APITRACE_BUILD_ROOT_HOST)
	$(hide) # apitrace: run cmake for android if it has not been run
	$(hide) if [ ! -e $(MY_APITRACE_BUILD_ROOT_TARGET)/Makefile ] ; then \
		cd $(MY_APITRACE_ROOT) && \
		cmake \
		-DCMAKE_TOOLCHAIN_FILE=android.toolchain.cmake \
		-DANDROID_NDK=../../$(NDK) \
		-DANDROID_NDK_LAYOUT=LINARO \
		-DANDROID_TOOLCHAIN_NAME=$(TOOLCHAIN) \
		-DANDROID_API_LEVEL=9 \
		-DANDROID_NO_UNDEFINED=OFF \
		-DLIBRARY_OUTPUT_PATH_ROOT=../../$(MY_APITRACE_BUILD_ROOT_TARGET) \
		-H. -B../../$(MY_APITRACE_BUILD_ROOT_TARGET) ; \
	fi
	$(hide) # apitrace: compile for android
	$(hide) make -C $(MY_APITRACE_BUILD_ROOT_TARGET)

$(linked_module): apitrace_private_target
	$(hide) # apitrace: copy egltrace lib to where the build system expects it
	$(hide) mkdir -p $(dir $@)
	$(hide) cp $(MY_APITRACE_BUILD_ROOT_TARGET)/libs/*/egltrace$(TARGET_SHLIB_SUFFIX) $@

$(LOCAL_INSTALLED_MODULE): $(LOCAL_BUILT_MODULE) | $(ACP)
	@echo "Install (overridden): $@"
	@mkdir -p $(dir $@)/apitrace/wrappers
	$(hide) $(ACP) -fp $< $(dir $@)/apitrace/wrappers/egltrace$(TARGET_SHLIB_SUFFIX)

#

include $(CLEAR_VARS)

LOCAL_MODULE := apitrace
LOCAL_MODULE_TAGS := debug eng

include $(BUILD_EXECUTABLE)

$(linked_module): apitrace_private_target
	$(hide) # apitrace: copy apitrace executable to where the build system expects it
	$(hide) mkdir -p $(dir $@)
	$(hide) cp $(MY_APITRACE_BUILD_ROOT_TARGET)/apitrace$(TARGET_EXECUTABLE_SUFFIX) $@

endif # NDK
endif # cmake
