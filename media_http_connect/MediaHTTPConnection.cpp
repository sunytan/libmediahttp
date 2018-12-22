/*
 * MediaHTTPConnection.cpp
 *
 *  Created on: 2018年10月26日
 *      Author: yang-tan
 */

#include "MediaHTTPConnection.h"
#include "BnMediaHTTPConnection.h"
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/Parcel.h>
#include <binder/MemoryDealer.h>
#include <binder/IInterface.h>
#include <memunreachable/memunreachable.h>

using namespace std;
namespace android {

//int fd;
const char* file = "/storage/self/debug.txt";

//本地中间缓存
LocalCache cache;


MediaHTTPConnection::MediaHTTPConnection() {

/*	String8 result;
	//
	fd = open(file,O_RDWR | O_APPEND);
	result.append("\n dump memory :\n");
	string mem = dumpMemoryAddresses(100);
	result.append(mem.c_str(),mem.size());
	result.append("\n dump unreachable :\n");
	string unreachable = GetUnreachableMemoryString(true,10000);
	result.append(unreachable.c_str(),unreachable.size());
	write(fd,result.string(),result.size());
	close(fd);*/
	//
	ALOGD("MediaHTTPConnection()");
	curl = NULL;
	mime_type = String16("");
	totalSize = 0;
	uri = NULL;
	header = NULL;
	mDealer = new MemoryDealer(kBufferSize, "MediaHTTPConnection1");
	mMemory = mDealer->allocate(kBufferSize);
	if (mMemory == NULL || mMemory == 0 || mMemory -> pointer() == NULL
			|| mMemory.get() == nullptr || mMemory.get() == 0) {
		ALOGE("MemoryDealer allocate mMemory failed is NULL");
	}
	bufferBytes = static_cast<char*>(malloc(kBufferSize));
	cache.buffer = static_cast<char*>(malloc(kBufferSize));
	cache.size = 0;
	//fp = fopen(file, "w");
}

MediaHTTPConnection::~MediaHTTPConnection() {
	ALOGD("~MediaHTTPConnection()");

	//delete mime_type; 错误的调用 +++ ALLOCATION 0xb3cbfd68 HAS INVALID TAG 10 (free)
	ALOGD("~MediaHTTPConnection()11");
	if (this->header != NULL) {
		free(this->header);
		this->header = NULL;
	}
	if (this->uri != NULL) {
		free(this->uri);
		this->uri = NULL;
	}
	this->mDealer.clear();
	this->mDealer = nullptr;
	if (this->bufferBytes != NULL) {
		free(this->bufferBytes);
		this->bufferBytes = NULL;
	}
	if (cache.buffer != NULL) {
		free(cache.buffer);
		cache.buffer = NULL;
		cache.size = 0;
	}
	if (this->curl != NULL) {
		curl_easy_cleanup(this->curl);
	}
	this->curl == NULL;
#ifdef localTest
	if(in.is_open()) {
		in.close();
	}
#else

#endif
}

sp<IBinder> MediaHTTPConnection::connect_impl(const char* uri,
		const char* header) {
	disconnect();
	this->uri = static_cast<char*>( malloc(strlen(uri) * sizeof(char)));
	this->header = static_cast<char*>(malloc(strlen(header) * sizeof(char)));
	strcpy(this->uri, uri);
	strcpy(this->header, header);
	ALOGD("connect http uri = %s, header = %s", this->uri, this->header);
#ifdef localTest
	in.open(file,ios::in|ios::binary|ios::ate);
	totalSize = in.tellg();
#else

#endif
	return IInterface::asBinder(mMemory);
}

void MediaHTTPConnection::disconnect() {
#ifdef localTest
	if(in.is_open()) {
		in.close();
	}
#else
	if (this->curl != NULL) {
		curl_easy_cleanup(this->curl);
		this->curl = NULL;
	}

	if (this->header != NULL) {
		free(this->header);
		this->header = NULL;
	}

	if (this->uri != NULL) {
		free(this->uri);
		this->uri = NULL;
	}
	memset(cache.buffer, '\0', kBufferSize);
	cache.size = 0;
#endif
}

int32_t MediaHTTPConnection::readAt(off64_t offset, size_t size) {
	int32_t n = 0;
	// 每次请求的size大小不能大于缓冲区大小
	if (size > kBufferSize) {
		size = kBufferSize;
	}
#ifdef localTest
	if (in.is_open()) {
		in.seekg(offset,ios::beg);
		in.read((char*)data,size);
		n = in.gcount();
	} else {
		n = -1;
	}
#else
	n = seekTo(offset, size);
#endif
	//fwrite(bufferBytes,1,n,fp);
	ALOGV("readAT size %zu >> n %zu , offset = %lld",size, n, offset);
	if (n < 0) {
		return n;
	}

	size_t len = n;

	//按照谷歌已知bug,对此进行判断,避免出现内存溢出
	//https://android.googlesource.com/platform/frameworks/av/+/51504928746edff6c94a1c498cf99c0a83bedaed%5E%21/#F0
	if (len > size) {
		ALOGE("requested %zu, got %zu", size, len);
		return ERROR_OUT_OF_RANGE;
	}
	ALOGV("mMemory addr= %p",&(this->mMemory));
	ALOGV("mMemory addr= %p",this->mMemory.get());

	if (this->mMemory == 0 || this->mMemory == NULL ||
			this->mMemory.get() == nullptr || this->mMemory.get() == 0) {
			ALOGE("mMemory is NULL");
			return ERROR_OUT_OF_RANGE;
	}

	size_t memory_size = this->mMemory->size();
	if (len > memory_size) {
		ALOGE("got %zu, but memory has %zu", len, memory_size);
		return ERROR_OUT_OF_RANGE;
	}

	if (this->bufferBytes == NULL) {
		ALOGE("readAt got a NULL buffer");
		return UNKNOWN_ERROR;
	}

	if (mMemory->pointer() == NULL || mMemory->pointer() == 0){
		ALOGE("readAt got a NULL mMemory->pointer()");
		return UNKNOWN_ERROR;
	}

	memcpy(this->mMemory->pointer(), this->bufferBytes, len);
	memset(this->bufferBytes, '\0', kBufferSize);
	cache.size = 0;
	return len;
}

static size_t writeCallBack(void *contents, size_t size, size_t nmemb,
		void *userp) {
	size_t realsize = size * nmemb;
	ALOGV("size = %zu, nmemb = %zu, realsize = %zu",size,nmemb,realsize);
	memcpy(&(cache.buffer[cache.size]), contents, realsize);
	cache.size += realsize;
	ALOGV("writeCallBack cache.size = %zu", cache.size);
	return realsize;
}

int32_t MediaHTTPConnection::seekTo(off64_t offset, size_t size) {
	int32_t result = 0;

	if (this->curl == NULL) {
		ALOGV("seekTo curl init start");
		this->curl = curl_easy_init();
		ALOGV("seekTo curl init end");
	}

	//重置几个buffer内存
	memset(this->bufferBytes, '\0', kBufferSize);
	memset(cache.buffer, '\0', kBufferSize);
	cache.size = 0;

	if (this->curl) {
		// 重置curl 句柄恢复到默认init时的状态
		// 但是会保存 live connections, the Session ID cache, the DNS cache, the cookies and shares.
		ALOGD("curl easy reset start");
		//curl_easy_reset(curl);
		ALOGD("curl easy reset end");

		// 下面是正常play的情况
		CURLcode res;
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, header);
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_URL, uri);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30);
		curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallBack);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, NULL);

		//限定每次请求的数据大小为8192, 并且请求的数据大小不大于media自己内部需要请求的数据
		ALOGV("seekTo offset = %lld, size = %zu",offset,size);
		if (offset >= 0 && offset + size - 1 <= this->totalSize) {
			string range = to_string(offset) + "-" + to_string(offset + size - 1);
			ALOGV("range = %s", range.c_str());
			curl_easy_setopt(this->curl, CURLOPT_RANGE, range.c_str());
		}
		// offset不能超过文件总大小长度
		else if (offset < this->totalSize) {
			string range = to_string(offset) + "-" + to_string(this->totalSize - 1);
			ALOGD("range = %s", range.c_str());
			curl_easy_setopt(curl, CURLOPT_RANGE, range.c_str());
		}
		// 当不满足上述的时候说明已经到文件末尾了
		else {
			ALOGD("offset is at the end of file");
			curl_slist_free_all(headers);
			return 0;
		}

		// 开始curl请求
		ALOGD("perform");
		res = curl_easy_perform(this->curl);
		curl_slist_free_all(headers);
		if (res != CURLE_OK) {
			ALOGE("curl_easy_perform error res = %s", curl_easy_strerror(res));
			if (res == CURLE_UNSUPPORTED_PROTOCOL
					|| res == CURLE_REMOTE_ACCESS_DENIED) {
				totalSize = -1;
				return ERROR_UNSUPPORTED;
			}
		}

		// 成功请求完数据之后进行数据的操作
		if (size >= cache.size && cache.size > 0) {
			memcpy(&(this->bufferBytes[0]), cache.buffer, cache.size);
		} else {
			cache.size = 0;
		}
		result = cache.size;
	} else {
		ALOGE("curl init failed ");
		curl_easy_cleanup(this->curl);
		this->curl = NULL;
		result = -1;
	}
	return result;
}

void MediaHTTPConnection::getSize_internal() {
	CURL* size_curl = curl_easy_init();
	curl_easy_setopt(size_curl, CURLOPT_URL, this->uri);
	curl_easy_setopt(size_curl, CURLOPT_CUSTOMREQUEST, "GET");    //自定义GET请求
	curl_easy_setopt(size_curl, CURLOPT_NOBODY, 1);    //不需要body
	CURLcode res;
	res = curl_easy_perform(size_curl);
	if (res == CURLE_OK) {
		double temp_size;
		res = curl_easy_getinfo(size_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD,
				&temp_size);
		if (res == CURLE_OK) {
			this->totalSize = temp_size;
		} else {
			this->totalSize = -1;
			ALOGE("curl_easy_getinfo getSize_internal error res = %s",
					curl_easy_strerror(res));
		}
		ALOGD("getSize_internal totalSize = %lld", this->totalSize);
	} else {
		ALOGE("curl_easy_perform getSize_internal error res = %s",
				curl_easy_strerror(res));
		this->totalSize = -1;
	}
	curl_easy_cleanup(size_curl);
	size_curl = NULL;
}

void MediaHTTPConnection::getMimeType_internal() {
	CURL* type_curl = curl_easy_init();
	curl_easy_setopt(type_curl, CURLOPT_URL, this->uri);
	curl_easy_setopt(type_curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(type_curl, CURLOPT_NOBODY, 1);
	CURLcode res;
	res = curl_easy_perform(type_curl);
	if (res == CURLE_OK) {
		// 获取content-type
		char* temp_type;
		res = curl_easy_getinfo(type_curl, CURLINFO_CONTENT_TYPE, &temp_type);
		if (CURLE_OK == res) {
			this->mime_type = String16(temp_type);
		} else {
			ALOGE("curl_easy_getinfo getMimeType_internal error res = %s",
					curl_easy_strerror(res));
			this->mime_type = String16("application/octet-stream");
		}
		// 一并把content-length获取了
		double temp_size;
		res = curl_easy_getinfo(type_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD,
				&temp_size);
		if (CURLE_OK == res) {
			this->totalSize = temp_size;
		} else {
			ALOGE(
					"curl_easy_getinfo getMimeType_internal getSize error res = %s",
					curl_easy_strerror(res));
			this->totalSize = -1;
		}
		ALOGD("getMimeType_internal  totalSize = %lld", this->totalSize);
		ALOGD("getMimeType_internal mime_type = %s",
				String8(this->mime_type).string());
	} else {
		ALOGE("curl_easy_perform get type error res = %s",
				curl_easy_strerror(res));
		this->mime_type = String16("application/octet-stream");
		this->totalSize = -1;
	}
	curl_easy_cleanup(type_curl);
	type_curl = NULL;
}

off64_t MediaHTTPConnection::getSize() {
	getSize_internal();
	ALOGI("getSize totalSize =%lld", this->totalSize);
	return this->totalSize;
}

String16 MediaHTTPConnection::getMIMEType() {
	String16 type = String16("");
#ifdef localTest
	type = String16("audio/mp3");
#else
	getMimeType_internal();
	ALOGI("getMIMEType mime_type = %s", String8(this->mime_type).string());
	type = this->mime_type;
#endif
	return type;
}

String16 MediaHTTPConnection::getUri() {
	ALOGD("getUri = %s", this->uri);
	return String16(this->uri);
}

}
