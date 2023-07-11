/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef _C_YOLOV_ONNXRUNTIME_H
#define _C_YOLOV_ONNXRUNTIME_H

// Cpp native
#include <fstream>
#include <vector>
#include <string>
#include <random>

// OpenCV / DNN / Inference
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/opencv.hpp>
#include<onnxruntime_cxx_api.h>
#include<utility>
#include<iostream>
#include "detector.h"
#include<codecvt>
#include<fstream>
namespace chen {
	class Utils
	{
	public:
		Utils();
		size_t vectorProduct(const std::vector<int64_t>& vector);
		std::wstring charToWstring(const char* str);
		static std::vector<std::string> loadNames(const std::string&path);//加载类别名称
		void visualizeDetection(cv::Mat& image, const std::vector<CDetection>& detections,
			const std::vector<std::string>& classNames);
		void letterbox(const cv::Mat &image, cv::Mat& outImage, const cv::Size &newShape, const cv::Scalar &color,
			bool auto_, bool scaleFill, bool scaleUp, int stride);//信封图片预处理
		void scaleCoords(const cv::Size& imageShape, cv::Rect& box, const cv::Size& imageOriginalShape);
		//模板函数
		template <typename T>
		T clip(const T& n, const T& lower, const T& upper);
	};
	class cyolov_onnxruntime
	{
	public:
		cyolov_onnxruntime();
		void YOLODetector(const bool& isGPU, const cv::Size& inputSize);
		void getBestClassInfo(std::vector<float>::iterator it, const int& numClasses,
			float& bestConf, int& bestClassId);
		void preprocession(cv::Mat &image, float*& blob, std::vector<int64_t>& inputTensorShape);
		std::vector<CDetection> postprocessing(const cv::Size& resizedImageShape,
			const cv::Size& originalImageShape,
			std::vector<Ort::Value>& outputTensors,
			const float& confThreshold, const float& iouThreshold);
		std::vector<CDetection> detect(cv::Mat &image, const float& confThreshold, const float& iouThreshold);

	private:
		std::string modelPath = "D:/Work/cartificial_intelligence/yolov5/data/images/VisDrone.onnx";
		Ort::Env env{ nullptr };//构建Ort的环境，这是一个线性的固定写法
		Ort::SessionOptions sessionOptions{ nullptr };//session选项
		Ort::Session session{ nullptr };//Ort session管理器
		std::vector<const char*> inputNames;
		std::vector<const char*> outputNames;
		bool isDynamicInputShape;
		cv::Size2f inputImageShape;
		Utils utils;
	};
}

#endif // _C_YOLOV_ONNXRUNTIME_H
