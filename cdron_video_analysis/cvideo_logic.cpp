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
					std::string source;
					uint32       action;
					uint32		 video_skip_frame = 0;
					bool         parse_video_skip_frame = false;
					uint32		 car_analysis = 0;
					bool		 parse_car_analysis = false;
					bool ret = true;
					try
					{
						PARSE_VALUE(source, String, false, parse_video_skip_frame);
						PARSE_VALUE(action, UInt, false, parse_video_skip_frame);
						PARSE_VALUE(video_skip_frame, UInt, true, parse_video_skip_frame);
						PARSE_VALUE(car_analysis, UInt, true, parse_car_analysis);

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

							if (!action)
							{
								if (!video_analysis_ptr->get_startup())
								{
									if (!video_analysis_ptr->startup(source))
									{
										WARNING_EX_LOG("startup video analysis failed [source = %s]", source.c_str());
									}
								}
								if (parse_video_skip_frame)
								{
									video_analysis_ptr->set_skip_frame(video_skip_frame);
								}
								if (parse_car_analysis)
								{
									video_analysis_ptr->set_car_analysis(car_analysis);
								}
							}
							else
							{
								//video_analysis_ptr->stop();
								g_video_analysis_mgr.del_video_analysis(source);
							}
						}
						else
						{
							WARNING_EX_LOG("not find [source = %s]", source.c_str());
						}
					}
					else
					{
						WARNING_EX_LOG("parse value [topic = %s][payload = %s] failed !!!", msg.m_topic.c_str(), msg.m_payload.c_str());

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