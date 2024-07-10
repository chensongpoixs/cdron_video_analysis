/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#define _CRT_SECURE_NO_WARNINGS
#include "cvideo_analysis.h"
//#include "detector.h"
#include <json/json.h>
#include <json/value.h>
#include "clog.h"
#include "chttp_face_util.h"
//#include "cmqtt_mgr.h"
#include<opencv2/opencv.hpp>
#include<opencv2/imgcodecs.hpp>
#include<opencv2/imgcodecs/legacy/constants_c.h> //出现CV_LOAD_IMAGE_COLOR未声明错误，通常是这个头文件未被包含
#include "facedetectcnn.h"
#include "ccfg.h"
//#include "cyolov_onnxruntime.h"
//#include "clicense_plate.h"
#include <mutex>
namespace chen {
	static std::mutex					g_video_analysis_lock;
	//std::vector<std::string> LoadNames(const std::string& path) {
	//	// load class names
	//	std::vector<std::string> class_names;
	//	std::ifstream infile(path);
	//	if (infile.is_open()) {
	//		std::string line;
	//		while (getline(infile, line)) {
	//			class_names.emplace_back(line);
	//		}
	//		infile.close();
	//	}
	//	else {
	//		std::cerr << "Error loading the class names!\n";
	//	}

	//	return class_names;
	//}


	bool cvideo_analysis::init()
	{
		//m_license_plate.init("resource/models/r2_mobile");
		//m_car_color_ptr = new ctorch_classify("weights/car_color.torchscript");
		//m_car_type_ptr = new ctorch_classify("weights/car_type.torchscript");
		m_action = 0;
		return true;
	}
	bool cvideo_analysis::startup(const std::string& source)
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

#if 0
		if (m_video_analysis_type != EVideoAnalysisONNXRuntime && m_video_analysis_type != EVideoAnalysisTorchScript)
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
#endif
#if 0
		if (m_video_analysis_type == EVideoAnalysisTorchScript)
		{
			torch::DeviceType device_type;
			if (torch::cuda::is_available() && true)
			{
				device_type = torch::kCUDA;
			}
			else
			{
				device_type = torch::kCPU;
			}
			// load network
			std::string weights = "./weights/VisDrone.torchscript";
			//m_yolov_ptr = new cyolov_torch(weights, "v5", "gpu");
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
#endif

		//m_video_cap_ptr = new cv::VideoCapture();

		//m_video_cap_ptr->open(source, cv::CAP_FFMPEG);
		//if (!m_video_cap_ptr->isOpened())
		//{
		//	std::cerr << "ERROR! Can't to open file: " << source << std::endl;
		//	WARNING_EX_LOG("Can't ot open [source = %s]", source.c_str());
		//	return false;
		//}
		m_source_path = source;
		m_stoped = false;
		if (!_open())
		{
			return false;
		}
		m_action = 1;
		//const int videoBaseIndex = (int)m_video_cap_ptr->get(cv::CAP_PROP_VIDEO_STREAM);
		//m_video_index = (int)m_video_cap_ptr->get(cv::CAP_PROP_VIDEO_TOTAL_CHANNELS);

		m_decode_thread = std::thread(&cvideo_analysis::_work_decode_thread, this);
		//m_thread = std::thread(&cvideo_analysis::_work_analysis_pthread, this);

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
		NORMAL_EX_LOG("[source = %s] exit !!!", m_source_path.c_str());
		m_stoped = true;
		m_action = 1;
		if (m_decode_thread.joinable())
		{
			m_decode_thread.join();
		}
		NORMAL_EX_LOG("[source = %s] exit !!!", m_source_path.c_str());

		if (m_thread.joinable())
		{
			m_thread.join();

		}
		NORMAL_EX_LOG("[source = %s] exit !!!", m_source_path.c_str());

		for (cv::Mat& m : m_queue)
		{
			m.release();
		}
		m_queue.clear();
		NORMAL_EX_LOG("[source = %s] thread exit OK !!!", m_source_path.c_str());
#if 0
		if (m_detector_ptr)
		{
			std::lock_guard<std::mutex> lock(g_video_analysis_lock);
			delete m_detector_ptr;
			m_detector_ptr = NULL;
			NORMAL_EX_LOG("[source = %s]   totch delete !!!", m_source_path.c_str());
		}
		if (m_yolov_ptr)
		{
			delete m_yolov_ptr;
			m_yolov_ptr = NULL;
			NORMAL_EX_LOG("[source = %s]  yolov totch delete !!!", m_source_path.c_str());
		}
#endif
		NORMAL_EX_LOG("[source = %s] thread exit OK !!!", m_source_path.c_str());
#if defined(_MSC_VER)
#if 0

		if (m_onnxruntime_ptr)
		{
			delete m_onnxruntime_ptr;
			m_onnxruntime_ptr = NULL;
			NORMAL_EX_LOG("[source = %s] onnxruntime delete !!!", m_source_path.c_str());
		}
#endif
		NORMAL_EX_LOG("[source = %s] thread exit OK !!!", m_source_path.c_str());
#endif 
		if (m_video_cap_ptr)
		{
			m_video_cap_ptr->release();
			delete m_video_cap_ptr;
			m_video_cap_ptr = NULL;
			NORMAL_EX_LOG("[source = %s]  video cap delete OK !!!", m_source_path.c_str());
		}
		NORMAL_EX_LOG("[source = %s] thread exit OK !!!", m_source_path.c_str());
		//m_license_plate.destroy();
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
	void cvideo_analysis::set_result_video_analysis(const std::string& result_video_analysis)
	{
		m_result_video_analysis = "video_analysis/" + result_video_analysis;
	}

	void cvideo_analysis::_work_analysis_pthread()
	{

		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::milliseconds ms;
		uint32_t elapse = 0;
		
		/*uint32_t total_ms = 100 / g_cfg.get_uint32(ECI_AiVideoFrame);
		std::vector<std::vector<CDetection>> result;
		std::vector<CDetection> onnxruntimeresult;*/
		cv::Mat img;
		bool one = true;
		bool data = false;

#if 0
		{
			if (m_video_analysis_type == EVideoAnalysisTorchScript)
			{
				torch::DeviceType device_type;
				if (torch::cuda::is_available() && true)
				{
					device_type = torch::kCUDA;
				}
				else
				{
					device_type = torch::kCPU;
				}
				// load network
				std::string weights = "./weights/VisDrone.torchscript";
				//std::string weights = "./weights/person.torchscript";
				//std::string weights = "./weights/yolov5s.torchscript";
				//m_yolov_ptr = new cyolov_torch(weights, "v5", "gpu");
				std::lock_guard<std::mutex> lock(g_video_analysis_lock);
				m_detector_ptr = new Detector(weights, device_type);
			}
		}
#endif 
#if defined(_MSC_VER)
#if 0
		else  if (m_video_analysis_type == EVideoAnalysisONNXRuntime)
		{
			m_onnxruntime_ptr = new cyolov_onnxruntime();
			/*if (!m_onnxruntime_ptr)
			{
				WARNING_EX_LOG("alloc onnxruntime failed !!!");
				return false;
			}*/
			m_onnxruntime_ptr->YOLODetector(true, cv::Size(640, 640));
			NORMAL_EX_LOG("[source = %s]onnxruntime  init OK !!!", m_source_path.c_str());
		}
#endif
#endif 

		//g_license_plate.init("D:/Work/cartificial_intelligence/HyperLPR/resource/models/r2_mobile");
		while (!m_stoped)
		{
			pre_time = std::chrono::steady_clock::now();//std::chrono::high_resolution_clock::now();
			//if (m_video_cap_ptr->grab() && m_video_cap_ptr->retrieve(img, m_video_index) /*d_reader->grab() && d_reader->nextFrame(d_frame)*/)
			{
				{
					std::lock_guard<std::mutex> lock(m_queue_lock);
					//m_queue_lock.push(img.clone());
					if (!m_queue.empty())
					{
						img = m_queue.front();
						m_queue.pop_front();
						data = true;
						//	img.release();
					}

				}

				//if (++skip_frame_count > m_skip_frame)
				if (data && !m_stoped)
				{
					//	skip_frame_count = 0;
#if 0
					if (one && m_detector_ptr)
					{
						//cv::cuda::GpuMat temp_img = cv::cuda::GpuMat(d_frame.rows, d_frame.cols, CV_32FC3);
						auto temp_img = cv::Mat::zeros(img.rows, img.cols, CV_32FC3);
						std::lock_guard<std::mutex> lock(g_video_analysis_lock);
						m_detector_ptr->Run(temp_img, 1.0f, 1.0f);

						one = false;
					}
#endif 
#if 0
					if (m_video_analysis_type == EVideoAnalysisTorchScript)
					{
						std::lock_guard<std::mutex> lock(g_video_analysis_lock);
						result = m_detector_ptr->Run(img, 0.25, 0.45);
						//std::vector<torch::Tensor> r = m_yolov_ptr->prediction(img);
						//if (r.size())
						//{
						//
						//	result = m_yolov_ptr->result(r[0]);
						//}

					}
#endif 
#if defined(_MSC_VER)
#if 0

					else if (m_video_analysis_type == EVideoAnalysisONNXRuntime)
					{
						onnxruntimeresult = m_onnxruntime_ptr->detect(img, 0.4, 0.45);
						result.push_back(onnxruntimeresult);
					}
#endif 
#endif 

				//	_send_video_info(img, result, class_names, false);
					//for (auto lll : result)
					//{
					//	lll.clear();
					//}
					//result.clear();
					//auto  end = std::chrono::steady_clock::now(); //std::chrono::high_resolution_clock::now();
					//auto  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - pre_time);
					//// It should be known that it takes longer time at first time
					////std::cout << "=======> post-process takes : " << duration.count() << " ms" << std::endl;
					//if (duration.count() > 25)
					//{
					//	NORMAL_EX_LOG("[source=%s] post-process takes : %u ms", m_source_path.c_str(), duration.count());
					//}

					//using namespace chen;
					//if (g_cfg.get_uint32(ECI_OpencvShow))
					//{
					//	//cv::resizeWindow(m_source_path, 800, 600);
					//	cv::imshow(m_source_path, img);
					//	//cv::imwrite("test.jpg", img);
					//	cv::waitKey(1);
					//}
					//img.release();

					//img.deallocate();

				}
				cur_time_ms = std::chrono::steady_clock::now();
				dur = cur_time_ms - pre_time;
				ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
				elapse = static_cast<uint32_t>(ms.count());
				if (elapse < 100 && !m_stoped)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(100 - elapse));
				}
				data = false;

			}

		}

		NORMAL_EX_LOG("_work_pthread  exit !!!");
		//cv::destroyAllWindows();
	}



	void cvideo_analysis::_work_decode_thread()
	{
		bool one = true;
		cv::Mat img;
		uint32 skip_frame_count = 0;

		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::milliseconds ms;
		uint32_t elapse = 0;
		uint32_t total_ms = 100 / 30; // g_cfg.get_uint32(ECI_AiVideoFrame);

		while (!m_stoped)
		{
			pre_time = std::chrono::steady_clock::now();//std::chrono::high_resolution_clock::now();
			if (m_video_index != -1 && m_video_cap_ptr->grab() && m_video_cap_ptr->retrieve(img, m_video_index) /*d_reader->grab() && d_reader->nextFrame(d_frame)*/)
			{
				if (  ++skip_frame_count > m_skip_frame)
				{
					skip_frame_count = 0;
					{
						
					/*	static uint32 count = 0;
						std::string image_name = std::to_string(++count) + ".jpg";
						FILE* out_ptr = fopen(image_name.c_str(), "wb+");
						
						if (out_ptr)
						{
							fwrite(image_data.c_str(), 1, image_data.size(), out_ptr);
							fflush(out_ptr);
							fclose(out_ptr);
							out_ptr = NULL;
						}
						*/
						time_t cur_time = ::time(NULL);
						//printf("send time = %u\n", cur_time);
						std::vector<  FaceRect> faces = facedetect_cnn(img.data, img.cols, img.rows, img.step);
						// 获取检测结果
						//int count = pResults ? *pResults : 0;
						//printf("Detected %d faces\n", count);

						// 绘制检测结果
						auto font_face = cv::FONT_HERSHEY_DUPLEX;
						auto font_scale = 1.0;
						int thickness = 1;
						for (int i = 0; i < faces.size(); ++i) {
							//short* p = ((short*)(pResults + 1)) + 142 * i;
							if (faces[i].score > 0.5)
							{
								//cv::Size s_size = cv::getTextSize(std::to_string(faces[i].score), font_face, font_scale, thickness, &baseline);
								cv::putText(img, std::to_string(faces[i].score), cv::Point(faces[i].x, faces[i].y - 5),
									font_face, font_scale, cv::Scalar(255, 255, 255), thickness);
								int x = faces[i].x;
								int y = faces[i].y;
								int w = faces[i].w;
								int h = faces[i].h;
								cv::rectangle(img, cv::Rect(x, y, w, h), cv::Scalar(0, 255, 0), 1);
							}
									
						}

						//http_face_util::face_http_post("http://192.168.2.20:8078", "/api/v1/recognition/recognize", image_data, result);
						//printf("[diff recv time = %u] \n",(::time(NULL)- cur_time) );
						if (false)
						{
							std::string result;
							cv::Mat imgtest;
							int quality = 50; //压缩比率0～100
							std::vector<uint8_t> imageData;
							std::vector<int> compress_params;
							compress_params.push_back(cv::IMWRITE_JPEG_QUALITY);
							compress_params.push_back(quality);
							cv::imencode(".jpg", img, imageData, compress_params);
							std::string  image_data = std::string(imageData.begin(), imageData.end());
							// pase json 
							Json::Reader reader;
							Json::Value data;

							if (!reader.parse((const char*)result.c_str(), (const char*)result.c_str() + result.size(), data))
							{
								WARNING_EX_LOG("parse [topic = %s][payload = %s] failed !!!", result.c_str(), result.c_str());
							}

							if (data.isMember("result") && data["result"].isArray())
							{
								//Json::Value resArray = data["result"];
								printf("resArray = %u\n", data["result"].size());
								;
								for (int w = 0; w < data["result"].size(); ++w)
								{
									printf("w ==== %u\n", w);
									//cv::rectangle(img, detection.bbox, cv::Scalar(0, 0, 255), 2);
									//					int conf = (int)std::round(detection.score * 100);
									//					std::string s = class_names[detection.class_idx] + " " + " 0." + std::to_string(conf);// +" color:" + std::to_string(car_color.index) + "  : " + plate_code;




									std::string s = "c";
									/*auto font_face = cv::FONT_HERSHEY_DUPLEX;
									auto font_scale = 1.0;*/
									//int thickness = 1;
									int baseline = 0;

									//cv::Size s_size = cv::getTextSize(s, font_face, font_scale, thickness, &baseline);
									if (data["result"][w]["box"]["x_min"].isUInt() &&
										data["result"][w]["box"]["y_min"].isUInt() &&
										data["result"][w]["box"]["x_max"].isUInt() &&
										data["result"][w]["box"]["y_max"].isUInt())
									{
										cv::Rect test_rect;
										test_rect.x = data["result"][w]["box"]["x_min"].asInt();
										test_rect.y = data["result"][w]["box"]["y_min"].asInt();
										test_rect.width = data["result"][w]["box"]["x_max"].asInt() - test_rect.x;
										test_rect.height = data["result"][w]["box"]["y_max"].asInt() - test_rect.y;
										printf("w = %u, x = %u, y = %u \n", w, test_rect.x, test_rect.y);
										cv::rectangle(img,
											test_rect,
											cv::Scalar(0, 0, 255), 1);
									}
									/*cv::putText(img, s, cv::Point(data["result"][w]["box"]["x_min"].asUInt()
										, data["result"][w]["box"]["x_min"].asUInt() - 5),
										font_face, font_scale, cv::Scalar(255, 255, 255), thickness);*/
								}
						}
						
						}															 
						 
						//cv::imshow(m_source_path, img);
						//	//cv::imwrite("test.jpg", img);
						//	cv::waitKey(1);
						//std::lock_guard<std::mutex> lock(m_queue_lock);
						//m_queue.push_back(img.clone());

					}
				}

			}
			else
			{
				//if (!m_video_cap_ptr->isOpened())
				{
					 WARNING_EX_LOG("[source = %s] close !!!  wait %u  s", m_source_path.c_str(), g_cfg.get_uint32(ECI_MeidaReconnectWait));
				}
				m_action = 2;
				if (!m_stoped)
				{
					if (m_video_cap_ptr)
					{
						m_video_cap_ptr->release();
						delete m_video_cap_ptr;
						m_video_cap_ptr = NULL;
					}
					m_video_index = -1;
					std::this_thread::sleep_for(std::chrono::seconds(g_cfg.get_uint32(ECI_MeidaReconnectWait)));
					if (_open())
					{
						m_action = 1;
					}
				}

				continue;
				// TODO@chensong 20231017 

			}
			cur_time_ms = std::chrono::steady_clock::now();
			dur = cur_time_ms - pre_time;
			ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
			elapse = static_cast<uint32_t>(ms.count());
			printf("elapse = %u\n", elapse);
			 cv::imshow(m_source_path, img);
						//	//cv::imwrite("test.jpg", img);
						 	cv::waitKey(1);
						//std::lock_guard<std::mutex> lock(m_queue_lock);
			if (elapse < total_ms && !m_stoped)
			{
				//std::this_thread::sleep_for(std::chrono::milliseconds(total_ms - elapse));
			}
		}
		m_action = 0;
		NORMAL_EX_LOG("_work_decode_thread  exit !!!");
	}
	//void cvideo_analysis::_send_video_info(cv::Mat& img,
	//	const std::vector<std::vector<CDetection>>& detections,
	//	const std::vector<std::string>& class_names,
	//	bool label)
	//{

	//	if (!detections.empty())
	//	{
	//		Json::Value data;
	//		/*Json::Value pedestrian_obj;;*/
	//		Json::Value people_Obj;
	//		Json::Value Car_Obj;
	//		/*Json::Value car_obj;
	//		Json::Value var_obj;
	//		Json::Value truck_obj;
	//		Json::Value bus_obj;
	//		Json::Value vehicle_info;*/

	//		//////////////






	//		Json::Value item;
	//		for (const std::vector<CDetection>& pvec : detections)
	//		{
	//			for (const CDetection& detection : pvec)
	//			{
	//				if ((detection.bbox.x < 0 || detection.bbox.y < 0) || (detection.bbox.x + detection.bbox.width > img.size().width || detection.bbox.y + detection.bbox.height > img.size().height))
	//				{
	//					continue;
	//				}
	//				item["x"] = detection.bbox.x;
	//				item["y"] = detection.bbox.y;
	//				item["width"] = detection.bbox.width;
	//				item["height"] = detection.bbox.height;
	//				item["score"] = detection.score;
	//				//# #  3：汽车
	//				//# #  4:面包车
	//				//# #  5:卡车
	//				//# #  6:三轮车
	//				//# #  7：遮阳篷三轮车
	//				//# #  8：公交车

	//				if (g_cfg.get_uint32(ECI_OpencvShow))
	//				{
	//					cv::rectangle(img, detection.bbox, cv::Scalar(0, 0, 255), 2);
	//					int conf = (int)std::round(detection.score * 100);
	//					std::string s = class_names[detection.class_idx] + " " + " 0." + std::to_string(conf);// +" color:" + std::to_string(car_color.index) + "  : " + plate_code;





	//					auto font_face = cv::FONT_HERSHEY_DUPLEX;
	//					auto font_scale = 1.0;
	//					int thickness = 1;
	//					int baseline = 0;
	//					cv::Size s_size = cv::getTextSize(s, font_face, font_scale, thickness, &baseline);
	//					cv::rectangle(img,
	//						cv::Point(detection.bbox.tl().x, detection.bbox.tl().y - s_size.height - 5),
	//						cv::Point(detection.bbox.tl().x + s_size.width, detection.bbox.tl().y),
	//						cv::Scalar(0, 0, 255), -1);
	//					cv::putText(img, s, cv::Point(detection.bbox.tl().x, detection.bbox.tl().y - 5),
	//						font_face, font_scale, cv::Scalar(255, 255, 255), thickness);
	//				}

	//				if (detection.class_idx == 1 || detection.class_idx == 0)
	//				{
	//					people_Obj.append(item);

	//				}
	//				else if (detection.class_idx == 3 || detection.class_idx == 4 || detection.class_idx == 5 || detection.class_idx == 8)
	//				{
	//					Car_Obj.append(item);
	//				}


	//			}
	//		}







	//		if (!people_Obj.empty())
	//		{
	//			/*Json::Value class_object;
	//			class_object["class"] = 0;
	//			class_object["class_data"] = people_Obj;
	//			data.append(class_object);*/
	//			data["class_0"] = people_Obj;
	//		}
	//		if (!Car_Obj.empty())
	//		{
	//			/*Json::Value class_object;
	//			class_object["class"] = 1;
	//			class_object["class_data"] = Car_Obj;
	//			data.append(class_object);*/
	//			data["class_1"] = Car_Obj;
	//		}
	//		if (!data.empty())
	//		{
	//			s_mqtt_client_mgr.publish(m_result_video_analysis, data.toStyledString());
	//		}
	//		//NORMAL_EX_LOG("json = %s\n", data.toStyledString().c_str());
	//	}
	//}

	bool cvideo_analysis::_open()
	{
		if (m_stoped)
		{
			return false;
		}
		m_video_cap_ptr = new cv::VideoCapture();
		//m_video_cap_ptr->set();
		m_video_cap_ptr->open(m_source_path, cv::CAP_FFMPEG);
		if (!m_video_cap_ptr->isOpened())
		{
			std::cerr << "ERROR! Can't to open file: " << m_source_path << std::endl;
			WARNING_EX_LOG("Can't ot open [source = %s]", m_source_path.c_str());
			return false;
		}
		//m_source_path = source;
		//const int videoBaseIndex = (int)m_video_cap_ptr->get(cv::CAP_PROP_VIDEO_STREAM);
		m_video_index = (int)m_video_cap_ptr->get(cv::CAP_PROP_VIDEO_TOTAL_CHANNELS);

		NORMAL_EX_LOG("open [source = %s] OK ", m_source_path.c_str());
		return true;
	}


}
