/*
 * BnMediaHTTPService.h
 *
 *  Created on: 2018年10月25日
 *      Author: yang-tan
 */

#ifndef BNMEDIAHTTPSERVICE_H_
#define BNMEDIAHTTPSERVICE_H_

#include <media/IMediaHTTPService.h>
#include <binder/IBinder.h>
#include <binder/Parcel.h>
#include <binder/IInterface.h>
#include <utils/Log.h>

#define LOG_TAG "native_MediaHTTPService"

namespace android{

class BnMediaHTTPService:public BnInterface<IMediaHTTPService>{

public:
	status_t onTransact( uint32_t code,
	                                 const Parcel& data,
	                                 Parcel* reply,
	                                 uint32_t flags = 0){
		switch(code){
			case 1:{
				CHECK_INTERFACE(IMediaHTTPService,data,reply);
				sp<IMediaHTTPConnection> mediaHttpConnect =  makeHTTPConnection();
				if (mediaHttpConnect != NULL) {
				    reply->writeNoException();
				    reply->writeStrongBinder(IInterface::asBinder(mediaHttpConnect));
				}else {
					int32_t result = -1;
					reply->writeInt32(result);
					reply->writeStrongBinder(NULL);
				}
				return NO_ERROR;
			}
			default:{
				return BBinder::onTransact(code, data, reply, flags);
			}
		}
	}
};

}

#endif /* BNMEDIAHTTPSERVICE_H_ */
