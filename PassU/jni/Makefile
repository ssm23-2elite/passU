JAVA_CLASSES_DIR = bin/classes
JAVA_NATIVES_DIR = $(JAVA_CLASSES_DIR)/org/ssm232elite/passu/android/natives
JNI_HEADER_DIR = jni/include
SOURCES = InputHandler.cpp

all: header lib

header : ../$(JAVA_NATIVES_DIR)/InputHandler.class
	cd ../$(JAVA_CLASSES_DIR); \
	javah org.ssm232elite.passu.android.natives.InputHandler; \
	mv org_ssm232elite_passu_android_natives_InputHandler.h InputHandler.h; \
	mv InputHandler.h ../../jni/include/InputHandler.h
	
lib : $(SOURCES)
	ndk-build APP_ABI="armeabi x86"
	
clean :
	rm -rf ../libs/armeabi/libpassu.so; \
	rm -rf ../libs/armeabi/libfbuffer.so; \
	rm -rf ../libs/x86/libpassu.so; \
	rm -rf ../libs/x86/libfbuffer.so; \
	rm -rf ../obj