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
#endif

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


		bool recognition(cv::Mat image);
		
	private:
		std::string m_models_path;
	};


	extern clicense_plate g_license_plate;

}


#endif // C_LICENSE_PLATE_H

