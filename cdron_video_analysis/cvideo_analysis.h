/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef C_VIDEO_ANALYSIS_H
#define C_VIDEO_ANALYSIS_H
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
#include "detector.h"
#include "csingleton.h"
#include "cnet_type.h"
namespace chen
{
	class cvideo_analysis
	{
	public:
		explicit cvideo_analysis() {}
		virtual ~cvideo_analysis() {}

	public:
		bool init();
		bool startup(const std::string & source);
		void stop();
		void destroy();
	private:

		void _work_pthread();


		void _send_video_info(cv::Mat& img,
			const std::vector<std::vector<Detection>>& detections,
			const std::vector<std::string>& class_names,
			bool label = true);
	protected:
	private:
		std::string    m_source_path;
		//std::string    m_weights_file;
		Detector    * m_detector_ptr;
		std::vector<std::string> class_names;
		cv::VideoCapture * m_video_cap_ptr;
		int32				m_video_index;
		std::thread			m_thread;
	};

}

#endif // C_VIDEO_ANALYSIS_H