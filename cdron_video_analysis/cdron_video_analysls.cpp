﻿/***********************************************************************************************
created: 		2019-05-06

author:			chensong

purpose:		gateway


输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
*********************************************************************/

#include "cdron_video_analysls.h"
#include "clog.h"
#include "ccfg.h"
#include "ctime_api.h"
#include "cmqtt_mgr.h"
#include "ctime_elapse.h"
#include "clog.h"
#include "cvideo_analysis.h"
#include "cvideo_logic.h"
#include "cmqtt_mgr.h"
#include "cvideo_analysis_mgr.h"
#include "cweb_http_api_mgr.h"
#include "chttp_queue_mgr.h"
namespace chen {
	cdron_video_analysls g_cdron_video_analysls;


	cdron_video_analysls::cdron_video_analysls()
		: m_stop(false)
	{
	}

	cdron_video_analysls::~cdron_video_analysls()
	{
	}

	bool cdron_video_analysls::init(const char * log_path, const char * config_file)
	{
		printf("Log init ...\n");
		if (!CLOG::init(log_path, "dron_video_analysls"))
		{
			return false;
		}
		if (!g_cfg.init(config_file))
		{
			return false;
		}
		CLOG::set_level(static_cast<ELogLevelType>(g_cfg.get_uint32(ECI_LogLevel)));

		ctime_base_api::set_time_zone(g_cfg.get_int32(ECI_TimeZone));
		ctime_base_api::set_time_adjust(g_cfg.get_int32(ECI_TimeAdjust));


		SYSTEM_LOG("Web Http Server API init ...");
		if (!g_web_http_api_mgr.init())
		{
			return false;
		}
		SYSTEM_LOG("Web Http Server API startup ...");
		g_web_http_api_mgr.startup();
		SYSTEM_LOG("Web Http Server API startup OK ...");

		// MQtt init
		SYSTEM_LOG("Mqtt init");
		if (!s_mqtt_client_mgr.init())
		{
			return false;
		}

		if (!s_mqtt_client_mgr.startup())
		{
			return false;
		}
		SYSTEM_LOG("video logic init ");
		if (!g_video_logic.init())
		{
			return false;
		}
		SYSTEM_LOG("drone client video analysis mgr init ");
		if (!g_drone_client_mgr.init())
		{
			return false;
		}

		SYSTEM_LOG(" dron video analysls server init ok !!!");
		return true;
	}

	bool cdron_video_analysls::Loop()
	{
		static const uint32 TICK_TIME = 100;
		 
		ctime_elapse time_elapse;
		uint32 uDelta = 0;
		//cvideo_analysis analysis;
		//analysis.startup("rtsp://admin:admin12345@192.168.2.213");
		while (!m_stop)
		{
			uDelta += time_elapse.get_elapse();
	 
			g_video_logic.update(uDelta);
			g_http_queue_mgr.update(uDelta);
			g_drone_client_mgr.udpate(uDelta);
			uDelta = time_elapse.get_elapse();

			if (uDelta < TICK_TIME)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME - uDelta)); 
			}
		}

		SYSTEM_LOG("Leave main loop");
		return true;
	}

	void cdron_video_analysls::destroy()
	{
		SYSTEM_LOG("video analysls  Destroy OK!");
		g_video_logic.destroy();
		g_drone_client_mgr.destroy();

		s_mqtt_client_mgr.stop();
		s_mqtt_client_mgr.destroy();
		SYSTEM_LOG("mqtt destroy OK !!!");
		g_web_http_api_mgr.destroy();
		SYSTEM_LOG("Web Server Http  destroy ok !!!");
		g_cfg.destroy();
		SYSTEM_LOG("cfg destroy OK !!!");
		CLOG::destroy();;
	}

	void cdron_video_analysls::stop()
	{
		//s_mqtt_client_mgr.stop();
		m_stop = true;
	}

}