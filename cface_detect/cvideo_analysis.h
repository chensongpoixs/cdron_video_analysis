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
//
//#include <torch/script.h>
//#include <torch/torch.h>
//
//#include <c10/cuda/CUDAStream.h>
//#include <ATen/cuda/CUDAEvent.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
//#include <opencv2/cudaimgproc.hpp>
//#include <opencv2/cudacodec.hpp>
//#include "utils.h"
//#include <hv/mqtt_client.h>
//#include "detector.h"
#include "csingleton.h"
#include "cnet_type.h"
//#include "cvideo_analysis_platform.h"
//#include "cyolov_onnxruntime.h"
//#include "ctorch_classify.h"
//#include "cyolov_torch.h"
//#include "clicense_plate.h"
namespace chen
{
	struct cvideo_analysis_info
	{
		std::string  client;
		std::string  source;
		uint32_t		 action;
		uint32_t		 video_skip_frame;
		uint32_t       car_analysis;
		std::string  result_video_analysis;
		cvideo_analysis_info()
			: client("")
			, source("")
			, action(1)
			, video_skip_frame(0)
			, car_analysis(0)
			, result_video_analysis("") {}

	};
	class cvideo_analysis
	{
	public:
		explicit cvideo_analysis()
		//: m_video_analysis_type(EVideoAnalysisTorchScript)
		: m_source_path("")
		, m_stoped(true)
		, m_action(0)
	//	, m_detector_ptr(NULL)
		//, m_yolov_ptr(NULL)
#if defined(_MSC_VER)
	//	, m_onnxruntime_ptr(NULL)
#endif 
		, class_names()
		, m_video_cap_ptr(NULL)
		, m_video_index(-1)
		, m_skip_frame(0)
		, m_car_analysis(0)
	//	, m_license_plate()
		, m_result_video_analysis("")
		//, m_car_color_ptr(NULL)
		//, m_car_type_ptr(NULL)
		, m_queue()
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
		void set_result_video_analysis(const std::string& result_video_analysis);
		uint32_t get_action() const { return m_action; }
		uint32_t get_skip_frame() const { return m_skip_frame; }
		uint32_t get_car_analysis() const { return m_car_analysis; }
		std::string get_result_video_analysis() const {
			return m_result_video_analysis
				;
		}
	private:

		void _work_analysis_pthread();


		void _work_decode_thread();

		//void _send_video_info(cv::Mat& img,
		//	const std::vector<std::vector<CDetection>>& detections,
		//	const std::vector<std::string>& class_names,
		//	bool label = true);
		std::string _recognize_vehicle_color(const cv::Mat& img);
		


		bool _open();
	protected:
	private:
		//EVideoAnalysisPlatformType	m_video_analysis_type;
		std::string					m_source_path;
		bool						m_stoped;
		int32                       m_action;
		//std::string				m_weights_file;
		
		//Detector				*	 m_detector_ptr;
		//cyolov_torch			* m_yolov_ptr;
#if defined(_MSC_VER)
		//cyolov_onnxruntime		*	m_onnxruntime_ptr;
#endif 
		std::vector<std::string>	class_names;
		cv::VideoCapture		*	m_video_cap_ptr;
		int32						m_video_index;
		std::thread					m_thread;
		uint32						m_skip_frame;
		uint32						m_car_analysis;
		//clicense_plate				m_license_plate;
		std::string					m_result_video_analysis;
		//ctorch_classify*			m_car_color_ptr;
		//ctorch_classify*			m_car_type_ptr;
		std::thread					m_decode_thread;
		std::list< cv::Mat>			m_queue;
		std::mutex					m_queue_lock;
	};

}

#endif // C_VIDEO_ANALYSIS_H
