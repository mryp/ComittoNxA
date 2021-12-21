
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS := -fstrict-aliasing -fprefetch-loop-arrays

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

LOCAL_MODULE    := libunrar613

LOCAL_SRC_FILES := unpack.cpp rdwrfn.cpp \
                    crypt.cpp timefn.cpp strfn.cpp unicode.cpp \
                    secpassword.cpp rijndael.cpp sha256.cpp \
                    sha1.cpp crc.cpp hash.cpp blake2s.cpp \
                    getbits.cpp rarvm.cpp


include $(BUILD_STATIC_LIBRARY)
