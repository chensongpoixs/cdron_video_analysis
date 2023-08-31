/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#include "ctorch_classify.h"
#include "clog.h"
namespace chen {



	 
	ctorch_classify::ctorch_classify(std::string pt)
		: m_device(torch::kCUDA)
	{
		if (torch::cuda::is_available()) {
			NORMAL_EX_LOG("CUDA available! Test on GPU.");
			m_device = torch::kCUDA;
		}
		else {
			NORMAL_EX_LOG("Test on CPU.");
			m_device = torch::kCPU;
		}
		m_model = torch::jit::load(pt);;
		m_model.to(m_device);
		m_model.eval();

	}
	ctorch_classify::~ctorch_classify()
	{
	}

	

	cls_socre ctorch_classify::classitfy(cv::Mat img)
	{
		//norm
		cv::Mat image_resized_float = setNorm(img);
		//mean
		cv::Mat image_resized_merge = setMean(image_resized_float);

		auto img_tensor = torch::from_blob(image_resized_merge.data, { 224, 224, 3 }, torch::kFloat32);
		auto img_tensor_ = torch::unsqueeze(img_tensor, 0);
		img_tensor_ = img_tensor_.permute({ 0, 3, 1, 2 });													//	auto img_tensor_ = torch::unsqueeze(img_tensor, 0);
		//img_tensor_ = img_tensor_.permute({ 0, 3, 1, 2 });
		//printf("[%s][%d]\n", __FUNCTION__, __LINE__);
		// Create a vector of inputs.
		std::vector<torch::jit::IValue> inputs;
		inputs.push_back(img_tensor_.to(m_device));
		torch::Tensor prob = m_model.forward(inputs).toTensor();
		torch::Tensor output = torch::softmax(prob, 1);
		auto predict = torch::max(output, 1);
		//cout << "cost time:" << clock() - start_t << endl;
		 //std::cout << img_paths[i] << "\t";
		//for (int  w = 0; w < 2; ++w)
		//{
		//	//	int index = w * 2;
		//	//	int po = w * 2 + 1;
		//	std::cout << "class: " << std::get<1>(predict).item<int>() <<
		//		", prob: " << std::get<0>(predict).item<float>() << std::endl;
		//	/*std::cout << "class: " << std::get<3>(predict).item<int>() <<
		//		", prob: " << std::get<2>(predict).item<float>() << std::endl;*/
		//}
		 cls_socre cur;
		 cur.index = std::get<1>(predict).item<int>();
		 cur.socre = std::get<0>(predict).item<float>();
		 return cur;// cls_socre{ std::get<1>(predict).item<int>() ,std::get<0>(predict).item<float>() };
	}

	 
	void ctorch_classify::destroy()
	{
	}

	cv::Mat ctorch_classify::pilResize(cv::Mat &img, int size) {
		int imgWidth = img.cols;
		int imgHeight = img.rows;
		if ((imgWidth <= imgHeight && imgWidth == size) || (imgHeight <= imgWidth && imgHeight == size)) {
			return img;
		}
		cv::Mat output;
		if (imgWidth < imgHeight) {
			int outWidth = size;
			int outHeight = int(size * imgHeight / (float)imgWidth);
			cv::resize(img, output, cv::Size(outWidth, outHeight));
		}
		else {
			int outHeight = size;
			int outWidth = int(size * imgWidth / (float)imgHeight);
			cv::resize(img, output, cv::Size(outWidth, outHeight));
		}

		return output;
	}

	cv::Mat ctorch_classify::pilCropCenter(cv::Mat &img, int output_size) {
		cv::Rect imgRect;
		imgRect.x = int(round((img.cols - output_size) / 2.));
		imgRect.y = int(round((img.rows - output_size) / 2.));
		imgRect.width = output_size;
		imgRect.height = output_size;

		return img(imgRect).clone();
	}

	cv::Mat ctorch_classify::setNorm(cv::Mat &img) {
		cv::Mat img_rgb;
		cv::cvtColor(img, img_rgb, cv::COLOR_RGB2BGR);

		cv::Mat img_resize = pilResize(img_rgb, 256);
		cv::Mat img_crop = pilCropCenter(img_resize, 224);

		cv::Mat image_resized_float;
		img_crop.convertTo(image_resized_float, CV_32F, 1.0 / 255.0);

		return image_resized_float;
	}

	cv::Mat ctorch_classify::setMean(cv::Mat &image_resized_float)
	{
		std::vector<float> mean = { 0.485, 0.456, 0.406 };
		std::vector<float> std = { 0.229, 0.224, 0.225 };

		std::vector<cv::Mat> image_resized_split;
		cv::split(image_resized_float, image_resized_split);
		for (int ch = 0; ch < image_resized_split.size(); ch++) 
		{
			image_resized_split[ch] -= mean[ch];
			image_resized_split[ch] /= std[ch];
		}
		cv::Mat image_resized_merge;
		cv::merge(image_resized_split, image_resized_merge);

		return image_resized_merge;
	}

	void test_classification()
	{

		//torch::DeviceType device_type;
		//if (torch::cuda::is_available()) {
		//	std::cout << "CUDA available! Test on GPU." << std::endl;
		//	device_type = torch::kCUDA;
		//}
		//else {
		//	std::cout << "Test on CPU." << std::endl;
		//	device_type = torch::kCPU;
		//}
		//torch::Device device(device_type);

		//// Deserialize the ScriptModule from a file using torch::jit::load().
		//torch::jit::script::Module model = torch::jit::load("weights/yolov5s-cls.torchscript");
		////torch::jit::script::Module model = torch::jit::load("weights/yolov5s-cls.pt");
		//model.to(device);
		//model.eval();
		//std::vector<std::string> classes = { "cat","dog" };

		///*std::string test_path = "val/dog/";
		//std::vector<std::string> img_paths;
		//cv::glob(test_path, img_paths);*/

		//int truth_count = 0;

		////for (int i = 0; i < img_paths.size(); i++)
		//{
		//	cv::Mat img = cv::imread("resource/images/0086_c015_00081770_0.jpg"/*img_paths[i]*/);

		//	//clock_t start_t = clock();

		//	//norm
		//	//cv::Mat image_resized_float = setNorm(img);
		//	//mean
		//	//cv::Mat image_resized_merge = setMean(image_resized_float);
		//	 
		//	//norm
		//	cv::Mat image_resized_float = setNorm(img);
		//	//mean
		//	cv::Mat image_resized_merge = setMean(image_resized_float);

		//	auto img_tensor = torch::from_blob(image_resized_merge.data, { 224, 224, 3 }, torch::kFloat32);
		//	auto img_tensor_ = torch::unsqueeze(img_tensor, 0);
		//	img_tensor_ = img_tensor_.permute({ 0, 3, 1, 2 });													//	auto img_tensor_ = torch::unsqueeze(img_tensor, 0);
		//	//img_tensor_ = img_tensor_.permute({ 0, 3, 1, 2 });
		//	printf("[%s][%d]\n", __FUNCTION__, __LINE__);
		//	// Create a vector of inputs.
		//	std::vector<torch::jit::IValue> inputs;
		//	inputs.push_back(img_tensor_.to(device));
		//	torch::Tensor prob = model.forward(inputs).toTensor();
		//	torch::Tensor output = torch::softmax(prob, 1);
		//	auto predict = torch::max(output, 1);
		//	//cout << "cost time:" << clock() - start_t << endl;
		//	 //std::cout << img_paths[i] << "\t";
		//	//for (int  w = 0; w < 2; ++w)
		//	{
		//	//	int index = w * 2;
		//	//	int po = w * 2 + 1;
		//		std::cout << "class: " << std::get<1>(predict).item<int>() <<
		//			", prob: " << std::get<0>(predict).item<float>() << std::endl;
		//		/*std::cout << "class: " << std::get<3>(predict).item<int>() <<
		//			", prob: " << std::get<2>(predict).item<float>() << std::endl;*/
		//	}
		//	 
		//	//std::cout << img_paths[i] << "\t";
		//	//std::cout << "class: " << std::get<1>(predict).item<int>() /*classes[std::get<1>(predict).item<int>()]*/ <<
		//	//	", prob: " << std::get<0>(predict).item<float>()/*std::get<0>(predict).item<float>()*/ << std::endl;

		//	/*if (std::get<1>(predict).item<int>() == 1) {
		//		truth_count++;
		//	}*/
	 //   }

		////std::cout << truth_count << "/" << img_paths.size() << std::endl;
		//system("pause");

	}



#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>


	//常量
	const int INPUT_WIDTH = 640;
	const int INPUT_HEIGHT = 640;


	//预处理
	void pre_process(cv::Mat& image, cv::Mat& blob)
	{
		//CenterCrop
		int crop_size = std::min(image.cols, image.rows);
		int  left = (image.cols - crop_size) / 2, top = (image.rows - crop_size) / 2;
		cv::Mat crop_image = image(cv::Rect(left, top, crop_size, crop_size));
		cv::resize(crop_image, crop_image, cv::Size(INPUT_WIDTH, INPUT_HEIGHT));

		//Normalize
		crop_image.convertTo(crop_image, CV_32FC3, 1. / 255.);
		cv::subtract(crop_image, cv::Scalar(0.406, 0.456, 0.485), crop_image);
		cv::divide(crop_image, cv::Scalar(0.225, 0.224, 0.229), crop_image);

		cv::dnn::blobFromImage(crop_image, blob, 1, cv::Size(crop_image.cols, crop_image.rows), cv::Scalar(), true, false);
	}

	//网络推理
	void process(cv::Mat& blob, cv::dnn::Net& net, std::vector<cv::Mat>& outputs)
	{
		net.setInput(blob);
		net.forward(outputs, net.getUnconnectedOutLayersNames());
	}


	//后处理
	void post_process(std::vector<cv::Mat>& detections/*, std::vector<std::string>& class_name*/)
	{
		std::vector<float> values;
		for (size_t i = 0; i < detections[0].cols; i++)
		{
			values.push_back(detections[0].at<float>(0, i));
		}
		int id = std::distance(values.begin(), std::max_element(values.begin(), values.end()));
		printf("id = %u\n", id);
		//return class_name[id];
	}


	int test__main( )
	{
		//std::vector<std::string> class_name;
		//std::ifstream ifs("class_cls.txt");
		//std::string line;

		/*while (getline(ifs, line))
		{
			class_name.push_back(line);
		}*/

		cv::Mat image = cv::imread("resource/images/0086_c015_00081770_0.jpg"), blob;
		pre_process(image, blob);

		cv::dnn::Net net = cv::dnn::readNet("weights/yolov5s-cls.onnx");
		std::vector<cv::Mat> outputs;
		process(blob, net, outputs);
		post_process(outputs);
		//std::cout << << std::endl;
		return 0;
	}


}