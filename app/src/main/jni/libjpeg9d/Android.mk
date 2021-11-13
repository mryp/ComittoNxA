# libjpeg 9d : https://ijg.org/files/jpegsr9d.zip
# 参考:https://www.usefullcode.net/2011/01/android_ndklibjpeg.html

LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

# From autoconf-generated Makefile
libjpeg_SOURCES_DIST =  jaricom.c jcapimin.c jcapistd.c jcarith.c jccoefct.c jccolor.c \
                                jcdctmgr.c jchuff.c jcinit.c jcmainct.c jcmarker.c jcmaster.c \
                                jcomapi.c jcparam.c jcprepct.c jcsample.c jctrans.c jdapimin.c \
                                jdapistd.c jdarith.c jdatadst.c jdatasrc.c jdcoefct.c jdcolor.c \
                                jddctmgr.c jdhuff.c jdinput.c jdmainct.c jdmarker.c jdmaster.c \
                                jdmerge.c jdpostct.c jdsample.c jdtrans.c jerror.c jfdctflt.c \
                                jfdctfst.c jfdctint.c jidctflt.c jidctfst.c jidctint.c jquant1.c \
                                jquant2.c jutils.c jmemmgr.c \
                                jmemname.c

LOCAL_SRC_FILES:= $(libjpeg_SOURCES_DIST)

ifeq ($(APP_DEBUG), 0)
    LOCAL_CFLAGS += -O3
endif

ifeq ($(TARGET_ARCH_ABI),armeabi-v7a)
    LOCAL_CFLAGS += -mfpu=neon -mfloat-abi=softfp -fPIC -march=armv7-a
    LOCAL_ARM_MODE :=arm
    LOCAL_ARM_NEON :=true
else ifeq ($(TARGET_ARCH_ABI),armeabi)
    LOCAL_CFLAGS += -fPIC
    LOCAL_ARM_MODE :=arm
    LOCAL_ARM_NEON :=false
else ifeq ($(TARGET_ARCH_ABI),x86)
    LOCAL_CFLAGS += -mfpmath=sse -msse2 -m32 -masm=intel
endif

LOCAL_MODULE := libjpeg9d

include $(BUILD_STATIC_LIBRARY)
