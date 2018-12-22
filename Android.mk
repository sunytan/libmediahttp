LOCAL_PATH:= $(call my-dir)
#build mediahttp library
include $(CLEAR_VARS)


LOCAL_SRC_FILES := \
	media_http_service/MediaHTTPService.cpp \
	media_http_connect/MediaHTTPConnection.cpp
	
LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/include \
    $(LOCAL_PATH)/media_http_service/include \
    $(LOCAL_PATH)/media_http_connect/include \
    external/curl/include
    
LOCAL_SHARED_LIBRARIES := \
	libcutils \
    libutils \
    liblog \
    libbinder \
    libmedia \
    libstagefright \
    libmemunreachable \
    libmediautils \
    libcurl
    
LOCAL_MODULE_TAGS := optional 
    
LOCAL_MODULE := libmediahttp

include $(BUILD_SHARED_LIBRARY)
