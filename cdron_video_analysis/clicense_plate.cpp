/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/

#include "clicense_plate.h"
#include <iostream>
#include "c_api/hyper_lpr_sdk.h"
namespace chen {
	static const std::vector<std::string> TYPES = { "蓝牌", "黄牌单层", "白牌单层", "绿牌新能源", "黑牌港澳", "香港单层", "香港双层", "澳门单层", "澳门双层", "黄牌双层" };
	clicense_plate g_license_plate;;
	clicense_plate::clicense_plate()
		: m_models_path("")
	{
	}
	clicense_plate::~clicense_plate()
	{
	}
	bool clicense_plate::init(const char * models_path)
	{
		m_models_path = models_path;
		return true;
	}
	bool clicense_plate::recognition(cv::Mat image)
	{

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
		configuration.models_path = (  char *)(m_models_path.c_str());
		configuration.max_num = 5;
		configuration.det_level = DETECT_LEVEL_LOW;
		configuration.use_half = false;
		configuration.nms_threshold = 0.5f;
		configuration.rec_confidence_threshold = 0.5f;
		configuration.box_conf_threshold = 0.30f;
		configuration.threads = 1;
		P_HLPR_Context ctx = HLPR_CreateContext(&configuration);
		HREESULT ret = HLPR_ContextQueryStatus(ctx);
		if (ret != HResultCode::Ok) 
		{
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

			/*cv::rectangle(image, cv::Point2f(results.plates[i].x1, results.plates[i].y1), cv::Point2f(results.plates[i].x2, results.plates[i].y2),
				cv::Scalar(100, 100, 200), 3);*/

			printf("<%d> %s, %s, %f\n", i + 1, type.c_str(),
				results.plates[i].code, results.plates[i].text_confidence);
		}

		//    cv::imwrite("out.jpg", image);
		/*cv::imshow("out", image);
		cv::waitKey(0);*/

		// release buffer
		HLPR_ReleaseDataBuffer(buffer);
		// release context
		HLPR_ReleaseContext(ctx);
		return false;
	}
}