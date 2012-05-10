LOCAL_PATH := $(call my-dir)

# HAL module implemenation, not prelinked and stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SHARED_LIBRARIES := liblog libcutils libEGL libhardware libutils liboverlay
LOCAL_SHARED_LIBRARIES += libgenlock libQcomUI libmemalloc
LOCAL_SRC_FILES :=  hwcomposer.cpp
LOCAL_MODULE := hwcomposer.$(TARGET_BOARD_PLATFORM)
LOCAL_CFLAGS:= -DLOG_TAG=\"$(TARGET_BOARD_PLATFORM).hwcomposer\" -DDEBUG_CALC_FPS
LOCAL_C_INCLUDES += $(TARGET_OUT_HEADERS)/qcom/display
LOCAL_C_INCLUDES += hardware/qcom/display/liboverlay/badger/src
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include

#XXX Temporary include for compiling
LOCAL_C_INCLUDES += hardware/qcom/display/liboverlay/a-family

LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)
