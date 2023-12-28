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
#include <mutex>
namespace chen {
	static std::mutex					g_video_analysis_lock;
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
		m_thread = std::thread(&cvideo_analysis::_work_analysis_pthread, this);

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
		NORMAL_EX_LOG("[source = %s] thread exit OK !!!", m_source_path.c_str());
		if (m_onnxruntime_ptr)
		{
			delete m_onnxruntime_ptr;
			m_onnxruntime_ptr = NULL;
			NORMAL_EX_LOG("[source = %s] onnxruntime delete !!!", m_source_path.c_str());
		}
		NORMAL_EX_LOG("[source = %s] thread exit OK !!!", m_source_path.c_str());
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
		uint32_t total_ms = 100 / g_cfg.get_uint32(ECI_AiVideoFrame);
		std::vector<std::vector<CDetection>> result;
		std::vector<CDetection> onnxruntimeresult;
		cv::Mat img;
		bool one = true;
		bool data = false;


#if 1
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
			std::lock_guard<std::mutex> lock(g_video_analysis_lock);
			m_detector_ptr = new Detector(weights, device_type);
		}
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

		//g_license_plate.init("D:/Work/cartificial_intelligence/HyperLPR/resource/models/r2_mobile");
		while (!m_stoped)
		{
			pre_time = std::chrono::high_resolution_clock::now();
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
#if 1
					if (one && m_detector_ptr)
					{
						//cv::cuda::GpuMat temp_img = cv::cuda::GpuMat(d_frame.rows, d_frame.cols, CV_32FC3);
						auto temp_img = cv::Mat::zeros(img.rows, img.cols, CV_32FC3);
						std::lock_guard<std::mutex> lock(g_video_analysis_lock);
						m_detector_ptr->Run(temp_img, 1.0f, 1.0f);

						one = false;
					}
#endif 
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
					else if (m_video_analysis_type == EVideoAnalysisONNXRuntime)
					{
						onnxruntimeresult = m_onnxruntime_ptr->detect(img, 0.4, 0.45);
						result.push_back(onnxruntimeresult);
					}

					_send_video_info(img, result, class_names, false);
					for (auto lll : result)
					{
						lll.clear();
					}
					result.clear();
					auto  end = std::chrono::high_resolution_clock::now();
					auto  duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - pre_time);
					// It should be known that it takes longer time at first time
					//std::cout << "=======> post-process takes : " << duration.count() << " ms" << std::endl;
					if (duration.count() > 25)
					{
						NORMAL_EX_LOG("[source=%s] post-process takes : %u ms", m_source_path.c_str(), duration.count());
					}

					using namespace chen;
					if (g_cfg.get_uint32(ECI_OpencvShow))
					{
						//cv::resizeWindow(m_source_path, 800, 600);
						cv::imshow(m_source_path, img);
						//cv::imwrite("test.jpg", img);
						cv::waitKey(1);
					}
					img.release();

					img.deallocate();

				}
				cur_time_ms = std::chrono::steady_clock::now();
				dur = cur_time_ms - pre_time;
				ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
				elapse = static_cast<uint32_t>(ms.count());
				if (elapse < total_ms && !m_stoped)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(total_ms - elapse));
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
		uint32_t total_ms = 100 / g_cfg.get_uint32(ECI_AiVideoFrame);

		while (!m_stoped)
		{
			pre_time = std::chrono::high_resolution_clock::now();
			if (m_video_index != -1 && m_video_cap_ptr->grab() && m_video_cap_ptr->retrieve(img, m_video_index) /*d_reader->grab() && d_reader->nextFrame(d_frame)*/)
			{
				if (++skip_frame_count > m_skip_frame)
				{
					skip_frame_count = 0;
					{
						std::lock_guard<std::mutex> lock(m_queue_lock);
						m_queue.push_back(img.clone());

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
			if (elapse < total_ms && !m_stoped)
			{
				//std::this_thread::sleep_for(std::chrono::milliseconds(total_ms - elapse));
			}
		}
		m_action = 0;
		NORMAL_EX_LOG("_work_decode_thread  exit !!!");
	}
	void cvideo_analysis::_send_video_info(cv::Mat& img,
		const std::vector<std::vector<CDetection>>& detections,
		const std::vector<std::string>& class_names,
		bool label)
	{

		if (!detections.empty())
		{
			Json::Value data;
			/*Json::Value pedestrian_obj;;*/
			Json::Value people_Obj;
			Json::Value Car_Obj;
			/*Json::Value car_obj;
			Json::Value var_obj;
			Json::Value truck_obj;
			Json::Value bus_obj;
			Json::Value vehicle_info;*/

			//////////////






			Json::Value item;
			for (const std::vector<CDetection>& pvec : detections)
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

					if (g_cfg.get_uint32(ECI_OpencvShow))
					{
						cv::rectangle(img, detection.bbox, cv::Scalar(0, 0, 255), 2);
						int conf = (int)std::round(detection.score * 100);
						std::string s = class_names[detection.class_idx] + " " + " 0." + std::to_string(conf);// +" color:" + std::to_string(car_color.index) + "  : " + plate_code;





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

					if (detection.class_idx == 1 || detection.class_idx == 0)
					{
						people_Obj.append(item);

					}
					else if (detection.class_idx == 3 || detection.class_idx == 4 || detection.class_idx == 5 || detection.class_idx == 8)
					{
						Car_Obj.append(item);
					}


				}
			}







			if (!people_Obj.empty())
			{
				/*Json::Value class_object;
				class_object["class"] = 0;
				class_object["class_data"] = people_Obj;
				data.append(class_object);*/
				data["class_0"] = people_Obj;
			}
			if (!Car_Obj.empty())
			{
				/*Json::Value class_object;
				class_object["class"] = 1;
				class_object["class_data"] = Car_Obj;
				data.append(class_object);*/
				data["class_1"] = Car_Obj;
			}
			if (!data.empty())
			{
				s_mqtt_client_mgr.publish(m_result_video_analysis, data.toStyledString());
			}
			//NORMAL_EX_LOG("json = %s\n", data.toStyledString().c_str());
		}
	}

	bool cvideo_analysis::_open()
	{
		if (m_stoped)
		{
			return false;
		}
		m_video_cap_ptr = new cv::VideoCapture();

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