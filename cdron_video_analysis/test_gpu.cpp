/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/


#include <iostream>

#include "opencv2/opencv_modules.hpp"

#if defined(HAVE_OPENCV_CUDACODEC)

#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/cudacodec.hpp>
#include <opencv2/highgui.hpp>

int test_gupmain(int argc, const char* argv[])
{
	if (argc != 2)
		return -1;

	const std::string fname(argv[1]);

	cv::namedWindow("CPU", cv::WINDOW_NORMAL);
	//cv::namedWindow("GPU", cv::WINDOW_OPENGL);
	cv::cuda::setGlDevice(0);

	//cv::Mat frame;
	//cv::VideoCapture reader(fname);
	std::vector<int> params{ cv::CAP_PROP_AUDIO_STREAM, -1,
							cv::CAP_PROP_VIDEO_STREAM, 1, cv::CAP_PROP_STREAM_OPEN_TIME_USEC };
	cv::cuda::GpuMat d_frame;
	cv::Ptr<cv::cudacodec::VideoReader> d_reader = cv::cudacodec::createVideoReader(fname);

	cv::TickMeter tm;
	std::vector<double> cpu_times;
	std::vector<double> gpu_times;

	int gpu_frame_count = 0, cpu_frame_count = 0;

	/*for (;;)
	{
		tm.reset(); tm.start();
		if (!reader.read(frame))
			break;
		tm.stop();
		cpu_times.push_back(tm.getTimeMilli());
		cpu_frame_count++;

		cv::imshow("CPU", frame);

		if (cv::waitKey(3) > 0)
			break;
	}*/

	for (;;)
	{
		tm.reset(); tm.start();
		if (!d_reader->nextFrame(d_frame))
			break;
		tm.stop();
		gpu_times.push_back(tm.getTimeMilli());
		gpu_frame_count++;

		cv::imshow("GPU", d_frame);
		//cv::waitKey(1);
		if (cv::waitKey(3) > 0)
			break;
	}

	if (!cpu_times.empty() && !gpu_times.empty())
	{
		std::cout << std::endl << "Results:" << std::endl;

		std::sort(cpu_times.begin(), cpu_times.end());
		std::sort(gpu_times.begin(), gpu_times.end());

		double cpu_avg = std::accumulate(cpu_times.begin(), cpu_times.end(), 0.0) / cpu_times.size();
		double gpu_avg = std::accumulate(gpu_times.begin(), gpu_times.end(), 0.0) / gpu_times.size();

		std::cout << "CPU : Avg : " << cpu_avg << " ms FPS : " << 1000.0 / cpu_avg << " Frames " << cpu_frame_count << std::endl;
		std::cout << "GPU : Avg : " << gpu_avg << " ms FPS : " << 1000.0 / gpu_avg << " Frames " << gpu_frame_count << std::endl;
	}

	return 0;
}

#include <iostream>
#include <vector>
//#include <getopt.h>

#include <opencv2/opencv.hpp>
#include "cyolov_dnn.h"

using namespace std;
using namespace cv;

int dnn_main(int argc, char **argv)
{
	std::string projectBasePath = "D:/Work/cartificial_intelligence/yolov5/data/images"; // Set your ultralytics base path

	bool runOnGPU = false;

	//
	// Pass in either:
	//
	// "yolov8s.onnx" or "yolov5s.onnx"
	//
	// To run Inference with yolov8/yolov5 (ONNX)
	//

	// Note that in this example the classes are hard-coded and 'classes.txt' is a place holder.
	chen::cyolov inf(projectBasePath + "/yolov5s.onnx", cv::Size(640, 480), "classes.txt", runOnGPU);

	std::vector<std::string> imageNames;
	imageNames.push_back(projectBasePath + "/bus.jpg");
	imageNames.push_back(projectBasePath + "/zidane.jpg");

	for (int i = 0; i < imageNames.size(); ++i)
	{
		cv::Mat frame = cv::imread(imageNames[i]);

		// Inference starts here...
		std::vector<chen::Detection> output = inf.runInference(frame);

		int detections = output.size();
		std::cout << "Number of detections:" << detections << std::endl;

		for (int i = 0; i < detections; ++i)
		{
			chen::Detection detection = output[i];

			cv::Rect box = detection.box;
			cv::Scalar color = detection.color;

			// Detection box
			cv::rectangle(frame, box, color, 2);

			// Detection box text
			std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
			cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
			cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

			cv::rectangle(frame, textBox, color, cv::FILLED);
			cv::putText(frame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
		}
		// Inference ends here...

		// This is only for preview purposes
		float scale = 0.8;
		cv::resize(frame, frame, cv::Size(frame.cols*scale, frame.rows*scale));
		cv::imshow("Inference", frame);

		cv::waitKey(-1);
	}
}




#else

int main()
{
	std::cout << "OpenCV was built without CUDA Video decoding support\n" << std::endl;
	return 0;
}

#endif
