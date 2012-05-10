LOCAL_PATH := $(my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := utilsTest
LOCAL_CFLAGS := $(commonCflags)
LOCAL_C_INCLUDES := $(commonIncludes)
LOCAL_C_INCLUDES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr/include
LOCAL_C_INCLUDES += hardware/qcom/display/liboverlay/badger/src
LOCAL_C_INCLUDES += hardware/qcom/display/libgralloc
LOCAL_SRC_FILES := utilsTest.cpp
LOCAL_MODULE_TAGS := optional eng
LOCAL_SHARED_LIBRARIES := liboverlay
LOCAL_SHARED_LIBRARIES += libcutils
LOCAL_MODULE_PATH := $(TARGET_OUT_DATA)/utilsTest

LOCAL_ADDITIONAL_DEPENDENCIES += $(TARGET_OUT_INTERMEDIATES)/KERNEL_OBJ/usr
ifeq ($(TARGET_USES_OVERLAY),true)
LOCAL_CFLAGS += -DUSE_OVERLAY
endif

include $(BUILD_EXECUTABLE)
