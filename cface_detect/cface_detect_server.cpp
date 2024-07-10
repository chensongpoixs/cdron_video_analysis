/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/


#include "cface_detect_server.h"
#include "cwebsocket_wan_server.h"
#include "clog.h"
#include "ctime_api.h"
#include "cclient_msg_dispatch.h"
#include "ctime_elapse.h"
namespace chen {
	cface_detect_server g_face_detect_server;
	cface_detect_server::cface_detect_server(): m_stoped(true)
	{
	}
	cface_detect_server::~cface_detect_server()
	{
	}
	bool cface_detect_server::init(const char* log_path, const char* config_file)
	{
		printf("LOG init ...");
		if (!CLOG::init(log_path, "decoder_server"))
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
		SYSTEM_LOG("dispatch init ...");

		if (!g_client_msg_dispatch.init())
		{
			return false;
		}
		SYSTEM_LOG("client_msg dispatch init OK !!!");


		SYSTEM_LOG("websocket wan server  init ...");
		if (!g_websocket_wan_server.init())
		{
			return false;
		}
		SYSTEM_LOG("websocket wan server  startup ...");
		if (!g_websocket_wan_server.startup())
		{
			return false;
		}
		SYSTEM_LOG(" cface detect  server init ok");
		return true;
	}
	void cface_detect_server::Loop()
	{
		//return false;
		static const uint32 TICK_TIME = 100;
		//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È´ï¿½ï¿½ï¿½Ê¼ï¿½ï¿½ï¿½ï¿½ï¿½
		NORMAL_EX_LOG("");
		ctime_elapse time_elapse;
		uint32 uDelta = 0;
		while (!m_stoped)
		{
			uDelta += time_elapse.get_elapse();
			//	NORMAL_EX_LOG("");
	//			g_game_client.update(uDelta);
			//g_cmd_server.update(uDelta);
			g_websocket_wan_server.update(uDelta);
			//g_cmd_parser.update(uDelta);


			//g_global_logic_mgr.update(uDelta);
		//	g_http_queue_mgr.update();
			uDelta = time_elapse.get_elapse();

			if (uDelta < TICK_TIME)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(TICK_TIME - uDelta));
			}
		}

		SYSTEM_LOG("Leave main loop");
	}
	void cface_detect_server::destroy()
	{
		g_websocket_wan_server.shutdown();
		g_websocket_wan_server.destroy();
		SYSTEM_LOG("g_wan_server Destroy OK!!!");





		SYSTEM_LOG(" cfg  destroy OK !!!");
		g_cfg.destroy();

		g_client_msg_dispatch.destroy();
		SYSTEM_LOG("msg dispath destroy OK !!!");

		//1 log
		CLOG::destroy();
		printf(" cface detect Server  Destroy OK end !\n");
	}
	void cface_detect_server::stop()
	{
		m_stoped = true;
	}
}