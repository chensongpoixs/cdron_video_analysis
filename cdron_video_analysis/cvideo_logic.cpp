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
#include "cvideo_analysis.h"
#include "cvideo_analysis_mgr.h"
namespace chen {

#define PARSE_VALUE(VAR, TYPE) \
	if (data.isMember(#VAR) && data[#VAR].is##TYPE())\
	{\
		VAR = data[#VAR].as##TYPE();\
	} \
    else \
	{ \
		WARNING_EX_LOG(" [var = %s]  ", #VAR); \
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
					std::string source;
					bool ret = true;
					try
					{
						PARSE_VALUE(source, String);
					}
					catch (const std::exception&)
					{
						ret = false;
						WARNING_EX_LOG("parse value [topic = %s][payload = %s] failed !!!", msg.m_topic.c_str(), msg.m_payload.c_str());
					}
					if (ret)
					{
						cvideo_analysis* video_analysis_ptr =  g_video_analysis_mgr.get_video_analysis(source);
						if (video_analysis_ptr)
						{
							video_analysis_ptr->startup(source);
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