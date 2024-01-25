/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#include "cvideo_logic.h"
#include "cmqtt_mgr.h"
#include "clog.h"
#include <json/json.h>
#include <json/value.h>
#include "ccfg.h"
#include "cvideo_analysis.h"
#include "cvideo_analysis_mgr.h"
#include "cdrone_client_mgr.h"
namespace chen {

#define PARSE_VALUE(VAR, TYPE, RESULT, PARSE_VALUE_TYPE) 							\
	if (data.isMember(#VAR) && data[#VAR].is##TYPE())			\
	{															\
		VAR = data[#VAR].as##TYPE();							\
		if (RESULT) \
		{			\
			PARSE_VALUE_TYPE = true;		\
		}			\
	}															\
    else														\
	{															\
		if (ret)												\
		{														\
			ret = RESULT;										\
		}														\
		WARNING_EX_LOG("not find or type  [var = %s]  ", #VAR); \
	}

 


	cvideo_logic g_video_logic;

	bool cvideo_logic::init()
	{
		return true;
	}
	void cvideo_logic::update(uint32 uDelta)
	{
		std::list<cmqtt_msg> msgs;
		s_mqtt_client_mgr.process(msgs);
		while (!msgs.empty())
		{
			const cmqtt_msg & msg = msgs.front();
			{
				// pase json 
				Json::Reader reader;
				Json::Value data;

				if (!reader.parse((const char*)msg.m_payload.c_str(), (const char*)msg.m_payload.c_str() + msg.m_payload.size(), data))
				{
					WARNING_EX_LOG("parse [topic = %s][payload = %s] failed !!!", msg.m_topic.c_str(), msg.m_payload.c_str());
				}
				else
				{
					std::string	client;
					std::string source;
					uint32       action;
					uint32		 video_skip_frame = 0;

					uint32		 car_analysis = 0;
					bool		 parse_car_analysis = false;
					std::string	 result_video_analysis;
					bool ret = true;
					bool         parse_video_skip_frame = false;
					std::string topic = std::string(msg.m_topic, 0,  g_cfg.get_string(ECI_MqttDroneClientOnlineTopic).length());
					//NORMAL_EX_LOG("topic = %s", topic.c_str());
					if (topic == g_cfg.get_string(ECI_MqttDroneClientOnlineTopic))
					{
						try
						{
							PARSE_VALUE(client, String, false, parse_video_skip_frame);
						}
						catch (const std::exception&)
						{
							ret = false;
							WARNING_EX_LOG("parse value [topic = %s][payload = %s] failed !!!", msg.m_topic.c_str(), msg.m_payload.c_str());
						}
						if (ret)
						{
							g_drone_client_mgr.mqtt_drone_client_heart_beat(client);

						}
						else
						{
							WARNING_EX_LOG("parse value [topic = %s][payload = %s] failed !!!", msg.m_topic.c_str(), msg.m_payload.c_str());

						}
					}
					else
					{
						
						try
						{
							PARSE_VALUE(client, String, false, parse_video_skip_frame);
							PARSE_VALUE(source, String, false, parse_video_skip_frame);
							PARSE_VALUE(action, UInt, false, parse_video_skip_frame);
							PARSE_VALUE(video_skip_frame, UInt, true, parse_video_skip_frame);
							PARSE_VALUE(car_analysis, UInt, true, parse_car_analysis);
							PARSE_VALUE(result_video_analysis, String, false, parse_video_skip_frame);

						}
						catch (const std::exception&)
						{
							ret = false;
							WARNING_EX_LOG("parse value [topic = %s][payload = %s] failed !!!", msg.m_topic.c_str(), msg.m_payload.c_str());
						}
						if (ret)
						{
							cdrone_action_info action_info;
							action_info.client = client;
							action_info.source = source;
							if (video_skip_frame < 5)
							{
								video_skip_frame = 5;
							}
							action_info.video_skip_frame = video_skip_frame;
							action_info.car_analysis = car_analysis;
							action_info.result_video_analysis = result_video_analysis;
							g_drone_client_mgr.mqtt_drone_client_action(action_info);

						}
						else
						{
							WARNING_EX_LOG("parse value [topic = %s][payload = %s] failed !!!", msg.m_topic.c_str(), msg.m_payload.c_str());

						}
					}
					
				}
			}
			msgs.pop_front();
		}
	}
	void cvideo_logic::destroy()
	{
	}
}