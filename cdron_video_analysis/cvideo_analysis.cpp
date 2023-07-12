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
#include "cyolov_onnxruntime.h"
#include "clicense_plate.h"
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
		m_license_plate.init("resource/models/r2_mobile");
		return true;
	}
	bool cvideo_analysis::startup(const std::string & source)
	{
		if (!m_stoped)
		{
			WARNING_EX_LOG("[source = %s] stoped ", m_source_path.c_str());
			return false;
			if (m_thread.joinable())
			{
				m_thread.join();
			}
		}

		if (m_video_analysis_type != EVideoAnalysisONNXRuntime)
		{
			WARNING_EX_LOG("video analysis type = %u", m_video_analysis_type);
			return false;
		}
		// load class names from dataset for visualization
		class_names = LoadNames("weights/visdrone.names");
		if (class_names.empty())
		{
			WARNING_EX_LOG("not find weights/visdrone.names !!!");
			return false;
		}
		if (m_video_analysis_type == EVideoAnalysisTorchScript)
		{
			torch::DeviceType device_type;
			if (torch::cuda::is_available() && true)
			{
				device_type = torch::kCUDA;
			}
			else {
				device_type = torch::kCPU;
			} 
			// load network
			std::string weights = "./weights/VisDrone.torchscript";
			m_detector_ptr = new Detector(weights, device_type);
		}
		else  if (m_video_analysis_type == EVideoAnalysisONNXRuntime)
		{
			m_onnxruntime_ptr = new cyolov_onnxruntime();
			if (!m_onnxruntime_ptr)
			{
				WARNING_EX_LOG("alloc onnxruntime failed !!!");
				return false;
			}
			m_onnxruntime_ptr->YOLODetector(true, cv::Size(640, 640));
			NORMAL_EX_LOG("[source = %s]onnxruntime  init OK !!!", source.c_str());
		}


		m_video_cap_ptr = new cv::VideoCapture();

		m_video_cap_ptr->open(source, cv::CAP_FFMPEG);
		if (!m_video_cap_ptr->isOpened())
		{
			std::cerr << "ERROR! Can't to open file: " << source << std::endl;
			WARNING_EX_LOG("Can't ot open [source = %s]", source.c_str());
			return false;
		}

		//const int videoBaseIndex = (int)m_video_cap_ptr->get(cv::CAP_PROP_VIDEO_STREAM);
		m_video_index = (int)m_video_cap_ptr->get(cv::CAP_PROP_VIDEO_TOTAL_CHANNELS);
		m_stoped = false;
		m_thread = std::thread(&cvideo_analysis::_work_pthread, this);

		return true;
	}
	void cvideo_analysis::stop()
	{
		/*if (m_video_cap_ptr)
		{
			m_video_cap_ptr->release();
		}*/
		m_stoped = true;
	}
	void cvideo_analysis::destroy()
	{
		m_stoped = true;
		if (m_thread.joinable())
		{
			m_thread.join();
			
		}
		NORMAL_EX_LOG("[source = %s] thread exit OK !!!", m_source_path.c_str());
		if (m_detector_ptr)
		{
			delete m_detector_ptr;
			m_detector_ptr = NULL;
			NORMAL_EX_LOG("[source = %s]   totch delete !!!", m_source_path.c_str());
		}
		if (m_onnxruntime_ptr)
		{
			delete m_onnxruntime_ptr;
			m_onnxruntime_ptr = NULL;
			NORMAL_EX_LOG("[source = %s] onnxruntime delete !!!", m_source_path.c_str());
		}
		if (m_video_cap_ptr)
		{
			m_video_cap_ptr->release();
			delete m_video_cap_ptr;
			m_video_cap_ptr = NULL;
			NORMAL_EX_LOG("[source = %s]  video cap delete OK !!!", m_source_path.c_str());
		}
		m_license_plate.destroy();
		NORMAL_EX_LOG("[source = %s] destroy !!!", m_source_path.c_str());
		class_names.clear();
	}
	void cvideo_analysis::set_skip_frame(uint32 count)
	{
		if (count > 60)
		{
			WARNING_EX_LOG("[source = %s][skip_frame = %u]", m_source_path.c_str(), count);
			count = 60;
		}
		m_skip_frame = count;
	}

	void cvideo_analysis::set_car_analysis(uint32 analysis)
	{
		m_car_analysis = analysis;
	}

	void cvideo_analysis::_work_pthread()
	{
		bool one = true;
		cv::Mat img;
		std::vector<std::vector<CDetection>> result;
		std::vector<CDetection> onnxruntimeresult;
		uint32 skip_frame_count = 0;
		//g_license_plate.init("D:/Work/cartificial_intelligence/HyperLPR/resource/models/r2_mobile");
		while (!m_stoped)
		{
			auto start = std::chrono::high_resolution_clock::now();
			if (m_video_cap_ptr->grab() /*d_reader->grab() && d_reader->nextFrame(d_frame)*/)
			{

				 
				m_video_cap_ptr->retrieve(img, m_video_index);
				
				if (++skip_frame_count > m_skip_frame)
				{
					skip_frame_count = 0;
					if (one && m_detector_ptr)
					{
						//cv::cuda::GpuMat temp_img = cv::cuda::GpuMat(d_frame.rows, d_frame.cols, CV_32FC3);
						auto temp_img = cv::Mat::zeros(img.rows, img.cols, CV_32FC3);
						m_detector_ptr->Run(temp_img, 1.0f, 1.0f);
						one = false;
					}
					if (m_video_analysis_type == EVideoAnalysisTorchScript)
					{
						result = m_detector_ptr->Run(img, 0.25, 0.45);
					}
					else if (m_video_analysis_type == EVideoAnalysisONNXRuntime)
					{
						onnxruntimeresult = m_onnxruntime_ptr->detect(img, 0.25, 0.45);
						result.push_back(onnxruntimeresult);
					}

					_send_video_info(img, result, class_names, false);
					result.clear();
					auto  end = std::chrono::high_resolution_clock::now();
					auto  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
					// It should be known that it takes longer time at first time
					//std::cout << "=======> post-process takes : " << duration.count() << " ms" << std::endl;
					NORMAL_EX_LOG("[source=%s] post-process takes : %u ms", m_source_path.c_str(), duration.count());
					using namespace chen;
					if (g_cfg.get_uint32(ECI_OpencvShow))
					{
						//cv::resizeWindow(m_source_path, 800, 600);
						cv::imshow(m_source_path, img);
						//cv::imwrite("test.jpg", img);
						cv::waitKey(1);
					}
				}
				
			}

		}
		//cv::destroyAllWindows();
	}

	void cvideo_analysis::_send_video_info(cv::Mat& img,
		const std::vector<std::vector<CDetection>>& detections,
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
			Json::Value vehicle_info;

			//////////////
			





			Json::Value item;
			for (const std::vector<CDetection> & pvec : detections)
			{
				for (const CDetection& detection : pvec)
				{ 
					if ((detection.bbox.x < 0 || detection.bbox.y < 0) || (detection.bbox.x + detection.bbox.width > img.size().width || detection.bbox.y + detection.bbox.height > img.size().height))
					{
						continue;
					 }
					item["x"] = detection.bbox.x;
					item["y"] = detection.bbox.y;
					item["width"] = detection.bbox.width;
					item["height"] = detection.bbox.height;

					//# #  3：汽车
					//# #  4:面包车
					//# #  5:卡车
					//# #  6:三轮车
					//# #  7：遮阳篷三轮车
					//# #  8：公交车
					std::string plate_code;
					if (m_car_analysis> 0&&(detection.class_idx == 3 || detection.class_idx == 4 || detection.class_idx == 5 || detection.class_idx == 8))
					{
						 
							//detection.show();
							cv::Mat plate_img = img(detection.bbox);
							//cv::imshow("plate_img", plate_img);
							//cv::imwrite("test.jpg", img);
							//cv::waitKey(1);
							
							m_license_plate.recognition(plate_img, plate_code);
							
						//cvReleaseImage

					}

					if (g_cfg.get_uint32(ECI_OpencvShow))
					{
						cv::rectangle(img, detection.bbox, cv::Scalar(0, 0, 255), 2);
						int conf = (int)std::round(detection.score * 100);
						std::string s = class_names[ detection.class_idx] + " " +  " 0." + std::to_string(conf) + " : " + plate_code;
						
						 
						 


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
						if (!plate_code.empty())
						{
							vehicle_info["license_plate"] = plate_code;
							item["vehicle_info"] = vehicle_info;
						}
						 car_obj.append(item);
						 
					}
					else if (detection.class_idx == 4)
					{ 
						if (!plate_code.empty())
						{
							vehicle_info["license_plate"] = plate_code;
							item["vehicle_info"] = vehicle_info;
						}
						  var_obj.append(item);
						 
					}
					else if (detection.class_idx == 5)
					{ 
						if (!plate_code.empty())
						{
							vehicle_info["license_plate"] = plate_code;
							item["vehicle_info"] = vehicle_info;
						}
						 truck_obj.append(item);
					}
					else if (detection.class_idx == 8)
					{
						if (!plate_code.empty())
						{
							vehicle_info["license_plate"] = plate_code;
							item["vehicle_info"] = vehicle_info;
						}
						  bus_obj.append(item);
					}
					
				}
			}
			 


			
			if (!pedestrian_obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 0;
				class_object["class_data"] = pedestrian_obj;
				data.append(class_object);
			}
			
			 
			if (!people_Obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 1;
				class_object["class_data"] = people_Obj;
				data.append(class_object);
			}

			if (!car_obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 3;
				class_object["class_data"] = car_obj;
				data.append(class_object);
			}

			if (!var_obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 4;
				class_object["class_data"] = var_obj;
				data.append(class_object);
			}


			if (!truck_obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 5;
				class_object["class_data"] = truck_obj;
				data.append(class_object);
			}


			if (!bus_obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 8;
				class_object["class_data"] = bus_obj;
				data.append(class_object);
			}
			if (!data.empty())
			{
				s_mqtt_client_mgr.publish("video_analysis/result", data.toStyledString());
			}
			//NORMAL_EX_LOG("json = %s\n", data.toStyledString().c_str());
		} 
	} 
}