/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#include "cvideo_analysis_mgr.h"
#include "clog.h"
namespace chen {
	cvideo_analysis_mgr g_video_analysis_mgr;
	bool cvideo_analysis_mgr::init()
	{
		return true;
	}

	void cvideo_analysis_mgr::destroy()
	{
		for (VIDEO_ANALYSIS_MAP::iterator iter = m_all_video_map.begin(); iter != m_all_video_map.end(); ++iter)
		{
			NORMAL_EX_LOG("[source = %s] destroy .... ", iter->first.c_str());
			iter->second->destroy();
			NORMAL_EX_LOG("[source = %s] destroy OK !!! ", iter->first.c_str());
			delete iter->second;
			NORMAL_EX_LOG("[source = %s] free object OK !!! ", iter->first.c_str());
		}
		m_all_video_map.clear();
		cv::destroyAllWindows();
	}

	cvideo_analysis * cvideo_analysis_mgr::get_video_analysis(const std::string & source)
	{
		VIDEO_ANALYSIS_MAP::iterator iter =  m_all_video_map.find(source);
		if (iter != m_all_video_map.end())
		{
			return iter->second;
		}
		NORMAL_EX_LOG("all video not find [source = %s]", source.c_str());

		cvideo_analysis * video_analysis_ptr = new cvideo_analysis();
		if (!video_analysis_ptr)
		{
			WARNING_EX_LOG("alloc [source = %s] failed !!!", source.c_str());
			return NULL;
		}
		video_analysis_ptr->init();
		if (!m_all_video_map.insert(std::make_pair(source, video_analysis_ptr)).second)
		{
			WARNING_EX_LOG("insert all video analysis map [source = %s] failed !!!", source.c_str());
			delete video_analysis_ptr;
			video_analysis_ptr = NULL;
			return NULL;
		}
		return video_analysis_ptr;
	}

	bool cvideo_analysis_mgr::del_video_analysis(const std::string & source)
	{
		VIDEO_ANALYSIS_MAP::iterator iter = m_all_video_map.find(source);
		if (iter == m_all_video_map.end())
		{
			WARNING_EX_LOG("del video [source = %s] failed !!!", source.c_str());
			return false;
		}
		iter->second->stop();
		iter->second->destroy();
		delete iter->second;
		m_all_video_map.erase(iter);
		
		NORMAL_EX_LOG("del video analysis [source = %s] OK !!!", source.c_str());
		return true;
	}

}