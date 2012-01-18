#Enables the listed display HAL modules
ifeq ($(call is-vendor-board-platform,QCOM),true)
display-hals := libhwcomposer liboverlay libgralloc libgenlock libcopybit libtilerenderer
display-hals += libqcomui
include $(call all-named-subdir-makefiles,$(display-hals))
endif
