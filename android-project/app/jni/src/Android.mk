LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
SDL_IMAGE_PATH := ../SDL2_image-2.8.6/

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include \
                    $(LOCAL_PATH)/$(SDL_IMAGE_PATH)/include \
                    $(LOCAL_PATH)/../../../../core/include

LOCAL_CPPFLAGS += -fsigned-char -O3 -flto -fomit-frame-pointer -fno-exceptions -fno-rtti -DNDEBUG

# Add your application source files here...
EMU_ROOT := $(LOCAL_PATH)/../../../..
LOCAL_SRC_FILES := $(wildcard $(EMU_ROOT)/core/lib/*.cpp) \
                   $(wildcard $(EMU_ROOT)/core/lib/cpu/*.cpp) \
                   $(wildcard $(EMU_ROOT)/core/lib/cartridge/*.cpp) \
                   $(wildcard $(EMU_ROOT)/platform/android/*.cpp)

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -lOpenSLES -llog -landroid

include $(BUILD_SHARED_LIBRARY)
