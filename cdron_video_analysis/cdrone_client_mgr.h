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
#ifndef _C_DRONE_CLIENT_MGR_H_
#define _C_DRONE_CLIENT_MGR_H_
#include "cnoncopyable.h"
#include "cnet_type.h"
#include <unordered_map>
#include "cvideo_analysis.h"
//#include "cvideo_analysis_mgr.h"
#include <map>
namespace chen {

	class cvideo_analysis_mgr;
	struct cdrone_action_info
	{
		std::string	client;
		std::string source;
		uint32       action;
		uint32		 video_skip_frame  ;
		bool         parse_video_skip_frame  ;
		uint32		 car_analysis  ;
		bool		 parse_car_analysis  ;
		std::string	 result_video_analysis;
		cdrone_action_info()
			: client("")
			, source("")
			, action(0)
			, video_skip_frame(3)
			, parse_video_skip_frame(false)
			, car_analysis(0)
			, parse_car_analysis(false)
			, result_video_analysis(""){}
	};
	class cdrone_client_mgr : public cnoncopyable
	{ 
	private:
		typedef  std::unordered_map<std::string, cvideo_analysis_mgr*>   DRONE_CLIENT_MAP;

	public:
		explicit cdrone_client_mgr()
			: m_drone_client_map()
			, m_check_time(0){}
		virtual ~cdrone_client_mgr() {}


	public:
		bool init();

		void udpate(uint32 uDateTime);

		void destroy();

	public:

		// http
		int32    http_drone_client_action(const cdrone_action_info& action);

		std::vector< cvideo_analysis_info>         http_drone_client_video_analysis_infos();
	public:
		// MQTT update info
		int32    mqtt_drone_client_action(const cdrone_action_info& action);
		void	 mqtt_drone_client_heart_beat(const std::string & client);
	public:
	private:
		cvideo_analysis_mgr* _get_video_analysis_mgr(const std::string & client);
		//const cvideo_analysis_mgr* _get_video_analysis_mgr(const std::string& client) const;
	private:


		DRONE_CLIENT_MAP				m_drone_client_map;
		uint64							m_check_time;
	};


	extern cdrone_client_mgr g_drone_client_mgr;

}

#endif // 