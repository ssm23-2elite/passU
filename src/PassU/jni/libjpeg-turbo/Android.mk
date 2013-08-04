LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := fbuffer
LOCAL_SRC_FILES := fbuffer.c	\
					jcapimin.c	\
					jcapistd.c	\
					jcinit.c	\
					jdatadst.c	\
					jerror.c	\
					jcparam.c	\
					jcdctmgr.c	\
					jcphuff.c	\
					jsimd_none.c	\
					jcmaster.c	\
					jccolor.c	\
					jcsample.c	\
					jcprepct.c	\
					jcmarker.c	\
					jcarith.c	\
					jccoefct.c	\
					jchuff.c	\
					jcmainct.c	\
					jcomapi.c	\
					jctrans.c	\
					jaricom.c	\
					turbojpeg.c	\
					jdatadst-tj.c	\

LOCAL_LDLIBS := -llog -ljpeg
LOCAL_CFLAGS := -DCONFIG_EMBEDDED\ -DUSE_IND_THREAD\

include $(BUILD_SHARED_LIBRARY)