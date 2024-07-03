#define _CRT_SECURE_NO_WARNINGS
#include <iostream>

#include <csignal>
#include "cface_detect_server.h"
#include "httplib.h"


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

//*/
void RegisterSignal()
{
	signal(SIGINT, Stop);
	signal(SIGTERM, Stop);

}
int main(int argc, char* argv[])
{
	/*
	curl -X POST "http://192.168.2.20:8078/api/v1/recognition/recognize" -H "accept: */
	// *" -H "x-api-key: 00000000-0000-0000-0000-000000000002" 
	// -H "Content-Type: multipart/form-data" -F "file=@595577FF-59DC-49d2-BF5F-12FC58C406A2.png;type=image/png"
	
	//*/
	test_http_post();
	return 0;
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