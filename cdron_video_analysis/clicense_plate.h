/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef C_LICENSE_PLATE_H
#define C_LICENSE_PLATE_H
#include <thread>
#include <string>
#include <functional>
#if defined(_MSC_VER)
#include <Windows.h>
#include "c_api/hyper_lpr_sdk.h"
#include "opencv2/opencv.hpp"
namespace chen
{

	class clicense_plate
	{
	public:
		explicit clicense_plate();
		virtual ~clicense_plate();

	public:

		bool init(const char * models_path);
		void destroy();
	public:


		bool recognition(cv::Mat image, std::string & plate_codec);
		
	private:
		std::string m_models_path;

		P_HLPR_Context m_ctx_ptr;
	};


	extern clicense_plate g_license_plate;

}
#endif


#endif // C_LICENSE_PLATE_H

