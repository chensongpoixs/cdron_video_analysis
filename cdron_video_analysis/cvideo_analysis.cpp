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
		//m_license_plate.init("resource/models/r2_mobile");
		//m_car_color_ptr = new ctorch_classify("weights/car_color.torchscript");
		//m_car_type_ptr = new ctorch_classify("weights/car_type.torchscript");
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

		if (m_video_analysis_type != EVideoAnalysisONNXRuntime&& m_video_analysis_type != EVideoAnalysisTorchScript)
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
		m_source_path = source;
		//const int videoBaseIndex = (int)m_video_cap_ptr->get(cv::CAP_PROP_VIDEO_STREAM);
		m_video_index = (int)m_video_cap_ptr->get(cv::CAP_PROP_VIDEO_TOTAL_CHANNELS);
		m_stoped = false;
		m_decode_thread = std::thread(&cvideo_analysis::_work_decode_thread, this);
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
		NORMAL_EX_LOG("[source = %s] exit !!!", m_source_path.c_str());
		m_stoped = true;
		if (m_thread.joinable())
		{
			m_thread.join();
			
		}
		NORMAL_EX_LOG("[source = %s] exit !!!", m_source_path.c_str());
		if (m_decode_thread.joinable())
		{
			m_decode_thread.join();
		}
		m_queue.clear();
		NORMAL_EX_LOG("[source = %s] thread exit OK !!!", m_source_path.c_str());
		if (m_detector_ptr)
		{
			delete m_detector_ptr;
			m_detector_ptr = NULL;
			NORMAL_EX_LOG("[source = %s]   totch delete !!!", m_source_path.c_str());
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
		m_result_video_analysis =  "video_analysis/" + result_video_analysis;
	}

	void cvideo_analysis::_work_pthread()
	{
		
		std::chrono::steady_clock::time_point cur_time_ms;
		std::chrono::steady_clock::time_point pre_time = std::chrono::steady_clock::now();
		std::chrono::steady_clock::duration dur;
		std::chrono::milliseconds ms;
		uint32_t elapse = 0;
		uint32_t total_ms = 100 / 30;
		std::vector<std::vector<CDetection>> result;
		std::vector<CDetection> onnxruntimeresult;
		cv::Mat img;
		bool one = true;
		bool data = false;
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
					}
					else
					{
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
				 
				;
				
				//if (++skip_frame_count > m_skip_frame)
				if (data && !m_stoped)
				{
				//	skip_frame_count = 0;
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
						onnxruntimeresult = m_onnxruntime_ptr->detect(img, 0.4, 0.45);
						result.push_back(onnxruntimeresult);
					}
					
					_send_video_info(img, result, class_names, false);
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
				}
				
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
		uint32_t total_ms = 100 / 30;
		
		while (!m_stoped)
		{
			 pre_time = std::chrono::high_resolution_clock::now();
			if (m_video_cap_ptr->grab() && m_video_cap_ptr->retrieve(img, m_video_index) /*d_reader->grab() && d_reader->nextFrame(d_frame)*/)
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
			cur_time_ms = std::chrono::steady_clock::now();
			dur = cur_time_ms - pre_time;
			ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur);
			elapse = static_cast<uint32_t>(ms.count());
			if (elapse < total_ms && !m_stoped)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(total_ms - elapse));
			}
		}
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
#if 0
					std::string plate_code;
					cls_socre car_color  ;
					cls_socre car_type  ;
					Json::Value vehicle_info;
					if (m_car_analysis> 0&&(detection.class_idx == 3 || detection.class_idx == 4 || detection.class_idx == 5 || detection.class_idx == 8))
					{
						 
							//detection.show();
							cv::Mat plate_img = img(detection.bbox);
							//cv::imshow("plate_img", plate_img);
							//cv::imwrite("test.jpg", img);
							//cv::waitKey(1);
							//car_color = _recognize_vehicle_color(plate_img);
							m_license_plate.recognition(plate_img, plate_code);
							vehicle_info["license_plate"] = plate_code;

							if (m_car_color_ptr)
							{
								car_color = m_car_color_ptr->classitfy(plate_img);
								vehicle_info["car_color"] = car_color.index;
							}
							if (m_car_type_ptr)
							{
								car_type = m_car_color_ptr->classitfy(plate_img);
								vehicle_info["car_type"] = car_type.index;
							}

							item["vehicle_info"] = vehicle_info;
						//cvReleaseImage

					}
#endif 
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
				Json::Value class_object;
				class_object["class"] = 0;
				class_object["class_data"] = people_Obj;
				data.append(class_object);
			}
			if (!Car_Obj.empty())
			{
				Json::Value class_object;
				class_object["class"] = 1;
				class_object["class_data"] = Car_Obj;
				data.append(class_object);
			}
			if (!data.empty())
			{
				s_mqtt_client_mgr.publish( m_result_video_analysis, data.toStyledString());
			}
			//NORMAL_EX_LOG("json = %s\n", data.toStyledString().c_str());
		} 
	}
	/*std::string cvideo_analysis::_recognize_vehicle_color()
	{
		return std::string();
	}*/
	/* 输入（cv::Mat）： 车辆颜色图片;
   输出（std::string）： 车辆名*/
	std::string cvideo_analysis::_recognize_vehicle_color(const cv::Mat& img)
	{
		cv::Size imgSize = img.size();
		cv::Mat preImg = img;// img(cv::Rect(imgSize.width*0.15, imgSize.height*0.15, imgSize.width*(1 - 0.3), imgSize.height*(1 - 0.3)));
		// HSV 三通道操作
		/*cv::imshow("img", img);
		cv::imshow("preImg", preImg);
		
		cv::waitKey(-1);*/
		cv::Mat hsvImg;
		cv::cvtColor(preImg, hsvImg, cv::COLOR_BGR2HSV);//COLOR_BGR2RGB
		/*cv::imshow("COLOR_BGR2HSV", hsvImg);

		cv::waitKey(-1);*/
		std::vector<cv::Mat> hsv_planes;
		cv::split(hsvImg, hsv_planes);    // 分离HSV通道
		cv::Mat hMask, sMask, vMask;

		cv::threshold(hsv_planes[1], sMask, 43, 255, cv::THRESH_BINARY);	// 以43为阈值将S通道进行二值化  43来自 HSV基本颜色分量范围
		//cv::multiply(sMask, bgrMask / 255, hMask);						// 与背景 Mask 合成为一个 Mask

		cv::threshold(hsv_planes[2], vMask, 46, 255, cv::THRESH_BINARY);	// 以46为阈值将V通道进行二值化  46来自 HSV基本颜色分量范围
		cv::multiply(vMask, sMask / 255, hMask);						// 与背景 Mask 合成为一个 Mask 用于H通道

		cv::threshold(hsv_planes[1], vMask, 43, 255, cv::THRESH_BINARY_INV);// 以43为阈值将S通道进行二值化 用于V通道

		// 利用二值图 Mask 将 HSV 通道中的背景部分设为 0.
		hsv_planes[1] = hsv_planes[2].clone();
		cv::multiply(hsv_planes[0], hMask / 255, hsv_planes[0]);
		cv::multiply(hsv_planes[2], vMask / 255, hsv_planes[2]);

		// 分别计算三个通道的颜色直方图
		int histSize = 180;         // 将色调量化到180个级别 对应 H 取值范围 [0:180]
		float ranges[] = { 0, 180 };
		const float* hRanges = { ranges };
		cv::Mat hsv_hist, sv_hist, v_hist;
		//计算直方图
		calcHist(&hsv_planes[0], 1, 0, cv::Mat(), hsv_hist, 1, &histSize, &hRanges, true, false);
		histSize = 256;
		ranges[1] = 256;
		const float* Ranges = { ranges };
		calcHist(&hsv_planes[2], 1, 0, cv::Mat(), sv_hist, 1, &histSize, &Ranges, true, false);
		calcHist(&hsv_planes[1], 1, 0, cv::Mat(), v_hist, 1, &histSize, &Ranges, true, false);
		std::map<std::string, float> color;	// 定义颜色映射， 对每个颜色对应的HSV区间内进行统计
		color["red"] = 0;
		color["orange"] = 0; //橙色
		color["yellow"] = 0; //黄的
		color["green"] = 0; //绿色
		color["cyan"] = 0; //青色
		color["blue"] = 0; //蓝色
		color["violet"] = 0; //紫罗兰
		color["black"] = 0; //黑色
		color["grey"] = 0; //灰色
		color["white"] = 0;
		for (int r = 0; r < hsv_hist.rows; r++) {
			float binVal = hsv_hist.at<float>(r);
			if ((r < 10 && r > 0) || (r > 160))  // 红色区间 H [0,10]
			{
				color["red"] += binVal;
			}
			else if (r > 11 && r < 25)  // 橙色区间 H [11,25]
			{
				color["orange"] += binVal;
			}
			else if (r > 26 && r < 34)  // 黄色区间 H [26,34]
			{
				color["yellow"] += binVal;
			}
			else if (r > 35 && r < 77)  // 绿色区间 H [35,77]
			{
				color["green"] += binVal;
			}
			else if (r > 78 && r < 99)  // 青色区间 H [78,99]
			{
				color["cyan"] += binVal;
			}
			else if (r > 100 && r < 124)  // 蓝色区间 H [100,124]
			{
				color["blue"] += binVal;
			}
			else if (r > 125 && r < 159)  // 紫色区间 H [125,155]
			{
				color["violet"] += binVal;
			}
		}
		for (int r = 1; r < sv_hist.rows; r++) {
			float sv_binVal = sv_hist.at<float>(r);
			if (r > 200)
			{
				color["white"] += sv_binVal;	// 白色区间 H [200,255]
			}
		}
		for (int r = 1; r < v_hist.rows; r++) {
			float v_binVal = v_hist.at<float>(r);
			if (r > 0 && r <= 46)
			{
				color["black"] += v_binVal;		// 黑色区间 H [0,40]
			}
			else if (r > 46 && r < 211)
			{
				color["grey"] += v_binVal;		// 黑色区间 H [0,40]
			}
		}
		float maxVal = 0;
		std::string result = "";
		std::map<std::string, float> ::const_iterator it = color.begin();  //找出对应颜色区间统计数最多的颜色
		for (it; it != color.end(); it++)
		{
			if (it->second > maxVal)
			{
				maxVal = it->second;
				result = it->first;
			}
		}
		return result;
	}


}