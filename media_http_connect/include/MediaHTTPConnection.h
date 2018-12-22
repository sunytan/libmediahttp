/*
 * MediaHTTPConnection.h
 *
 *  Created on: 2018年10月26日
 *      Author: yang-tan
 */

#ifndef MEDIAHTTPCONNECTION_H_
#define MEDIAHTTPCONNECTION_H_

#include "BnMediaHTTPConnection.h"
#include <binder/Parcel.h>
#include <fstream>
#include <iostream>
#include <binder/MemoryDealer.h>
#include <binder/IMemory.h>
#include <curl/curl.h>
#include <media/stagefright/MediaErrors.h>
#include <utils/RefBase.h>
#include <utils/String16.h>
#include <media/MemoryLeakTrackUtil.h>

//打开这个宏进行本地文件模拟测试
//#define localTest;

namespace android {

enum {
	kBufferSize = 8192, //curl 一次writedata最大数据16k,网速测试下来一次数据基本维持在8192附近
};

struct LocalCache {
	char* buffer;
	size_t size;
};

class MediaHTTPConnection: public BnMediaHTTPConnection {

public:
	sp<IBinder> connect_impl(const char *uri, const char* header);
	void disconnect();
	int32_t readAt(off64_t offset, size_t size);
	off64_t getSize();
	String16 getMIMEType();
	String16 getUri();
	MediaHTTPConnection();
	virtual ~MediaHTTPConnection();

private:
	int32_t seekTo(off64_t offset, size_t size);
	void getSize_internal();
	void getMimeType_internal();
	char* bufferBytes;

private:
#ifdef localTest
	ifstream in;				//进行localTest的时候的文件流
#endif
	CURL *curl;
	sp<MemoryDealer> mDealer;
	sp<IMemory> mMemory; 		//给player 提供datasource的memory
	off64_t totalSize; 			//content-length
	String16 mime_type; 		// content-type
	char* uri;					// media 链接地址
	char* header;				// media http 头信息
};

}
#endif /* MEDIAHTTPCONNECTION_H_ */
