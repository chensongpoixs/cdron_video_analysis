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
#include "cvideo_analysis_platform.h"
#include "cyolov_onnxruntime.h"
#include "ctorch_classify.h"
#include "clicense_plate.h"
namespace chen
{
	class cvideo_analysis
	{
	public:
		explicit cvideo_analysis()
		: m_video_analysis_type(EVideoAnalysisTorchScript)
		, m_source_path("")
		, m_stoped(true)
		, m_detector_ptr(NULL)
		, class_names()
		, m_video_cap_ptr(NULL)
		, m_video_index(-1)
		, m_skip_frame(0)
		, m_car_analysis(0)
		, m_license_plate()
		, m_result_video_analysis("")
		, m_car_color_ptr(NULL)
		, m_car_type_ptr(NULL)
		{}
		virtual ~cvideo_analysis() {}

	public:
		bool init();
		bool startup(const std::string & source);
		void stop();
		void destroy();
	public:

		bool get_startup() const { return !m_stoped; }



		void set_skip_frame(uint32 count);
		void set_car_analysis(uint32 analysis);

	private:

		void _work_pthread();


		void _send_video_info(cv::Mat& img,
			const std::vector<std::vector<CDetection>>& detections,
			const std::vector<std::string>& class_names,
			bool label = true);
		std::string _recognize_vehicle_color(const cv::Mat& img);
	protected:
	private:
		EVideoAnalysisPlatformType	m_video_analysis_type;
		std::string					m_source_path;
		bool						m_stoped;
		//std::string				m_weights_file;
		Detector				*	 m_detector_ptr;
		cyolov_onnxruntime		*	m_onnxruntime_ptr;
		std::vector<std::string>	class_names;
		cv::VideoCapture		*	m_video_cap_ptr;
		int32						m_video_index;
		std::thread					m_thread;
		uint32						m_skip_frame;
		uint32						m_car_analysis;
		clicense_plate				m_license_plate;
		std::string					m_result_video_analysis;
		ctorch_classify*			m_car_color_ptr;
		ctorch_classify*			m_car_type_ptr;
	};

}

#endif // C_VIDEO_ANALYSIS_H