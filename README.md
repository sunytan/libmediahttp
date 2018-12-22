# libmediahttp
Android native下进行mediaplayer播放在线url的支持库

播放代码demo:

----------------start---------------------

#include <media/mediaplayer.h>
#include <media/IMediaPlayer.h>
#include <utils/Log.h>
#include <binder/IPCThreadState.h>
#include <media/IMediaPlayerService.h>
#include <media/IMediaHTTPService.h>
#include "media/mediahttp/MediaHTTPServiceInterface.h"
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <curl/curl.h>
#include <utils/RefBase.h>
#include <utildeInit_global_curl();s/String16.h


using namespace android;


int main(int argc,char* argv[]){
	if(!init_global_curl()){
		return -1;
	}
  const char* url = "http://www.xylsl.cn/images/ibl.mp3";
  sp<MediaPlayer> mp = new MediaPlayer();
  sp<IMediaHTTPService> httpService = createMediaHTTPService();
  mp.reset();
  status_t ret = 0;
  ret = mp->setDataSource(httpService,url,NULL);
  
  if(ret != OK){
			ALOGD("setDataSource failed");
			mp->reset();
			return -1;
		}
    
    mp->prepare();
		mp->start();
    
    while(mp->isPlaying()){
			   sleep(1);
		}
		mp->stop();
    
    mp->setListener(0);
		mp->disconnect();
    
    mp.clear();
    mp = nullptr;
    deInit_global_curl();
}


---------end-----------------
