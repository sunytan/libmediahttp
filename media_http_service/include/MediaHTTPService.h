/*
 * MediaHttpService.h
 *
 *  Created on: 2018年10月25日
 *      Author: yang-tan
 */

#ifndef MEDIAHTTPSERVICE_H_
#define MEDIAHTTPSERVICE_H_

#include <media/IMediaHTTPConnection.h>
#include <media/IMediaHTTPService.h>
#include "media/mediahttp/MediaHTTPServiceInterface.h"
#include "BnMediaHTTPService.h"

namespace android{

class MediaHTTPService:public BnMediaHTTPService{

public:
	virtual sp<IMediaHTTPConnection> makeHTTPConnection();
	MediaHTTPService();
	virtual ~MediaHTTPService();
};

}
#endif /* MEDIAHTTPSERVICE_H_ */
