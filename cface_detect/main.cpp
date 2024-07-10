#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include <csignal>
#include "cface_detect_server.h"
#include "httplib.h"
#include "cvideo_analysis.h"
#include "chttp_face_util.h"

#include <iostream> 
#include <fstream>
#include <string>
#include <json/json.h>
void Stop(int i)
{

	chen::g_face_detect_server.stop();
}

long get_file_size(FILE* stream)
{
	long file_size = -1;
	long cur_offset = ::ftell(stream);	// 获取当前偏移位置
	if (cur_offset == -1) {
		printf("ftell failed :%s\n", strerror(errno));
		return -1;
	}
	if (fseek(stream, 0, SEEK_END) != 0) {	// 移动文件指针到文件末尾
		printf("fseek failed: %s\n", strerror(errno));
		return -1;
	}
	file_size = ::ftell(stream);	// 获取此时偏移值，即文件大小
	if (file_size == -1) {
		printf("ftell failed :%s\n", strerror(errno));
	}
	if (fseek(stream, cur_offset, SEEK_SET) != 0) {	// 将文件指针恢复初始位置
		printf("fseek failed: %s\n", strerror(errno));
		return -1;
	}
	return file_size;
}
 



void _test_http_image()
{

	std::string result;
	FILE* in_file_ptr = fopen("D:/Work/cai/db/人脸_陈虹旭_2.jpg", "rb");
	if (in_file_ptr == NULL)
	{
		printf("not open image failed !!!\n");
		return;
	}

	long file_size = get_file_size(in_file_ptr);
	unsigned char* buffer_ptr = new unsigned char[file_size + 1];
	if (!buffer_ptr)
	{
		return;
	}

	//std::string camera_data;
	//camera_data.resize(size + 1);
	size_t read_size = ::fread((void*)buffer_ptr, 1, file_size, in_file_ptr);
	//bay.push_back(std::string(buffer_ptr, file_size));
	//bay.append(buffer_ptr);
	 std::string image_data =  std::string((char*)buffer_ptr, file_size);
	chen::http_face_util::face_http_post("http://192.168.2.20:8078", "/api/v1/recognition/recognize", image_data, result);

}

void test_http_post()
{

	///*
	//curl -X POST "http://192.168.2.20:8078/api/v1/recognition/recognize" 
	//-H "accept: */ //*" -H "x-api-key: 00000000-0000-0000-0000-000000000002" 
	//-H "Content-Type: multipart/form-data" -F "det_prob_threshold=0.1" 
	//-F "face mask=0.1" -F "file=@贝蕾.jpg;type=image/jpeg"

	std::string web_http = "http://192.168.2.20:8078";
	httplib::Client cli(web_http);
	try
	{
		httplib::Headers headers;
		//headers["x-api-key"] = "00000000-0000-0000-0000-000000000002";
		headers.insert(std::make_pair("Accept", "*/*"));
		headers.insert(std::make_pair("Accept-Encoding", "gzip, deflate"));
		headers.insert(std::make_pair("Connection", "keep-alive"));
		headers.insert(std::make_pair("x-api-key", "00000000-0000-0000-0000-000000000002"));
		//headers.insert(std::make_pair("Content-Type", "multipart/form-data; boundary=----WebKitFormBoundaryXjaYW0dP6bnlAzGo"));
		headers.insert(std::make_pair("Cookie", "k0XBxWqacTscUOqx8WzKI407vFiCNbb5MIw_CaEb:P9Msf8J8YpOMOow62fhLocCDihM=:eyJzY29wZSI6Imp1bGl5ZSIsImR"));
		// -F "face_plugins=landmarks,gender,age" -F "file=@595577FF-59DC-49d2-BF5F-12FC58C406A2.png;type=image/png"
		std::string bay = "------WebKitFormBoundaryXjaYW0dP6bnlAzGo\r\nContent-Disposition: form-data; name=\"file\"; filename=\"test.jpg\"\r\nContent-Type: image/jpeg\r\n\r\n";
		
		
		FILE* in_file_ptr = fopen("D:/Work/cai/db/人脸_陈虹旭_2.jpg", "rb");
		if (in_file_ptr == NULL)
		{
			printf("not open image failed !!!\n");
			return;
		}

		long file_size = get_file_size(in_file_ptr);
		unsigned char* buffer_ptr = new unsigned char[file_size+1];
		if (!buffer_ptr)
		{
			return;
		}

		//std::string camera_data;
		//camera_data.resize(size + 1);
		size_t read_size = ::fread((void*)buffer_ptr, 1, file_size, in_file_ptr);
		//bay.push_back(std::string(buffer_ptr, file_size));
		//bay.append(buffer_ptr);
		bay += std::string((char *)buffer_ptr, file_size);
		bay += "\r\n------WebKitFormBoundaryXjaYW0dP6bnlAzGo--\r\n";
		httplib::Result res = cli.Post("/api/v1/recognition/recognize"
		 , headers, bay.c_str(), bay.length() , "multipart/form-data; boundary=----WebKitFormBoundaryXjaYW0dP6bnlAzGo");
		if (res.error() == httplib::Error::Success && res->status == 200)
		{
			 printf("%s \n", res->body.c_str());
			//res.value();
		} 
		//std::string body = res->body;
		//	ErrorL << "\n" << body << "\n";
		//NORMAL_EX_LOG("body %s", body);

		//std::string conect = base64_decode(body);
		/*MA2D_Login reply;
		if (reply.ParsePartialFromArray(conect.c_str(), conect.size()) == false)
		{
			WARNING_EX_LOG("[http url = %s]parse  [payload = %s] failed !!!", paras.c_str(), conect.c_str());

			return false;
		}
		if (reply.result() != 0)
		{
			WARNING_EX_LOG("result(%u) failed !!!", reply.result());
			return false;
		}*/
	}
	catch (const std::exception& e)
	{
		printf("%s\n", e.what());
	}
	


}


void test_cv()
{
	chen::cvideo_analysis video_analysis;
	video_analysis.init();
	std::string media_url = "rtsp://admin:hik12345@192.168.2.77/h264/ch1/main/av_stream";
	video_analysis.startup(media_url);
	video_analysis.set_skip_frame(25);
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}


void test_parse_json_array()
{
	//std::string  json_data = "{\"result\":[{"age":{"probability":1,"high":37,"low":37},"gender":{"probability":1,"value":"male"},"box":{"probability":0.99244,"x_max":712,"y_max":714,"x_min":511,"y_min":433},"subjects":[{"subject":"第三方都是","embeddingId":"7f326944-e79e-466d-938c-65ead457c1ff","similarity":0.85326,"imgUrl":"/20240703/00000000-0000-0000-0000-000000000002/20240703085125_43203ad2-6b1d-429e-ae52-a0e4f16efbe5/5f46be58-2a37-4795-86e4-6ea6d75fa2f6.png"}],"landmarks":[[635,545],[677,526],[709,579],[665,649],[694,634]]},{"age":{"probability":1,"high":34,"low":34},"gender":{"probability":1,"value":"male"},"box":{"probability":0.24922,"x_max":1097,"y_max":291,"x_min":1053,"y_min":236},"subjects":[{"subject":"向威","embeddingId":"813281ae-5252-42e9-8609-cc0673bc6f85","similarity":0.57596,"imgUrl":"/20240703/00000000-0000-0000-0000-000000000002/20240703085125_43203ad2-6b1d-429e-ae52-a0e4f16efbe5/6e206395-9bc1-4cba-a8fc-5b98c199166c.png"}],"landmarks":[[1070,255],[1088,255],[1080,266],[1072,277],[1086,277]]},{"age":{"probability":1,"high":16,"low":16},"gender":{"probability":1,"value":"female"},"box":{"probability":0.20415,"x_max":989,"y_max":158,"x_min":948,"y_min":107},"subjects":[{"subject":"索境","embeddingId":"44c3f52e-f549-42e9-9cb9-4573ad8cffae","similarity":0.55792,"imgUrl":"/20240703/00000000-0000-0000-0000-000000000002/20240703085125_43203ad2-6b1d-429e-ae52-a0e4f16efbe5/a47da71d-cbbd-479f-a54e-96f73e3d80cf.png"}],"landmarks":[[962,128],[979,127],[973,138],[965,148],[978,147]]},{"age":{"probability":1,"high":17,"low":17},"gender":{"probability":1,"value":"male"},"box":{"probability":0.15177,"x_max":1090,"y_max":269,"x_min":1062,"y_min":238},"subjects":[{"subject":"索境","embeddingId":"44c3f52e-f549-42e9-9cb9-4573ad8cffae","similarity":0.49774,"imgUrl":"/20240703/00000000-0000-0000-0000-000000000002/20240703085125_43203ad2-6b1d-429e-ae52-a0e4f16efbe5/3136f665-735f-4c70-bb8b-6d8a569d52b6.png"}],"landmarks":[[1070,250],[1082,250],[1075,257],[1071,264],[1080,264]]}]}";


	std::ifstream ifs;
	ifs.open("C:/Users/Administrator/Desktop/beijingtaishan/format.1720072598102.txt");
	Json::Value val;
	Json::Reader reader;

	if (!reader.parse(ifs, val)) 
	{
		printf("parse failed !!!\n");
		return  ;
	}

	int sz = val["result"].size();
	for (int i = 0; i < sz; ++i)
	{
		std::cout << "result " << i + 1 << ": ";
	//	std::cout << val["result"][i].asString();




		std::cout << "x = " << val["result"][i]["box"]["x_min"].asInt() << ", y = " << val["result"][i]["box"]["y_min"].asInt();
		std::cout << ", width = " << val["result"][i]["box"]["x_max"].asInt() - val["result"][i]["box"]["x_min"].asInt() << ", height = " << val["result"][i]["box"]["y_max"].asInt() - val["result"][i]["box"]["y_min"].asInt();







		std::cout << std::endl;
	}


}

#include "facedetectcnn.h"
//define the buffer size. Do not change the size!
#define DETECT_BUFFER_SIZE 0x20000
int test_open_main_cv() {
	// 加载图像
	cv::Mat image = cv::imread("2.jpg");

	// 进行人脸检测
	int* pResults = NULL;
	unsigned char* pBuffers = new unsigned char [10240];//large enough
	/*for (int i = 0; i < 1; i++)
	{
		pBuffers[i] = p + (DETECT_BUFFER_SIZE)*i;
	}*/
	cv::Mat  new_image = image.clone();
	cv::imshow("Face Detection", image);
	cv::waitKey(1);
	//pResults = facedetect_cnn(pBuffers, (unsigned char*)new_image.data, new_image.cols, new_image.rows, new_image.step);
	std::vector<  FaceRect> faces = facedetect_cnn(new_image.data, new_image.cols, new_image.rows, new_image.step);
	// 获取检测结果
	int count = pResults ? *pResults : 0;
	printf("Detected %d faces\n", count);

	// 绘制检测结果
	for (int i = 0; i < faces.size(); ++i) {
		//short* p = ((short*)(pResults + 1)) + 142 * i;
		int x = faces[i].x;
		int y = faces[i].y;
		int w = faces[i].w;
		int h = faces[i].h;
		cv::rectangle(image, cv::Rect(x, y, w, h), cv::Scalar(0, 255, 0), 2);
	}

	// 释放检测结果内存
	if (pResults) {
		free(pResults);
	}

	// 显示结果图像
	cv::imshow("Face----Detection", image);
	cv::waitKey(0);

	return 0;
}

//*/
void RegisterSignal()
{
	signal(SIGINT, Stop);
	signal(SIGTERM, Stop);

}
int main(int argc, char* argv[])
{
	//return test_main(argc, argv);
	//test_open_main_cv();
	//return 0;
	/*
	curl -X POST "http://192.168.2.20:8078/api/v1/recognition/recognize" -H "accept: */
	// *" -H "x-api-key: 00000000-0000-0000-0000-000000000002" 
	// -H "Content-Type: multipart/form-data" -F "file=@595577FF-59DC-49d2-BF5F-12FC58C406A2.png;type=image/png"
	//_test_http_image();
	//return 0;
	//*/
	//test_parse_json_array();
	//return 0;
	/*test_cv();
	return 0;
	test_http_post();
	return 0;*/
	RegisterSignal();

	const char* config_filename = "server.cfg";
	if (argc > 1)
	{
		config_filename = argv[1];
	}
	const char* log_path = "./log";
	if (argc > 2)
	{
		log_path = argv[2];
	} 
	bool init = chen::g_face_detect_server.init(log_path, config_filename);

	if (init)
	{
		  chen::g_face_detect_server.Loop();
	}

	chen::g_face_detect_server.destroy();
	if (!init)
	{
		return 1;
	}


	return 0;
}