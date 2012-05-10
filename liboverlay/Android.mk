ifeq ($(call is-board-platform-in-list,copper),true)
    overlay-hal := badger
else
    overlay-hal := a-family
endif
include $(call all-named-subdir-makefiles,$(overlay-hal))
