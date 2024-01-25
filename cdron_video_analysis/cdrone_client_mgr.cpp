/***********************************************************************************************
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


#include "cdrone_client_mgr.h"

#include "cvideo_analysis_mgr.h"
namespace chen {

	cdrone_client_mgr g_drone_client_mgr;
	bool cdrone_client_mgr::init()
	{
		m_check_time = 0;
		return true;
	}
	void cdrone_client_mgr::udpate(uint32 uDateTime)
	{
		m_check_time += uDateTime;
		if (m_check_time < (180 * 1000))
		{
			return;
		}
		m_check_time = 0;
		std::vector<std::string> offline_clients;
		for (/*std::pair<const std::string, cvideo_analysis_mgr>*/auto & pi : m_drone_client_map)
		{
			if (pi.second->check_drone_offline())
			{
				WARNING_EX_LOG("drone client offline ->> [uuid = %s] ", pi.first.c_str());
				offline_clients.push_back(pi.first);
				pi.second->destroy();
				delete pi.second;
			}
			//pi.second.destroy();
		}

		for(const std::string & offline: offline_clients)
		{
			if (!m_drone_client_map.erase(offline))
			{
				WARNING_EX_LOG("delete drone [client = %s] failed !!!", offline.c_str());
			}
		}
	}
	void cdrone_client_mgr::destroy()
	{
		for (/*std::pair<const std::string, cvideo_analysis_mgr> */auto & pi    : m_drone_client_map)
		{
			NORMAL_EX_LOG("destroy drone [client = %s] ...", pi.first.c_str());
			pi.second->destroy();
			delete pi.second;
		}
		m_drone_client_map.clear();
		cv::destroyAllWindows();
	}
	int32 cdrone_client_mgr::http_drone_client_action(const cdrone_action_info& action)
	{
		cvideo_analysis_mgr* video_analysis_mgr_ptr = _get_video_analysis_mgr(action.client);
		if (!video_analysis_mgr_ptr)
		{
			return 510;
		}
		cvideo_analysis* video_analysis_ptr = video_analysis_mgr_ptr->get_video_analysis(action.source);
		if (video_analysis_ptr)
		{

			if (!action.action)
			{
				if (!video_analysis_ptr->get_startup())
				{
					if (!video_analysis_ptr->startup(action.source))
					{
						WARNING_EX_LOG("startup [drone client = %s]video analysis failed [source = %s]", action.client.c_str(),  action.source.c_str());
						return 509;
					}
				}
				if (action.video_skip_frame)
				{
					video_analysis_ptr->set_skip_frame(action.video_skip_frame);
				}
				if (!action.result_video_analysis.empty())
				{
					video_analysis_ptr->set_result_video_analysis(action.result_video_analysis);
				}
			}
			else
			{
				//video_analysis_ptr->stop();
				video_analysis_mgr_ptr->del_video_analysis(action.source);
			}
		}
		else
		{
			return 505;
		}
		return 200;
	}
	cvideo_analysis_mgr* cdrone_client_mgr::_get_video_analysis_mgr(const std::string& client)
	{

		DRONE_CLIENT_MAP::iterator iter =  m_drone_client_map.find(client);
		if (iter != m_drone_client_map.end())
		{
			return  iter->second;
		}
		WARNING_EX_LOG("not find drone [clinet = %s]", client.c_str());

		cvideo_analysis_mgr* video_analysis_mgr = new cvideo_analysis_mgr();
		video_analysis_mgr->init();
		//std::pair<DRONE_CLIENT_MAP::iterator, bool> pi = m_drone_client_map.insert(std::make_pair(client, video_analysis_mgr));
		if (!m_drone_client_map.insert(std::make_pair(client, video_analysis_mgr)).second)
		{
			WARNING_EX_LOG("insert all  drone client  map [client = %s] failed !!!", client.c_str());
			delete video_analysis_mgr;
			video_analysis_mgr = NULL;
			return NULL;
		}
		  
		return video_analysis_mgr;
	 
	}
	int32    cdrone_client_mgr::mqtt_drone_client_action(const cdrone_action_info& action)
	{
		cvideo_analysis_mgr* video_analysis_mgr_ptr = _get_video_analysis_mgr(action.client);
		if (!video_analysis_mgr_ptr)
		{
			return 510;
		}

		cvideo_analysis* video_analysis_ptr = video_analysis_mgr_ptr->get_video_analysis(action.source);
		if (video_analysis_ptr)
		{

			if (!action.action)
			{
				if (!video_analysis_ptr->get_startup())
				{
					if (!video_analysis_ptr->startup(action.source))
					{
						WARNING_EX_LOG("startup video analysis [client = %s] failed  [source = %s]", action.client.c_str(), action.source.c_str());
					}
				}
				if (action.parse_video_skip_frame)
				{
					video_analysis_ptr->set_skip_frame(action.video_skip_frame);
				}
				if (action.parse_car_analysis)
				{
					video_analysis_ptr->set_car_analysis(action.car_analysis);
				}
				if (!action.result_video_analysis.empty())
				{
					video_analysis_ptr->set_result_video_analysis(action.result_video_analysis);
				}
			}
			else
			{
				//video_analysis_ptr->stop();
				video_analysis_mgr_ptr->del_video_analysis(action.source);
			}
		}
		else
		{
			WARNING_EX_LOG("[drone client = %s]not find [source = %s]", action.client.c_str(), action.source.c_str());
		}

		return 0;
	}

	// //std::vector< cvideo_analysis_info> video_analysis_infos;
		 // g_video_analysis_mgr.build_video_analysis_infos(video_analysis_infos);

	std::vector< cvideo_analysis_info>         cdrone_client_mgr::http_drone_client_video_analysis_infos()
	{
		std::vector< cvideo_analysis_info> video_analysis_infos;

		for (/*std::pair<const std::string, cvideo_analysis_mgr>*/auto & pi : m_drone_client_map)
		{
			std::vector< cvideo_analysis_info> temp;
			pi.second->build_video_analysis_infos(temp);
			for (  cvideo_analysis_info& v : temp)
			{
				v.client = pi.first;
				video_analysis_infos.push_back(v);
			}
			//pi.second.destroy();
		}
		return video_analysis_infos;
	}
	void	 cdrone_client_mgr::mqtt_drone_client_heart_beat(const std::string& client)
	{
		cvideo_analysis_mgr* video_analysis_mgr_ptr = _get_video_analysis_mgr( client);
		if (!video_analysis_mgr_ptr)
		{
			WARNING_EX_LOG("[drone client = %s]heart_beat", client.c_str());
			return;
		}
		NORMAL_EX_LOG("[drone client = %s]heart_beat", client.c_str());
		video_analysis_mgr_ptr->update_drone_client_heart_beat();
	}
}