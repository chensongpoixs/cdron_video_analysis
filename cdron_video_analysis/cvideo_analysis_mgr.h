/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef C_VIDEO_ANALYSIS_MGR_H
#define C_VIDEO_ANALYSIS_MGR_H
#include <thread>
#include <string>
#include <functional>
#if defined(_MSC_VER)
#include <Windows.h>
#endif

#include "opencv2/opencv.hpp"
#include <memory>

#include <torch/script.h>
#include <torch/torch.h>

#include <c10/cuda/CUDAStream.h>
#include <ATen/cuda/CUDAEvent.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudacodec.hpp>
#include "utils.h"
#include <hv/mqtt_client.h>
#include "csingleton.h"
#include "cweb_http_api_proxy.h"
#include "cvideo_analysis.h"
#include <unordered_map>
namespace chen
{
	class cvideo_analysis_mgr
	{
	private:
		typedef std::unordered_map<std::string, cvideo_analysis*>         VIDEO_ANALYSIS_MAP;
	public:
		explicit cvideo_analysis_mgr()
		 : m_all_video_map(){}
		virtual ~cvideo_analysis_mgr() {}

	public:
		bool init();
		void destroy();

	public:
		cvideo_analysis*  get_video_analysis(const std::string& source);
		bool             del_video_analysis(const std::string& source);

		void			build_video_analysis_infos(std::vector< cvideo_analysis_info>& video_infos);
	protected:
	private:
		VIDEO_ANALYSIS_MAP								m_all_video_map;
	};

	extern cvideo_analysis_mgr g_video_analysis_mgr;
}

#endif // C_VIDEO_ANALYSIS_H