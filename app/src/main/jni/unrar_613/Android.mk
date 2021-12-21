
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
                    blake2s.cpp crc.cpp crypt.cpp getbits.cpp hash.cpp \
                    rarvm.cpp rijndael.cpp secpassword.cpp sha1.cpp sha256.cpp strfn.cpp timefn.cpp unicode.cpp
#                    blake2sp.cpp coder.cpp crypt1.cpp crypt2.cpp crypt3.cpp crypt5.cpp model.cpp suballoc.cpp \
#                    unpack15.cpp unpack20.cpp unpack30.cpp unpack50.cpp unpack50frag.cpp unpackinline.cpp

include $(BUILD_STATIC_LIBRARY)
