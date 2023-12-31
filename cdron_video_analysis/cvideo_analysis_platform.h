﻿/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef C_VIDEO_ANALYSIS_PLATFORM_H
#define C_VIDEO_ANALYSIS_PLATFORM_H
#include <thread>
#include <string>
#include <functional>
#if defined(_MSC_VER)
#include <Windows.h>
#endif

#include "opencv2/opencv.hpp"
#include <memory>

#include <torch/script.h>
#include <torch/torch.h>

#include <c10/cuda/CUDAStream.h>
#include <ATen/cuda/CUDAEvent.h>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudacodec.hpp>
#include "utils.h"
#include <hv/mqtt_client.h>
#include "detector.h"
#include "csingleton.h"
#include "cnet_type.h"
namespace chen
{
	enum EVideoAnalysisPlatformType
	{
		EVideoAnalysisTorchScript = 0,
		EVideoAnalysisONNXRuntime,
		EVideoAnalysisOpenCVDNN,
		EVideoAnalysisOpenVINO,
		EVideoAnalysisTensorRT,
		EVideoAnalysisCoreML,
		EVideoAnalysisTensorFlowSavedModel,
		EVideoAnalysisTensorFlowGraphDef,
		EVideoAnalysisTensorFlowLite,
		EVideoAnalysisTensorFlowEdgeTPU,
		EVideoAnalysisPaddlePaddle,
	};
}
#endif // C_VIDEO_ANALYSIS_PLATFORM_H