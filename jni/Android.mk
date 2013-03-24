LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := com.ekatas.fmtheremin
LOCAL_SRC_FILES := com.ekatas.fmtheremin.cpp

LOCAL_LDLIBS    := -llog -landroid -lEGL -lGLESv1_CM
# for native audio
LOCAL_LDLIBS    += -lOpenSLES
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)