/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef _C_FACE_DETECT_SERVER_H_
#define _C_FACE_DETECT_SERVER_H_


namespace chen {


	class  cface_detect_server
	{
	public:
		explicit cface_detect_server();
		virtual ~cface_detect_server();




	public:
		bool init(const char* log_path, const char* config_file);
		void Loop();
		void destroy();
		void stop();
	private:
	private:
		volatile bool m_stoped;
	};

	extern cface_detect_server g_face_detect_server;
}



#endif // _C_FACE_DETECT_SERVER_H_