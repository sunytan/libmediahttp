/*
 * BnMediaHTTPConnection.h
 *
 *  Created on: 2018年10月26日
 *      Author: yang-tan
 */

#ifndef BNMEDIAHTTPCONNECTION_H_
#define BNMEDIAHTTPCONNECTION_H_

#include <media/IMediaHTTPConnection.h>
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/Parcel.h>
#include <utils/Log.h>

#define LOG_TAG "native_MediaHTTPConnection"

namespace android {

enum {
	CONNECT = IBinder::FIRST_CALL_TRANSACTION,
	DISCONNECT,
	READ_AT,
	GET_SIZE,
	GET_MIME_TYPE,
	GET_URI
};

class BnMediaHTTPConnection: public BnInterface<IMediaHTTPConnection> {

public:
	bool connect(const char *uri,
			const KeyedVector<String8, String8> *headers) {
		return true;
	}
	;
	ssize_t readAt(off64_t offset, void *data, size_t size) {
		return 0;
	}
	;
	status_t getMIMEType(String8 *mimeType) {
		return NO_ERROR;
	}
	;
	status_t getUri(String8 *uri) {
		return NO_ERROR;
	}
	;

	virtual sp<IBinder> connect_impl(const char *uri, const char *header)=0;
	virtual void disconnect()=0;
	virtual int32_t readAt(off64_t offset, size_t size)=0;
	virtual off64_t getSize()=0;
	virtual String16 getMIMEType()=0;
	virtual String16 getUri()=0;

	status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply,
			uint32_t flags = 0) {
		switch (code) {
		case CONNECT: {
			ALOGD("CONNECT");
			CHECK_INTERFACE(IMediaHTTPConnection, data, reply);
			String8 uri = String8(data.readString16());
			String8 header = String8(data.readString16());
			ALOGD("uri = %s, header = %s", uri.string(), header.string());
			reply->writeNoException();
			reply->writeStrongBinder(
					(connect_impl(uri.string(), header.string())));
			return NO_ERROR;
		}
			break;
		case DISCONNECT: {
			ALOGD("DISCONNECT");
			CHECK_INTERFACE(IMediaHTTPConnection, data, reply);
			disconnect();
			return NO_ERROR;
		}
			break;
		case READ_AT: {
			ALOGD("READ_AT");
			CHECK_INTERFACE(IMediaHTTPConnection, data, reply);
			off64_t offset = data.readInt64();
			size_t size = data.readInt32();
			int32_t len = readAt(offset, size);
			reply->writeNoException();
			reply->writeInt32(len);
			return NO_ERROR;
		}
			break;
		case GET_SIZE: {
			ALOGD("GET_SIZE");
			CHECK_INTERFACE(IMediaHTTPConnection, data, reply);
			off64_t size = getSize();
			reply->writeNoException();
			reply->writeInt64(size);
			return NO_ERROR;
		}
			break;
		case GET_MIME_TYPE: {
			ALOGD("GET_MIME_TYPE");
			CHECK_INTERFACE(IMediaHTTPConnection, data, reply);
			String16 type = getMIMEType();
			reply->writeNoException();
			reply->writeString16(type);
			return NO_ERROR;
		}
			break;
		case GET_URI: {
			ALOGD("GET_URI");
			CHECK_INTERFACE(IMediaHTTPConnection, data, reply);
			String16 uri = getUri();
			reply->writeNoException();
			reply->writeString16(uri);
			return NO_ERROR;
		}
			break;
		default: {
			return BBinder::onTransact(code, data, reply, flags);
		}
		}
	}
};

}
#endif /* BNMEDIAHTTPCONNECTION_H_ */
