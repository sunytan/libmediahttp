/*
 * MediaHTTPService.cpp
 *
 *  Created on: 2018年10月25日
 *      Author: yang-tan
 */
#include "include/MediaHTTPService.h"
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/Parcel.h>
#include "MediaHTTPConnection.h"

namespace android{

sp<IMediaHTTPService> createMediaHTTPService(){
	return interface_cast<IMediaHTTPService>(new MediaHTTPService());
}

bool init_global_curl(){
	ALOGI("init_global_curl");
	CURLcode code  = curl_global_init(CURL_GLOBAL_ALL);
	if(code != CURLE_OK){
		ALOGE("curl global init fail err = %s",curl_easy_strerror(code));
		return false;
	}
	return true;
}

void deInit_global_curl(){
	ALOGI("deInit_global_curl");
	curl_global_cleanup();
}

MediaHTTPService::MediaHTTPService() {
	ALOGI("MediaHTTPService constructor");
}

MediaHTTPService::~MediaHTTPService() {
	ALOGI("MediaHTTPService destructor");
}

sp<IMediaHTTPConnection> MediaHTTPService::makeHTTPConnection(){
	ALOGD("makeHTTPConnection");
	return interface_cast<IMediaHTTPConnection>(new MediaHTTPConnection());
}

}
