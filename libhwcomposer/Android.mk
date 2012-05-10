ifeq ($(call is-board-platform-in-list,copper),true)
    hwc-hal := badger
else
    hwc-hal := a-family
endif
include $(call all-named-subdir-makefiles,$(hwc-hal))
