

#include "cyolov_onnxruntime.h"
#include "clog.h"
#include <opencv2/opencv.hpp>
namespace chen {


	Utils::Utils()
	{

	}
	/**
	 * @brief Utils::vectorProduct 生成vector
	 * @param vector
	 * @return
	 */
	size_t Utils::vectorProduct(const std::vector<int64_t> &vector)
	{
		if (vector.empty())
			return 0;
		size_t product = 1;
		for (const auto& element : vector)
			product *= element;
		return product;
	}
	/**
	 * @brief Utils::charToWstring 将char转换为wstring格式
	 * @param str
	 * @return
	 */
	std::wstring Utils::charToWstring(const char *str)
	{
		typedef std::codecvt_utf8<wchar_t> convert_type;
		 std::wstring_convert<convert_type, wchar_t> converter;
		return converter.from_bytes(str);
	}
	/**
	 * @brief Utils::loadNames 加载标签文件
	 * @param path 标签路径
	 * @return
	 */
	std::vector<std::string> Utils::loadNames(const std::string &path)
	{
		std::vector<std::string> classNames;
		std::ifstream infile(path);//读取文件
		if (infile.good()) {
			std::string line;
			while (std::getline(infile, line)) {
				if (line.back() == '\r')
					line.pop_back();
				classNames.emplace_back(line);
			}
			infile.close();
		}
		else {
			std::cerr << "ERROR:Failed to access class name path: " << path << std::endl;
		}
		return classNames;
	}
	/**
	 * @brief Utils::visualizeDetection 识别结果可视化
	 * @param image
	 * @param detections
	 * @param classNames
	 */
	void Utils::visualizeDetection(cv::Mat &image, const std::vector<CDetection> &detections, const std::vector<std::string> &classNames)
	{
		for (const CDetection& detection : detections)
		{
			cv::rectangle(image, detection.bbox, cv::Scalar(229, 160, 21), 2);

			int x = detection.bbox.x;
			int y = detection.bbox.y;

			int conf = (int)std::round(detection.score * 100);
			int classId = detection.class_idx;
			std::string label = classNames[classId] + " 0." + std::to_string(conf);

			int baseline = 0;
			cv::Size size = cv::getTextSize(label, cv::FONT_ITALIC, 0.8, 2, &baseline);
			cv::rectangle(image,
				cv::Point(x, y - 25), cv::Point(x + size.width, y),
				cv::Scalar(229, 160, 21), -1);

			cv::putText(image, label,
				cv::Point(x, y - 3), cv::FONT_ITALIC,
				0.8, cv::Scalar(255, 255, 255), 2);
		}
	}
	/**
	 * @brief Utils::letterbox 信封的图片缩放与填充
	 * @param image
	 * @param outImage
	 * @param newShape
	 * @param color
	 * @param auto_
	 * @param scaleFill
	 * @param scaleUp
	 * @param stride
	 */
	void Utils::letterbox(const cv::Mat &image, cv::Mat &outImage, const cv::Size &newShape, const cv::Scalar &color = cv::Scalar(114, 114, 114), bool auto_ = true, bool scaleFill = false, bool scaleUp = true, int stride = 32)
	{
		cv::Size shape = image.size();
		float r = std::min((float)newShape.height / (float)shape.height, (float)newShape.width / (float)shape.width);
		if (!scaleUp)
			r = std::min(r, 1.0f);
		float ratio[2]{ r,r };
		int newUnpad[2]{ (int)round((float)shape.width*r),
					   (int)round((float)shape.height*r) };
		auto dw = (float)(newShape.width - newUnpad[0]);
		auto dh = (float)(newShape.height - newUnpad[1]);
		if (auto_) {
			dw = (float)((int)dw%stride);
			dh = (float)((int)dh%stride);
		}
		else if (scaleFill) {
			dw = 0.0f;
			dh = 0.0f;
			newUnpad[0] = newShape.width;
			newUnpad[1] = newShape.height;
			ratio[0] = (float)newShape.width / (float)shape.width;
			ratio[1] = (float)newShape.height / (float)shape.height;
		}
		dw /= 2.0f;
		dh /= 2.0f;
		if (shape.width != newUnpad[0] && shape.height != newUnpad[1])
		{
			cv::resize(image, outImage, cv::Size(newUnpad[0], newUnpad[1]));
		}

		int top = int(std::round(dh - 0.1f));
		int bottom = int(std::round(dh + 0.1f));
		int left = int(std::round(dw - 0.1f));
		int right = int(std::round(dw + 0.1f));
		cv::copyMakeBorder(outImage, outImage, top, bottom, left, right, cv::BORDER_CONSTANT, color);
	}
	/**
	 * @brief Utils::scaleCoords 坐标还原
	 * @param imageShape
	 * @param box
	 * @param imageOriginalShape
	 */
	void Utils::scaleCoords(const cv::Size &imageShape, cv::Rect &box, const cv::Size &imageOriginalShape)
	{
		float gain = std::min((float)imageShape.height / (float)imageOriginalShape.height,
			(float)imageShape.width / (float)imageOriginalShape.width);
		int pad[2] = { (int)(((float)imageShape.width - (float)imageOriginalShape.width*gain) / 2.0f),
				   (int)(((float)imageShape.height - (float)imageOriginalShape.height*gain) / 2.0f) };
		box.x = (int)round(((float)(box.x - pad[0]) / gain));
		box.y = (int)round(((float)(box.y - pad[1]) / gain));
		box.width = (int)round(((float)box.width / gain));
		box.height = (int)round(((float)box.height / gain));
	}
	/////////////////////////////////////////////////////////

	cyolov_onnxruntime::cyolov_onnxruntime()
	{

	}
	/**
	 * @brief Detector::YOLODetector yolo检测进行onnxruntime的初始化
	 * @param isGPU
	 * @param inputSize
	 */
	void cyolov_onnxruntime::YOLODetector(const bool &isGPU = true, const cv::Size &inputSize = cv::Size(640, 640))
	{
		env = Ort::Env(OrtLoggingLevel::ORT_LOGGING_LEVEL_WARNING, "yolov5_ONNXRUNTIME");//创建yolov5运行环境
		sessionOptions = Ort::SessionOptions();//sessionOptions选择项
		sessionOptions.SetIntraOpNumThreads(1);
		sessionOptions.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
		OrtCUDAProviderOptions cudaOption;
		   // sessionOptions.AppendExecutionProvider_CUDA(cudaOption);
		   // sessionOptions.AppendExecutionProvider_CUDA(cuda
		    std::vector<std::string> availableProviders=Ort::GetAvailableProviders();//获取Providers这是Ort的一般做法
		    auto cudaAvailable=find(availableProviders.begin(),availableProviders.end(),"CUDAExecutionProvider");//遍历provides
		   // OrtCUDAProviderOptions cudaOption;
		    //判断GPU是否可用
		    if(isGPU&&(cudaAvailable==availableProviders.end())){
				//std::cout<<"GPU is not supported by your ONNXRuntime build.Fallback to CPU."<<std::endl;
				WARNING_EX_LOG("GPU is not supported by your ONNXRuntime build.Fallback to CPU.");
				//std::cout<<"Inference device:CPU"<< std::endl;
				WARNING_EX_LOG("Inference device:CPU");
		    }
		    else if(isGPU&&(cudaAvailable!=availableProviders.end())){
				//std::cout<<"Inference Device :GPU"<< std::endl;
				NORMAL_EX_LOG("Inference Device :GPU");
		        sessionOptions.AppendExecutionProvider_CUDA(cudaOption);//增加CUDA选项
		    }
		    else
			{
				NORMAL_EX_LOG("Inference device:CPU");
				//std::cout<<"Inference device:CPU "<< std::endl;
		    }
#ifdef _WIN32
		std::wstring w_modelPath = utils.charToWstring(modelPath.c_str());
		session = Ort::Session(env, w_modelPath.c_str(), sessionOptions);
#else
		session = Ort::Session(env, modelPath.c_str(), sessionOptions);
#endif
		Ort::AllocatorWithDefaultOptions allocator;//设置allocator分配
		Ort::TypeInfo inputTypeInfo = session.GetInputTypeInfo(0);//获取输入的类型信息
		std::vector<int64_t> inputTensorShape = inputTypeInfo.GetTensorTypeAndShapeInfo().GetShape();//获取输入的tensorshape
		this->isDynamicInputShape = false;
		//如下也是固定写法判断高和宽是不是固定的
		if (inputTensorShape[2] == -1 && inputTensorShape[3] == -1)
		{
			NORMAL_EX_LOG("Dynamic input shape");
			//std::cout << "Dynamic input shape" <<std:: endl;
			this->isDynamicInputShape = true;
		}
		for (int64_t shape : inputTensorShape) {
			//std::cout << "Input shape: " << shape << std::endl;
			NORMAL_EX_LOG("Input shape: %u ", shape);
		}
		//将输入和输出的节点名字添加进来
		inputNames.push_back(session.GetInputName(0, allocator));
		outputNames.push_back(session.GetOutputName(0, allocator));
		//std::cout << "Input name :" << inputNames[0] << std::endl;
		NORMAL_EX_LOG("[Input name : %s][Output name 1: %s]", inputNames[0], outputNames[0]);
		//std::cout << "Output name 1:" << outputNames[0] << std::endl;
		this->inputImageShape = cv::Size2f(inputSize);
	}

	void cyolov_onnxruntime::getBestClassInfo(std::vector<float>::iterator it, const int &numClasses, float &bestConf, int &bestClassId)
	{
		bestClassId = 5;
		bestConf = 0;
		for (int i = 5; i < numClasses + 5; i++)
		{
			if (it[i] > bestConf)
			{
				bestConf = it[i];
				bestClassId = i - 5;
			}
		}
	}
	/**
	 * @brief Detector::preprocession 图片预处理过程
	 * @param image
	 * @param blob
	 * @param inputTensorShape
	 */
	void cyolov_onnxruntime::preprocession(cv::Mat &image, float *&blob, std:: vector<int64_t> &inputTensorShape)
	{
		cv::Mat resizedImage, floatImage;
		cv::cvtColor(image, resizedImage, cv::COLOR_BGR2RGB);
		utils.letterbox(resizedImage, resizedImage, this->inputImageShape, cv::Scalar(114, 114, 114), this->isDynamicInputShape, false, true, 32);
		inputTensorShape[2] = resizedImage.rows;
		inputTensorShape[3] = resizedImage.cols;

		resizedImage.convertTo(floatImage, CV_32FC3, 1 / 255.0);
		blob = new float[floatImage.cols*floatImage.rows*floatImage.channels()];
		cv::Size floatImageSize{ floatImage.cols,floatImage.rows };
		//hwc->chw转换
		std::vector<cv::Mat> chw(floatImage.channels());
		for (int i = 0; i < floatImage.channels(); ++i)
		{
			chw[i] = cv::Mat(floatImageSize, CV_32FC1, blob + i * floatImageSize.width*floatImageSize.height);
		}
		cv::split(floatImage, chw);
	}
	/**
	 * @brief Detector::postprocessing 识别结果后处理阶段
	 * @param resizedImageShape 缩放后的图片
	 * @param originalImageShape 原始图片
	 * @param outputTensors 输出的Tensors
	 * @param confThreshold 置信度
	 * @param iouThreshold iou的值
	 * @return
	 */
	std::vector<CDetection> cyolov_onnxruntime::postprocessing(const cv::Size &resizedImageShape, const cv::Size &originalImageShape, std::vector<Ort::Value> &outputTensors, const float &confThreshold, const float &iouThreshold)
	{
		std::vector<cv::Rect> boxes;
		std::vector<float> confs;
		std::vector<int> classIds;
		auto* rawOutput = outputTensors[0].GetTensorData<float>();
		std::vector<int64_t> outputShape = outputTensors[0].GetTensorTypeAndShapeInfo().GetShape();
		size_t count = outputTensors[0].GetTensorTypeAndShapeInfo().GetElementCount();
		std::vector<float> output(rawOutput, rawOutput + count);
		int numClasses = (int)outputShape[2] - 5;
		int elementsInBatch = (int)(outputShape[1] * outputShape[2]);
		//only for batch size=1
		for (auto it = output.begin(); it != output.begin() + elementsInBatch; it += outputShape[2])
		{
			float clsConf = it[4];
			if (clsConf > confThreshold)
			{
				int centerX = (int)(it[0]);
				int centerY = (int)(it[1]);
				int width = (int)(it[2]);
				int height = (int)(it[3]);
				int left = centerX - width / 2;
				int top = centerY - height / 2;
				float objConf;
				int classId;
				this->getBestClassInfo(it, numClasses, objConf, classId);
				float confidence = clsConf * objConf;
				boxes.emplace_back(left, top, width, height);
				confs.emplace_back(confidence);
				classIds.emplace_back(classId);
			}
		}
		std::vector<int> indices;
		cv::dnn::NMSBoxes(boxes, confs, confThreshold, iouThreshold, indices);
		std::vector<CDetection> detections;

		for (int idx : indices) {
			CDetection det;
			det.bbox = cv::Rect(boxes[idx]);
			//将检测后的坐标还原回去
			utils.scaleCoords(resizedImageShape, det.bbox, originalImageShape);
			det.score = confs[idx];
			det.class_idx = classIds[idx];
			//std::cout << "the ..........." << classIds[idx];
			detections.emplace_back(det);
		}
		//std::cout << "\n" << std::endl;
		return detections;
	}
	/**
	 * @brief Detector::detect 检测的主代码
	 * @param image 图片
	 * @param confThreshold 置信度
	 * @param iouThreshold iou阈值
	 * @return
	 */
	std::vector<CDetection> cyolov_onnxruntime::detect(cv::Mat &image, const float &confThreshold = 0.4, const float &iouThreshold = 0.45)
	{
		float *blob = nullptr;
		std::vector<int64_t> inputTensorShape{ 1,3,-1,-1 };
		this->preprocession(image, blob, inputTensorShape);
		size_t inputTensorSize = utils.vectorProduct(inputTensorShape);
		std::vector<float> inputTensorValues(blob, blob + inputTensorSize);
		std::vector<Ort::Value> inputTensors;
		//Ort的固定写法
		Ort::MemoryInfo memoryInfo = Ort::MemoryInfo::CreateCpu(OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
		inputTensors.push_back(Ort::Value::CreateTensor<float>(memoryInfo, inputTensorValues.data(), inputTensorSize, inputTensorShape.data(),
			inputTensorShape.size()));
		//执行推断
		std::vector<Ort::Value> outputTensors = this->session.Run(Ort::RunOptions{ nullptr }, inputNames.data(),
			inputTensors.data(),
			1,
			outputNames.data(), 1);
		cv::Size resizedShape = cv::Size((int)inputTensorShape[3], (int)inputTensorShape[2]);
		std::vector<CDetection> result = this->postprocessing(resizedShape, image.size(), outputTensors, confThreshold, iouThreshold);
		delete[] blob;
		return result;
	}


}