#include <iostream>
#include <memory>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudacodec.hpp>
#include "detector.h"
#include "cxxopts.h"


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
        const std::vector<std::vector<Detection>>& detections,
        const std::vector<std::string>& class_names,
        bool label = true) {

    if (!detections.empty()) 
	{
		for (const std::vector<Detection> & p : detections)
		{
			for (const auto& detection : p)
			{
				const auto& box = detection.bbox;
				float score = detection.score;
				int class_idx = detection.class_idx;

				cv::rectangle(img, box, cv::Scalar(0, 0, 255), 2);

				if (label) {
					std::stringstream ss;
					ss << std::fixed << std::setprecision(2) << score;
					std::string s = class_names[class_idx] + " " + ss.str();

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
    }

   
	
    
}


int test_main(int argc, const char* argv[])
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
	std::vector<std::vector<Detection>> result;
	/////////////////////////////////////////////////////////////////
	//if (!cv:videoio_registry::hasBackend(CAP_FFMPEG))
		//throw SkipTestException("FFmpeg backend not found");
	// ´´½¨CUDA½âÂëÆ÷
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
			//if (frame_count > 5)
			{
				result = detector.Run(img, conf_thres, iou_thres);
				frame_count = 0;
			}
			Demo(img, result, class_names);
			//cv::resizeWindow("cdron_video_analysis", 800, 600);
			cv::imshow("cdron_video_analysis", img);
			//cv::imwrite("test.jpg", img);
			cv::waitKey(1);
		}

   }
    cv::destroyAllWindows();
    return 0;
}



#include <iostream>
#include "c_api/hyper_lpr_sdk.h"
#include "opencv2/opencv.hpp"

static const std::vector<std::string> TYPES = { "À¶ÅÆ", "»ÆÅÆµ¥²ã", "°×ÅÆµ¥²ã", "ÂÌÅÆÐÂÄÜÔ´", "ºÚÅÆ¸Û°Ä", "Ïã¸Ûµ¥²ã", "Ïã¸ÛË«²ã", "°ÄÃÅµ¥²ã", "°ÄÃÅË«²ã", "»ÆÅÆË«²ã" };

int main(int argc, char **argv) {
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
			type = "Î´Öª";
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