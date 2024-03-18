

#include "cweb_http_api_mgr.h"
#include <iostream>
#include "ccfg.h"
#include <thread>
#include "cweb_http_api_proxy.h"
#include "cdrone_client_mgr.h"
//#include <json.h>
#include "cvideo_analysis_mgr.h"
#include <json/json.h>
//#include "croom_mgr.h"
#include "ccfg.h"

#define PARSE_VALUE(VAR, TYPE, RESULT, PARSE_VALUE_TYPE) 							\
	if (data.isMember(#VAR) && data[#VAR].is##TYPE())			\
	{															\
		VAR = data[#VAR].as##TYPE();							\
		if (RESULT) \
		{			\
			PARSE_VALUE_TYPE = true;		\
		}			\
	}															\
    else														\
	{															\
		WARNING_EX_LOG("not find or type  [var = %s]  ", #VAR); \
	}


namespace chen {

	using namespace std;


	cweb_http_api_mgr g_web_http_api_mgr;
	cweb_http_api_mgr::cweb_http_api_mgr()
	: m_server(){}


	cweb_http_api_mgr::~cweb_http_api_mgr(){}
	bool cweb_http_api_mgr::init()
	{
		m_server.config.port = g_cfg.get_uint32(ECI_WebHttpWanPort);
		//m_server.resource["^/api_doc$"]["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request)
		//{
		//	Json::StyledWriter swriter;
		//	std::string str = swriter.write(value);
		//	//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
		//	response->write(str);
		//
		//};
		//m_server.resource["^/rooms$"]["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request)
		//{
		//	std::cout << "http thread id  :" << std::this_thread::get_id() << std::endl;
		//	std::vector<croom_info> room_infos = g_web_http_api_proxy.get_all_room();

		//	//if (infos.)
		//	Json::Value value;
		//	//Json::arrayValue Array_Value;
		//	//Json::Value Array_Value;
		//	//value["room_name"] = request->path_match[1].str();
		//	for (size_t i = 0; i < room_infos.size(); ++i)
		//	{

		//		Json::Value room_item;
		//		room_item["room_name"] = room_infos[i].room_name;

		//		for (size_t j = 0; j < room_infos [i].infos.size(); ++j)
		//		{
		//			Json::Value  item;
		//			item["user_name"] = room_infos[i].infos[j].username;
		//			item["type"] = room_infos [i].infos[j].m_type;
		//			item["ip"] = room_infos[i].infos[j].m_remote_ip;
		//			item["port"] = room_infos[i].infos[j].m_remote_port;
		//			room_item["user_infos"].append(item);
		//		}
		//		
		//		Json::Value  while_item;
		//		for (const std::string & user_ : room_infos[i].m_while_list)
		//		{
		//			while_item.append(user_);
		//			/*item[""] = infos[i].username;

		//			item["type"] = infos[i].m_type;
		//			item["ip"] = infos[i].m_remote_ip;*/
		//		}
		//		room_item["while_user"].append(while_item);
		//		value["room_infos"].append(room_item);
		//	}
		//	// = Array_Value;
		//	Json::StyledWriter swriter;
		//	std::string str = swriter.write(value);
		//	//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
		//	response->write(str);
		//	//g_web_http_api_proxy.g(request->path_match[1].str());
		//	//response->write(request->path_match[1].str());
		//};
		//// GET-example for the path /match/[number], responds with the matched string in path (number)
	// For instance a request GET /match/123 will receive: 123
		//m_server.resource["^/roomId/([a-zA-Z0-9_-]+)$"]["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request)
		//{
		//	std::cout << "http thread id  :" << std::this_thread::get_id() << std::endl;
		//	std::vector< chen::cuser_info> infos =  g_web_http_api_proxy.get_room_info(request->path_match[1].str());

		//	//if (infos.)
		//	Json::Value value;
		//	//Json::arrayValue Array_Value;
		//	//Json::Value Array_Value;
		//	value["room_name"] = request->path_match[1].str();
		//	for (size_t i = 0; i < infos.size(); ++i)
		//	{
		//		Json::Value  item;
		//		item["user_name"] = infos[i].username;

		//		item["type"] = infos[i].m_type ;
		//		item["ip"] = infos[i].m_remote_ip;
		//		item["port"] = infos[i].m_remote_port;
		//		value["user_infos"].append(item);
		//		//Json::Value  while_item;
		//		//for (const std::string & user_ :  infos.m_while_list)
		//		//{
		//		//	while_item.append(user_);
		//		//	/*item[""] = infos[i].username;

		//		//	item["type"] = infos[i].m_type;
		//		//	item["ip"] = infos[i].m_remote_ip;*/
		//		//}
		//		//value["while_user"].append(while_item);
		//	}
		//	 
		//	// = Array_Value;
		//	Json::StyledWriter swriter;
		//	std::string str = swriter.write(value);
		//	//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
		//	response->write(str);
		//};
		//// Kick out
		//m_server.resource["^/kickout/roomId/([a-zA-Z0-9_.@-]+)/username/([a-zA-Z0-9_.@-]+)$"]["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request)
		//{
		//	std::cout << "http thread id  :" << std::this_thread::get_id() << std::endl;
		//	//std::vector< chen::cuser_info> infos = g_web_http_api_proxy.get_room_info(request->path_match[1].str());
		//	std::cout << "room_name = " << request->path_match[1] << ", username = " << request->path_match[2];
		//	//if (infos.)
		//	Json::Value value;
		//	value["result"] = g_web_http_api_proxy.kick_room_username(request->path_match[1], request->path_match[2]);
		//	//Json::arrayValue Array_Value;
		//	//Json::Value Array_Value;
		//	//value["room_name"] = request->path_match[1].str();
		//	/*for (size_t i = 0; i < infos.size(); ++i)
		//	{
		//		Json::Value  item;
		//		item["user_name"] = infos[i].username;
		//		item["type"] = infos[i].m_type;
		//		item["ip"] = infos[i].m_remote_ip;
		//		item["port"] = infos[i].m_remote_port;
		//		value["user_infos"].append(item);
		//	}*/
		//	// = Array_Value;
		//	Json::StyledWriter swriter;
		//	std::string str = swriter.write(value);
		//	//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
		//	response->write(str);
		//};


		//m_server.resource["^/add_while/roomId/([a-zA-Z0-9_.@-]+)/username/([a-zA-Z0-9_.@-]+)$"]["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request)
		//{
		//	std::cout << "http thread id  :" << std::this_thread::get_id() << std::endl;
		//	//std::vector< chen::cuser_info> infos = g_web_http_api_proxy.get_room_info(request->path_match[1].str());
		//	std::cout << "room_name = " << request->path_match[1] << ", username = " << request->path_match[2];
		//	//if (infos.)
		//	Json::Value value;
		//	value["result"] = g_web_http_api_proxy.add_while_room_username(request->path_match[1], request->path_match[2]);
		//	//Json::arrayValue Array_Value;
		//	//Json::Value Array_Value;
		//	//value["room_name"] = request->path_match[1].str();
		//	/*for (size_t i = 0; i < infos.size(); ++i)
		//	{
		//		Json::Value  item;
		//		item["user_name"] = infos[i].username;
		//		item["type"] = infos[i].m_type;
		//		item["ip"] = infos[i].m_remote_ip;
		//		item["port"] = infos[i].m_remote_port;
		//		value["user_infos"].append(item);
		//	}*/
		//	// = Array_Value;
		//	Json::StyledWriter swriter;
		//	std::string str = swriter.write(value);
		//	//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
		//	response->write(str);
		//};

		m_server.resource["^/topic_video_analysis$"] ["POST"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request)
		{
			std::cout << "http thread id  :" << std::this_thread::get_id() << std::endl;
			NORMAL_EX_LOG("---topic_video_analysis$->>>");
			// Retrieve string:
			std::string  content = request->content.string();


			// pase json 
			Json::Reader reader;
			Json::Value data;

			if (!reader.parse((const char*)content.c_str(), (const char*)content.c_str() + content.size(), data))
			{
				WARNING_EX_LOG("parse  [payload = %s] failed !!!",   content.c_str());
				std::string ret = "parse   failed !!! ";
				Json::Value value;
				value["result"] = 500;
				Json::StyledWriter swriter;
				std::string str = swriter.write(value);
				//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
				response->write(str);
				return;
			}

			std::string client;
			std::string source;
			uint32       action;
			uint32		 video_skip_frame = 0;
			bool         parse_video_skip_frame = false;
			uint32		 car_analysis = 0;
			bool		 parse_car_analysis = false;
			std::string	 result_video_analysis;
		 
			try
			{
				PARSE_VALUE(client, String, false, parse_video_skip_frame);
				PARSE_VALUE(source, String, false, parse_video_skip_frame);
				PARSE_VALUE(action, UInt, false, parse_video_skip_frame);
				PARSE_VALUE(video_skip_frame, UInt, true, parse_video_skip_frame);
				PARSE_VALUE(car_analysis, UInt, true, parse_car_analysis);
				PARSE_VALUE(result_video_analysis, String, false, parse_video_skip_frame);

			}
			catch (const std::exception&)
			{ 
				WARNING_EX_LOG("parse value  [payload = %s] failed !!!", content.c_str());
				std::string ret = "parse   failed !!! ";
				/**response << "HTTP/1.1 200 OK\r\nContent-Length: " << ret.length() << "\r\n\r\n"
					<< ret;*/
				Json::Value value;
				value["result"] = 501;
				Json::StyledWriter swriter;
				std::string str = swriter.write(value);
				//g_wan_server.send_msg(m_session_id, msg_id, str.c_str(), str.length());
				response->write(str);
				return;
			}
			cdrone_action_info action_info;
			action_info.client = client;
			action_info.source = source;
			action_info.action = action;
			if (video_skip_frame < 5)
			{
				video_skip_frame = 5;
			}
			action_info.video_skip_frame = video_skip_frame;
			action_info.car_analysis = car_analysis;
			action_info.result_video_analysis = result_video_analysis;
			uint32_t ret = g_web_http_api_proxy.topic_video_analysis(action_info);
			 	Json::Value value;
				value["result"] = ret;
				Json::StyledWriter swriter;
			std::string str = swriter.write(value);
			 
			response->write(str);
			 
		};


		m_server.resource["^/get_all_video_analysis_info"]["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request)
		{
			std::cout << "http thread id  :" << std::this_thread::get_id() << std::endl;
			NORMAL_EX_LOG("---get_all_video_analysis_info->>>");
			std::vector< cvideo_analysis_info>  cvideo_analysis_infos = g_web_http_api_proxy.get_all_video_analysis_info( );
			Json::Value value;
			value["result"] = 200;
			 for (size_t i = 0; i < cvideo_analysis_infos.size(); ++i)
		 	{
		 		Json::Value  item;
				item["client"] = cvideo_analysis_infos[i].client.c_str();
		 		item["source"] = cvideo_analysis_infos[i].source.c_str();
		 		item["action"] = cvideo_analysis_infos[i].action;
		 		item["car_analysis"] = cvideo_analysis_infos[i].car_analysis;
		 		item["video_skip_frame"] = cvideo_analysis_infos[i].video_skip_frame;
				item["result_video_analysis"] = cvideo_analysis_infos[i].result_video_analysis.c_str();
		 		value["video_analysis_infos"].append(item);
		 	} 
			Json::StyledWriter swriter;
			std::string str = swriter.write(value);
			response->write(str);
		};
		//// Default GET-example. If no other matches, this anonymous function will be called.
	// Will respond with content in the web/-directory, and its subdirectories.
	// Default file: index.html
	// Can for instance be used to retrieve an HTML 5 client that uses REST-resources on this server
		m_server.default_resource["GET"] = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> response, std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> request) {
			try {
				auto web_root_path = boost::filesystem::canonical("web");
				auto path = boost::filesystem::canonical(web_root_path / request->path);
				// Check if path is within web_root_path
				if (distance(web_root_path.begin(), web_root_path.end()) > distance(path.begin(), path.end()) ||
					!equal(web_root_path.begin(), web_root_path.end(), path.begin()))
					throw invalid_argument("path must be within root path");
				if (boost::filesystem::is_directory(path))
					path /= "index.html";

				SimpleWeb::CaseInsensitiveMultimap header;

				// Uncomment the following line to enable Cache-Control
				// header.emplace("Cache-Control", "max-age=86400");

#ifdef HAVE_OPENSSL
//    Uncomment the following lines to enable ETag
//    {
//      ifstream ifs(path.string(), ifstream::in | ios::binary);
//      if(ifs) {
//        auto hash = SimpleWeb::Crypto::to_hex_string(SimpleWeb::Crypto::md5(ifs));
//        header.emplace("ETag", "\"" + hash + "\"");
//        auto it = request->header.find("If-None-Match");
//        if(it != request->header.end()) {
//          if(!it->second.empty() && it->second.compare(1, hash.size(), hash) == 0) {
//            response->write(SimpleWeb::StatusCode::redirection_not_modified, header);
//            return;
//          }
//        }
//      }
//      else
//        throw invalid_argument("could not read file");
//    }
#endif

				auto ifs = make_shared<ifstream>();
				ifs->open(path.string(), ifstream::in | ios::binary | ios::ate);

				if (*ifs) {
					auto length = ifs->tellg();
					ifs->seekg(0, ios::beg);

					header.emplace("Content-Length", to_string(length));
					response->write(header);

					// Trick to define a recursive function within this scope (for example purposes)
					class FileServer {
					public:
						static void read_and_send(const shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Response> &response, const std::shared_ptr<ifstream> &ifs) {
							// Read and send 128 KB at a time
							static vector<char> buffer(131072); // Safe when server is running on one thread
							streamsize read_length;
							if ((read_length = ifs->read(&buffer[0], static_cast<streamsize>(buffer.size())).gcount()) > 0) {
								response->write(&buffer[0], read_length);
								if (read_length == static_cast<streamsize>(buffer.size())) {
									response->send([response, ifs](const SimpleWeb::error_code &ec) {
										if (!ec)
											read_and_send(response, ifs);
										else
											std::cerr << "Connection interrupted" << endl;
									});
								}
							}
						}
					};
					FileServer::read_and_send(response, ifs);
				}
				else
					throw std::invalid_argument("could not read file");
			}
			catch (const std::exception &e) {
				response->write(SimpleWeb::StatusCode::client_error_bad_request, "Could not open path " + request->path + ": " + e.what());
			}
		};

		m_server.on_error = [](std::shared_ptr<SimpleWeb::Server<SimpleWeb::HTTP>::Request> /*request*/, const SimpleWeb::error_code & /*ec*/) {
			// Handle errors here
			// Note that connection timeouts will also call this handle with ec set to SimpleWeb::errc::operation_canceled
		};

		// Start server and receive assigned port when server is listening for requests
		//std::promise<unsigned short> server_port;
		//std::thread server_thread([&m_server, &server_port]() {
		//	// Start server
		//	m_server.start([&server_port](unsigned short port) {
		//		server_port.set_value(port);
		//	});
		//});
		//std::cout << "Server listening on port " << server_port.get_future().get() << std::endl << std::endl;



		return true;
	}
	void cweb_http_api_mgr::update()
	{}
	void cweb_http_api_mgr::destroy()
	{
		m_server.stop();
		if (m_thread.joinable())
		{
			m_thread.join();
		}
	}

	void cweb_http_api_mgr::startup()
	{
		std::promise<unsigned short> server_port;
		std::thread t_([this, &server_port]() 
		{
			// Start server
			m_server.start([&server_port](unsigned short port) 
			{
				server_port.set_value(port);
			});
		});
		std::cout << "Server listening on port " << server_port.get_future().get() << std::endl;
		m_thread.swap(t_);
	}
	void cweb_http_api_mgr::_pthread_work()
	{
		


	}

	  uint32_t      cweb_http_api_mgr:: topic_video_analysis(const struct cdrone_action_info& action_info)
	{
		  return g_drone_client_mgr.http_drone_client_action(action_info);
	}
	  std::vector< cvideo_analysis_info>  cweb_http_api_mgr::get_all_video_analysis_info()
	  {
		  //std::vector< cvideo_analysis_info> video_analysis_infos;
		 // g_video_analysis_mgr.build_video_analysis_infos(video_analysis_infos);
			return g_drone_client_mgr.http_drone_client_video_analysis_infos();
	  }
	/*std::vector< croom_info>   cweb_http_api_mgr::get_all_room()
	{

		std::cout << "work thread id :" << std::this_thread::get_id() <<std::endl;
		std::vector< croom_info>  room_infos;
			g_room_mgr.build_all_room_info(room_infos);
		return room_infos;
	}
	std::vector< chen::cuser_info>   cweb_http_api_mgr::get_room_info(const std::string& room_name )
	{
		std::vector< chen::cuser_info> infos;

		g_room_mgr.get_room_info(room_name, infos);

		std::cout << room_name << std::endl;
		return infos;
	}
	uint32_t cweb_http_api_mgr::kick_room_username(const std::string & room_name, const std::string & user_name)
	{
		return g_room_mgr.kick_room_username(room_name, user_name);
	}

	  uint32_t  cweb_http_api_mgr::add_while_room_username(const std::string& room_name, const std::string & user_name)
	  {
		  return g_room_mgr.add_white_room_username(room_name, user_name);
	  }
	  uint32_t  cweb_http_api_mgr::delete_while_room_username(const std::string& room_name, const std::string & user_name)
	  {
		  return g_room_mgr.delete_white_room_username(room_name, user_name);
	  }*/
}
