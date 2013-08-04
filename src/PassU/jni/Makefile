JAVA_CLASSES_DIR = bin/classes
JAVA_NATIVES_DIR = $(JAVA_CLASSES_DIR)/org/secmem/remoteroid/natives
JAVA_NATIVES_U_DIR = $(JAVA_CLASSES_DIR)/org/secmem/remoteroid/universal/natives
JNI_HEADER_DIR = jni/include
SOURCES = InputHandler.cpp FrameHandler.cpp

all: header lib

header : ../$(JAVA_NATIVES_DIR)/InputHandler.class ../$(JAVA_NATIVES_DIR)/FrameHandler.class ../$(JAVA_NATIVES_U_DIR)/FrameHandlerU.class
	cd ../$(JAVA_CLASSES_DIR); \
	javah org.secmem.remoteroid.natives.InputHandler; \
	javah org.secmem.remoteroid.natives.FrameHandler; \
	javah org.secmem.remoteroid.universal.natives.FrameHandlerU; \
	mv org_secmem_remoteroid_natives_InputHandler.h InputHandler.h; \
	mv org_secmem_remoteroid_natives_FrameHandler.h FrameHandler.h; \
	mv org_secmem_remoteroid_universal_natives_FrameHandlerU.h FrameHandlerU.h; \
	mv InputHandler.h ../../jni/include/InputHandler.h; \
	mv FrameHandler.h ../../jni/include/FrameHandler.h; \
	mv FrameHandlerU.h ../../jni/include/FrameHandlerU.h
	
lib : $(SOURCES)
	ndk-build APP_ABI="armeabi x86"
	
clean :
	rm -rf ../libs/armeabi/libremoteroid.so; \
	rm -rf ../libs/armeabi/libfbuffer.so; \
	rm -rf ../libs/x86/libremoteroid.so; \
	rm -rf ../libs/x86/libfbuffer.so; \
	rm -rf ../obj