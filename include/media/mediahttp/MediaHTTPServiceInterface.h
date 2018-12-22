/*
 * MediaHTTPServiceInterface.h
 *
 *  Created on: 2018年10月27日
 *      Author: yang-tan
 */

#ifndef MEDIAHTTPSERVICEINTERFACE_H_
#define MEDIAHTTPSERVICEINTERFACE_H_

#include <media/IMediaHTTPService.h>

namespace android{

	sp<IMediaHTTPService> createMediaHTTPService();
	bool init_global_curl();
	void deInit_global_curl();

}


#endif /* MEDIAHTTPSERVICEINTERFACE_H_ */
