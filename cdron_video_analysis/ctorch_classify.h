/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef C_TORCH_CLASSIFY_H
#define C_TORCH_CLASSIFY_H
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
#include <torch/torch.h>
#include <torch/script.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <ctime>

 
namespace chen
{

	struct cls_socre
	{
		int   index;
		float   socre;
		cls_socre()
			: index(0)
			, socre(0.0f) {}
	};

	class ctorch_classify
	{
	public:
		ctorch_classify(std::string model_path);
		/*: m_device(torch::kCUDA)
		, m_model(){}*/
		virtual ~ctorch_classify();

	public:
		//bool init(std::string pt);

		cls_socre   classitfy(cv::Mat img);


		cv::Mat pilResize(cv::Mat &img, int size);
		cv::Mat pilCropCenter(cv::Mat &img, int output_size);
		cv::Mat setNorm(cv::Mat &img);
		cv::Mat setMean(cv::Mat &image_resized_float);
		//void update();
		void destroy();
	private:

		torch::Device		m_device;// (device_type);
		torch::jit::script::Module m_model;
	};



	void test_classification();
	int test__main();
}
#endif //C_YOLOV_H