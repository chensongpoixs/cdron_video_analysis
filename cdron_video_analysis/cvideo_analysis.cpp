/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#include "cvideo_analysis.h"
#include "detector.h"
#include <json/json.h>
#include "clog.h"
#include "cmqtt_mgr.h"
#include "ccfg.h"
namespace chen {

	std::vector<std::string> LoadNames(const std::string& path) {
		// load class names
		std::vector<std::string> class_names;
		std::ifstream infile(path);
		if (infile.is_open()) {
			std::string line;
			while (getline(infile, line)) {
				class_names.emplace_back(line);
			}
			infile.close();
		}
		else {
			std::cerr << "Error loading the class names!\n";
		}

		return class_names;
	}


	bool cvideo_analysis::init()
	{
		return false;
	}
	bool cvideo_analysis::startup(const std::string & source)
	{
		torch::DeviceType device_type;
		if (torch::cuda::is_available() && true)
		{
			device_type = torch::kCUDA;
		}
		else {
			device_type = torch::kCPU;
		}

		// load class names from dataset for visualization
		 class_names = LoadNames("weights/coco.names");
		if (class_names.empty())
		{
			return -1;
		}



		// load network
		std::string weights = "./weights/VisDrone.torchscript";
		m_detector_ptr =new Detector(weights, device_type);


		m_video_cap_ptr = new cv::VideoCapture();

		m_video_cap_ptr->open(source, cv::CAP_FFMPEG);
		if (!m_video_cap_ptr->isOpened())
		{
			std::cerr << "ERROR! Can't to open file: " << source << std::endl;
			return -1;
		}

		//const int videoBaseIndex = (int)m_video_cap_ptr->get(cv::CAP_PROP_VIDEO_STREAM);
		m_video_index = (int)m_video_cap_ptr->get(cv::CAP_PROP_VIDEO_TOTAL_CHANNELS);

		m_thread = std::thread(&cvideo_analysis::_work_pthread, this);

		return true;
	}
	void cvideo_analysis::stop()
	{
	}
	void cvideo_analysis::destroy()
	{
	}
	void cvideo_analysis::_work_pthread()
	{
		bool one = true;
		cv::Mat img;
		std::vector<std::vector<Detection>> result;
		for (;;)
		{
			auto start = std::chrono::high_resolution_clock::now();
			if (m_video_cap_ptr->grab() /*d_reader->grab() && d_reader->nextFrame(d_frame)*/)
			{

				 
				m_video_cap_ptr->retrieve(img, m_video_index);
				if (one)
				{
					//cv::cuda::GpuMat temp_img = cv::cuda::GpuMat(d_frame.rows, d_frame.cols, CV_32FC3);
					auto temp_img = cv::Mat::zeros(img.rows, img.cols, CV_32FC3);
					m_detector_ptr->Run(temp_img, 1.0f, 1.0f);
					one = false;
				}
				
				//if (frame_count > 5)
				{
					result =  m_detector_ptr->Run(img, 0.4, 0.5);
					//frame_count = 0;
				}
				_send_video_info(img, result, class_names, false);
				auto  end = std::chrono::high_resolution_clock::now();
				auto  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
				// It should be known that it takes longer time at first time
				std::cout << "=======> post-process takes : " << duration.count() << " ms" << std::endl;
				using namespace chen;
				if (g_cfg.get_uint32(ECI_OpencvShow))
				{
					//cv::resizeWindow("cdron_video_analysis", 800, 600);
					cv::imshow("cdron_video_analysis", img);
					//cv::imwrite("test.jpg", img);
					cv::waitKey(1);
				}
				
			}

		}
		cv::destroyAllWindows();
	}

	void cvideo_analysis::_send_video_info(cv::Mat& img,
		const std::vector<std::vector<Detection>>& detections,
		const std::vector<std::string>& class_names,
		bool label) 
	{

		if (!detections.empty())
		{
			Json::Value data;
			Json::Value pedestrian_obj;;
			Json::Value people_Obj;
			Json::Value car_obj;
			Json::Value var_obj;
			Json::Value truck_obj;
			Json::Value bus_obj;

			//////////////
			





			Json::Value item;
			for (const std::vector<Detection> & pvec : detections)
			{
				for (const Detection& detection : pvec)
				{ 
					 
					item["x"] = detection.bbox.x;
					item["y"] = detection.bbox.y;
					item["width"] = detection.bbox.width;
					item["height"] = detection.bbox.height;
					if (g_cfg.get_uint32(ECI_OpencvShow))
					{
						cv::rectangle(img, detection.bbox, cv::Scalar(0, 0, 255), 2);
						std::stringstream ss;
						ss << std::fixed << std::setprecision(2) << score;
						std::string s = class_names[/*detection.class_idx > 0 ? detection.class_idx - 1 :*/ detection.class_idx] + " " + ss.str();

						auto font_face = cv::FONT_HERSHEY_DUPLEX;
						auto font_scale = 1.0;
						int thickness = 1;
						int baseline = 0;
						cv::Size s_size = cv::getTextSize(s, font_face, font_scale, thickness, &baseline);
						cv::rectangle(img,
							cv::Point(detection.bbox.tl().x, detection.bbox.tl().y - s_size.height - 5),
							cv::Point(detection.bbox.tl().x + s_size.width, detection.bbox.tl().y),
							cv::Scalar(0, 0, 255), -1);
						cv::putText(img, s, cv::Point(detection.bbox.tl().x, detection.bbox.tl().y - 5),
							font_face, font_scale, cv::Scalar(255, 255, 255), thickness);
					}
					if (detection.class_idx == 0)
					{
						  pedestrian_obj.append(item);
						
					}
					else if (detection.class_idx == 1)
					{
						  people_Obj.append(item);
						
					}
					else if (detection.class_idx == 3)
					{
						 car_obj.append(item);
						 
					}
					else if (detection.class_idx == 4)
					{ 
						  var_obj.append(item);
						 
					}
					else if (detection.class_idx == 5)
					{ 
						 truck_obj.append(item);
					}
					else if (detection.class_idx == 8)
					{
						  bus_obj.append(item);
					}
					
				}
			}
			 


			
			if (!pedestrian_obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 0;
				class_object["array"] = pedestrian_obj;
				data["data"].append(class_object);
			}
			
			 
			if (!people_Obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 1;
				class_object["array"] = people_Obj;
				data["data"].append(class_object);
			}

			if (!car_obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 3;
				class_object["array"] = car_obj;
				data["data"].append(class_object);
			}

			if (!var_obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 4;
				class_object["array"] = var_obj;
				data["data"].append(class_object);
			}


			if (!truck_obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 5;
				class_object["array"] = truck_obj;
				data["data"].append(class_object);
			}


			if (!bus_obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 8;
				class_object["array"] = bus_obj;
				data["data"].append(class_object);
			}
			s_mqtt_client_mgr.publish("video_analysis/result", data.toStyledString());
			//NORMAL_EX_LOG("json = %s\n", data.toStyledString().c_str());
		}




	}

}