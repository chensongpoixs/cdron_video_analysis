#include <iostream>
#include <memory>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudacodec.hpp>
#include "detector.h"
#include "cxxopts.h"
#include "clicense_plate.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
//opencv
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <json/json.h>
#include "cyolov_onnxruntime.h"
#include "inference.h"
/******************************************************************************************
Function:       Screenshot
Description:    矩形截图
Input:          src:原图片  rect:截图范围
Output:         dst:截图后的图片
Return:         截图成功返回true,失败返回false
*******************************************************************************************/
//bool Screenshot(cv::Mat* src, cv::Mat* dst, CvRect rect)
//{
//	try {
//		cvSetImageROI(src, rect);
//		cvCopy(src, dst, 0);
//		cvResetImageROI(src);
//		return true;
//	}
//
//	catch (cv::Exception e)
//	{
//	}
//}
//
///******************************************************************************************
//Function:       SafeResetSizeOfRect
//Description:    安全重置矩形大小
//Input:          src:原图片 rect:截图范围
//Return:         无
//*******************************************************************************************/
//void SafeResetSizeOfRect(cv::Mat* src, CvRect& rect)
//{
//	try
//	{
//		rect.x = rect.x < 0 ? 0 : rect.x;
//		rect.y = rect.y < 0 ? 0 : rect.y;
//		rect.width = rect.width < 0 ? 0 : rect.width;
//		rect.height = rect.height < 0 ? 0 : rect.height;
//
//		if (rect.x > src->width || rect.y > src->height)
//		{
//			rect = cvRect(0, 0, src->width, src->height);
//		}
//		rect.width = std::min(rect.width, src->width - rect.x);
//		rect.height = std::min(rect.height, src->height - rect.y);
//	}
//
//	catch (cv::Exception e)
//	{
//	}
//}

std::vector<std::string> LoadNames(const std::string& path) {
    // load class names
    std::vector<std::string> class_names;
    std::ifstream infile(path);
    if (infile.is_open()) {
        std::string line;
        while (getline (infile,line)) {
            class_names.emplace_back(line);
        }
        infile.close();
    }
    else {
        std::cerr << "Error loading the class names!\n";
    }

    return class_names;
}


void Demo(cv::Mat& img,
        const std::vector<std::vector<CDetection>>& detections,
        const std::vector<std::string>& class_names,
        bool label = true) {

    if (!detections.empty()) 
	{
		//Json::Value data;
		Json::Value arrayObj;
		Json::Value item;
		for (const std::vector<CDetection> & p : detections)
		{
			for (const auto& detection : p)
			{
				//# #  3：汽车
				//# #  4:面包车
				//# #  5:卡车
				//# #  6:三轮车
				//# #  7：遮阳篷三轮车
				//# #  8：公交车
				if (detection.class_idx == 2 || detection.class_idx == 3 || detection.class_idx == 4 || detection.class_idx == 7)
				{
					cv::Mat plate_img = img(detection.bbox);
					//cv::imshow("plate_img", plate_img);
					//cv::imwrite("test.jpg", img);
					//cv::waitKey(1);
					std::string plate_codc; 
					chen::g_license_plate.recognition(plate_img, plate_codc);
					//cvReleaseImage
					
				}
				const auto& box = detection.bbox;
				float score = detection.score;
				int class_idx = detection.class_idx;
				item["class"] = class_idx;
				item["x"] = box.x;
				item["y"] = box.y;
				item["width"] = box.width;
				item["height"] = box.height;
				arrayObj.append(item);
				cv::rectangle(img, box, cv::Scalar(0, 0, 255), 2);
				//cv::inRange();
				if (label) {
					std::stringstream ss;
					ss << std::fixed << std::setprecision(2) << score;
					std::string s = class_names[class_idx>0 ? class_idx-1: class_idx] + " " + ss.str();

					auto font_face = cv::FONT_HERSHEY_DUPLEX;
					auto font_scale = 1.0;
					int thickness = 1;
					int baseline = 0;
					cv::Size s_size = cv::getTextSize(s, font_face, font_scale, thickness, &baseline);
					cv::rectangle(img,
						cv::Point(box.tl().x, box.tl().y - s_size.height - 5),
						cv::Point(box.tl().x + s_size.width, box.tl().y),
						cv::Scalar(0, 0, 255), -1);
					cv::putText(img, s, cv::Point(box.tl().x, box.tl().y - 5),
						font_face, font_scale, cv::Scalar(255, 255, 255), thickness);
				}
			}
		}

		//printf("%u\n", __CUDA_ARCH__);
		printf("json = %s\n", arrayObj.toStyledString().c_str());
    }

   
	
    
}


int pppmain(int argc, const char* argv[])
{
    cxxopts::Options parser(argv[0], "A LibTorch inference implementation of the yolov5");

    // TODO: add other args
    parser.allow_unrecognised_options().add_options()
            ("weights", "model.torchscript.pt path", cxxopts::value<std::string>())
            ("source", "source", cxxopts::value<std::string>())
            ("conf-thres", "object confidence threshold", cxxopts::value<float>()->default_value("0.4"))
            ("iou-thres", "IOU threshold for NMS", cxxopts::value<float>()->default_value("0.5"))
            ("gpu", "Enable cuda device or cpu", cxxopts::value<bool>()->default_value("false"))
            ("view-img", "display results", cxxopts::value<bool>()->default_value("false"))
			("model", "model path", cxxopts::value<std::string>())
            ("h,help", "Print usage");

    auto opt = parser.parse(argc, argv);

    if (opt.count("help")) {
        std::cout << parser.help() << std::endl;
        exit(0);
    }

    // check if gpu flag is set
    bool is_gpu = opt["gpu"].as<bool>();
	//cv::cuda::setGlDevice();
	cv::cuda::setDevice(0);
    // set device type - CPU/GPU
    torch::DeviceType device_type;
    if (torch::cuda::is_available() && is_gpu) {
        device_type = torch::kCUDA;
    } else {
        device_type = torch::kCPU;
    }

    // load class names from dataset for visualization
    std::vector<std::string> class_names = LoadNames("weights/coco.names");
    if (class_names.empty()) {
        return -1;
    }



    // load network
    std::string weights = opt["weights"].as<std::string>();
    auto detector = Detector(weights, device_type);

    // load input image
    std::string source = opt["source"].as<std::string>();
	std::string modes_path = opt["model"].as<std::string>();
	chen::g_license_plate.init(modes_path.c_str());
	/////////////////////////////////////////////////////////////////////////////////
    //cv::Mat img = cv::imread(source);
	cv::VideoCapture video_capture =  cv::VideoCapture();

	std::vector<int> params{ cv::CAP_PROP_AUDIO_STREAM, -1,
							cv::CAP_PROP_VIDEO_STREAM, 1, cv::CAP_PROP_STREAM_OPEN_TIME_USEC };
   /* if (!video_capture.open(source, cv::CAP_MSMF, params))
	{
        std::cerr << "Error open  video  " << source <<" !\n";
        return -1;
    }*/
	video_capture.open(source, cv::CAP_FFMPEG);
	if (!video_capture.isOpened())
	{
		std::cerr << "ERROR! Can't to open file: "  << source << std::endl;
		return -1;
	}

	const int videoBaseIndex = (int)video_capture.get(cv::CAP_PROP_VIDEO_STREAM);
	const int numberOfChannels = (int)video_capture.get(cv::CAP_PROP_VIDEO_TOTAL_CHANNELS);
	//std::cout << "CAP_PROP_VIDEO_STREAM: " << cv::depthToString((int)video_capture.get(CAP_PROP_AUDIO_DATA_DEPTH)) << std::endl;
	//std::cout << "CAP_PROP_AUDIO_SAMPLES_PER_SECOND: " << cap.get(CAP_PROP_VI_SAMPLES_PER_SECOND) << std::endl;
	std::cout << "CAP_PROP_AUDIO_TOTAL_CHANNELS: " << numberOfChannels << std::endl;
	//std::cout << "CAP_PROP_VIDEO_TOTAL_STRIN: " << cap.get(cv::CAP_PROP_VI_TOTAL_STREAMS) << std::endl;

 //   // run once to warm up
    std::cout << "Run once on empty image" << std::endl;
	cv::Mat img; 
	 //////////////////////////////////////////////////////////////////////////////////////////////////////////
	//cv::OutputArray img;
	 //video_capture.read(img);
	/*while (!video_capture.grab()) { }
	{
		video_capture.retrieve(img, videoBaseIndex);

	}*/
	// cv::namedWindow("Result", cv::A);
	//cv::resizeWindow("AI", 800, 600);
	/*auto temp_img = cv::Mat::zeros(img.rows, img.cols, CV_32FC3);
	detector.Run(temp_img, 1.0f, 1.0f);*/
	// set up threshold
	float conf_thres = opt["conf-thres"].as<float>();
	float iou_thres = opt["iou-thres"].as<float>();
	std::vector<std::vector<CDetection>> result;
	/////////////////////////////////////////////////////////////////
	//if (!cv:videoio_registry::hasBackend(CAP_FFMPEG))
		//throw SkipTestException("FFmpeg backend not found");
	// 创建CUDA解码器
	//cv::cuda::setDevice(0);
	//cv::cuda::GpuMat d_frame;
	//cv::Ptr<cv::cudacodec::VideoReader> d_reader = cv::cudacodec::createVideoReader(source, params);
	//const int videoBaseIndex = (int)d_reader.get(cv::CAP_PROP_VIDEO_STREAM);
	bool one = true;
	int frame_count = 0;
	for (;;)
	{
		if (video_capture.grab() /*d_reader->grab() && d_reader->nextFrame(d_frame)*/)
		{
			
			++frame_count;
			video_capture.retrieve(img, videoBaseIndex);
			if (one)
			{
				//cv::cuda::GpuMat temp_img = cv::cuda::GpuMat(d_frame.rows, d_frame.cols, CV_32FC3);
				auto temp_img = cv::Mat::zeros(img.rows, img.cols, CV_32FC3);
				detector.Run(temp_img, 1.0f, 1.0f);
				one = false;
			}
			auto start = std::chrono::high_resolution_clock::now();
			//if (frame_count > 5)
			{
				result = detector.Run(img, conf_thres, iou_thres);
				frame_count = 0;
			}
			Demo(img, result, class_names, true);
			auto  end = std::chrono::high_resolution_clock::now();
			auto  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			// It should be known that it takes longer time at first time
			std::cout << "=======> post-process takes : " << duration.count() << " ms" << std::endl;
			//cv::resizeWindow("cdron_video_analysis", 800, 600);
			//cv::imshow("cdron_video_analysis", img);
			//cv::imwrite("test.jpg", img);
			//cv::waitKey(1);
		}

   }
    cv::destroyAllWindows();
    return 0;
}



#include <iostream>
#include "c_api/hyper_lpr_sdk.h"
#include "opencv2/opencv.hpp"

static const std::vector<std::string> TYPES = { "蓝牌", "黄牌单层", "白牌单层", "绿牌新能源", "黑牌港澳", "香港单层", "香港双层", "澳门单层", "澳门双层", "黄牌双层" };

int test_main(int argc, char **argv) {
	char *model_path = argv[1];
	char *image_path = argv[2];
	cv::Mat image = cv::imread(image_path);
	// create ImageData
	HLPR_ImageData data = { 0 };
	data.data = image.ptr<uint8_t>(0);
	data.width = image.cols;
	data.height = image.rows;
	data.format = STREAM_BGR;
	data.rotation = CAMERA_ROTATION_0;
	// create DataBuffer
	P_HLPR_DataBuffer buffer = HLPR_CreateDataBuffer(&data);

	// create context
	HLPR_ContextConfiguration configuration = { 0 };
	configuration.models_path = model_path;
	configuration.max_num = 5;
	configuration.det_level = DETECT_LEVEL_LOW;
	configuration.use_half = false;
	configuration.nms_threshold = 0.5f;
	configuration.rec_confidence_threshold = 0.5f;
	configuration.box_conf_threshold = 0.30f;
	configuration.threads = 1;
	P_HLPR_Context ctx = HLPR_CreateContext(&configuration);
	HREESULT ret = HLPR_ContextQueryStatus(ctx);
	if (ret != HResultCode::Ok) {
		printf("create error.\n");
		return -1;
	}
	// exec plate recognition
	HLPR_PlateResultList results = { 0 };
	double time;
	time = (double)cv::getTickCount();
	HLPR_ContextUpdateStream(ctx, buffer, &results);
	time = ((double)cv::getTickCount() - time) / cv::getTickFrequency();
	printf("cost: %f\n", time);


	for (int i = 0; i < results.plate_size; ++i) {
		std::string type;
		if (results.plates[i].type == HLPR_PlateType::PLATE_TYPE_UNKNOWN) {
			type = "未知";
		}
		else {
			type = TYPES[results.plates[i].type];
		}

		cv::rectangle(image, cv::Point2f(results.plates[i].x1, results.plates[i].y1), cv::Point2f(results.plates[i].x2, results.plates[i].y2),
			cv::Scalar(100, 100, 200), 3);

		printf("<%d> %s, %s, %f\n", i + 1, type.c_str(),
			results.plates[i].code, results.plates[i].text_confidence);
	}

	//    cv::imwrite("out.jpg", image);
	cv::imshow("out", image);
	cv::waitKey(0);

	// release buffer
	HLPR_ReleaseDataBuffer(buffer);
	// release context
	HLPR_ReleaseContext(ctx);

	return 0;
}

#include "cdron_video_analysls.h"
#include <csignal>
void Stop(int i)
{

	chen::g_cdron_video_analysls.stop();
}

void RegisterSignal()
{
	signal(SIGINT, Stop);
	signal(SIGTERM, Stop);

}
#include <iostream>
#include <stdio.h>
#include "inference.h"
#include <filesystem>

//
//
//void file_iterator(DCSP_CORE*& p)
//{
//	//std::filesystem::path img_path = R"(E:\project\Project_C++\DCPS_ONNX\TEST_ORIGIN)";
//	//int k = 0;
//	//for (auto& i : std::filesystem::directory_iterator(img_path))
//	{
//		//if (i.path().extension() == ".jpg")
//		{
//			std::string img_path = "D:/Work/cartificial_intelligence/yolov5/data/images/bus.jpg";
//			//std::cout << img_path << std::endl;
//			cv::Mat img = cv::imread(img_path);
//			std::vector<DCSP_RESULT> res;
//			char* ret = p->RunSession(img, res);
//			for (int i = 0; i < res.size(); i++)
//			{
//				cv::rectangle(img, res.at(i).box, cv::Scalar(125, 123, 0), 3);
//			}
//			 
//			cv::imshow("TEST_ORIGIN", img);
//			cv::waitKey(0);
//			cv::destroyAllWindows();
//			//cv::imwrite("E:\\output\\" + std::to_string(k) + ".png", img);
//		}
//	}
//}






#include <opencv2/opencv.hpp>

#include "cyolov_dnn.h"

using namespace std;
using namespace cv;
//
//int test_yolov_main(int argc, char **argv)
//{
//	std::string projectBasePath = "D:/Work/cartificial_intelligence/yolov5/data/images"; // Set your ultralytics base path
//
//	bool runOnGPU = true;
//
//	//
//	// Pass in either:
//	//
//	// "yolov8s.onnx" or "yolov5s.onnx"
//	//
//	// To run Inference with yolov8/yolov5 (ONNX)
//	//
//
//	// Note that in this example the classes are hard-coded and 'classes.txt' is a place holder.
//	chen::cyolov inf(projectBasePath + "/yolov8s.onnx", cv::Size(640, 480), "classes.txt", runOnGPU);
//
//	std::vector<std::string> imageNames;
//	imageNames.push_back(projectBasePath + "/bus.jpg");
//	imageNames.push_back(projectBasePath + "/zidane.jpg");
//
//	for (int i = 0; i < imageNames.size(); ++i)
//	{
//		cv::Mat frame = cv::imread(imageNames[i]);
//
//		// Inference starts here...
//		std::vector<chen::Detection> output = inf.runInference(frame);
//
//		int detections = output.size();
//		std::cout << "Number of detections:" << detections << std::endl;
//
//		for (int i = 0; i < detections; ++i)
//		{
//			chen::Detection detection = output[i];
//
//			cv::Rect box = detection.box;
//			cv::Scalar color = detection.color;
//
//			// Detection box
//			cv::rectangle(frame, box, color, 2);
//
//			// Detection box text
//			std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
//			cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
//			cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);
//
//			cv::rectangle(frame, textBox, color, cv::FILLED);
//			cv::putText(frame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
//		}
//		// Inference ends here...
//
//		// This is only for preview purposes
//		float scale = 0.8;
//		cv::resize(frame, frame, cv::Size(frame.cols*scale, frame.rows*scale));
//		cv::imshow("Inference", frame);
//
//		cv::waitKey(-1);
//	}
//}
#include "detector.h"
void test_onnxruntime()
{
	chen::cyolov_onnxruntime detector;
	bool isGPU = true;
	chen::Utils utils;
	const vector<string> classNames = utils.loadNames("D:/Work/cai/cdron_video_analysis/bin/x64/Release/weights/visdrone.names");
	if (classNames.empty()) {
		cerr << "Error: Empty class names file" << endl;
		return;
	}
	 
	std::vector<CDetection> result;
	 
	detector.YOLODetector(isGPU, cv::Size(640, 640));
	std::cout << "Model was initialized......" << std::endl;
	//image = cv::imread("D:/Work/cartificial_intelligence/yolov5/data/images/bus.jpg");
	cv::VideoCapture* m_video_cap_ptr =  new cv::VideoCapture();

	m_video_cap_ptr->open("rtsp://admin:admin12345@192.168.2.213", cv::CAP_FFMPEG);
	if (!m_video_cap_ptr->isOpened())
	{
		std::cerr << "ERROR! Can't to open file: "   << std::endl;
		//WARNING_EX_LOG("Can't ot open [source = %s]", source.c_str());
		return  ;
	}

	//const int videoBaseIndex = (int)m_video_cap_ptr->get(cv::CAP_PROP_VIDEO_STREAM);
	int m_video_index = (int)m_video_cap_ptr->get(cv::CAP_PROP_VIDEO_TOTAL_CHANNELS);
	


	{
		cv::Mat img;
		 
		while (true)
		{
			
			if (m_video_cap_ptr->grab() /*d_reader->grab() && d_reader->nextFrame(d_frame)*/)
			{ 
				m_video_cap_ptr->retrieve(img, m_video_index);
				auto start = std::chrono::high_resolution_clock::now();
				result = detector.detect(img, 0.25, 0.4);
				 
				
				auto  end = std::chrono::high_resolution_clock::now();
				auto  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
				// It should be known that it takes longer time at first time
				std::cout <<"\n"<< std::endl;
				std::cout << "=======> post-process takes : " << duration.count() << " ms" << std::endl;
				utils.visualizeDetection(img, result, classNames);
				//cv::resizeWindow("cdron_video_analysis", 800, 600);
				cv::imshow("cdron_video_analysis", img);
				//cv::imwrite("test.jpg", img);
				cv::waitKey(1);
				 

			}

		}
		
	
	}


	 
	 
	cv::destroyAllWindows();
}


void test_plate()
{
	chen::g_license_plate.init("./resource/models/r2_mobile");
	std::string plate_code;
	cv::Mat plate_img = cv::imread("./resource/images/test_img.jpg");
	chen::g_license_plate.recognition(plate_img, plate_code);
}

int main(int argc, char *argv[])
{
	/*test_plate();
	return 0;*/
	//test_onnxruntime();
	//return 0;
	//test_yolov_main(0,NULL);


	/*DCSP_CORE* p1 = new DCSP_CORE;
	std::string model_path = "D:/Work/cartificial_intelligence/yolov5/data/images/yolov8s.onnx";
	DCSP_INIT_PARAM params{ model_path, YOLO_ORIGIN_V8, {640, 640}, 80, 0.01, 0.3, true };
	char* ret = p1->CreateSession(params);
	file_iterator(p1);*/
	//return 0;
	RegisterSignal();

	const char* config_filename = "server.cfg";
	if (argc > 1)
	{
		config_filename = argv[1];
	}
	const char* log_path = "./log";
	if (argc > 2)
	{
		log_path = argv[2];
	}




	bool init = chen::g_cdron_video_analysls.init(log_path, config_filename);

	if (init)
	{
		init = chen::g_cdron_video_analysls.Loop();
	}

	chen::g_cdron_video_analysls.destroy();
	if (!init)
	{
		return 1;
	}
	return 0;
}


