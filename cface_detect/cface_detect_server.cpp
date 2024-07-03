/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/


#include "cface_detect_server.h"


namespace chen {
	cface_detect_server g_face_detect_server;
	cface_detect_server::cface_detect_server(): m_stoped(true)
	{
	}
	cface_detect_server::~cface_detect_server()
	{
	}
	bool cface_detect_server::init(const char* log_path, const char* config_file)
	{
		return true;
	}
	void cface_detect_server::Loop()
	{
		//return false;
	}
	void cface_detect_server::destroy()
	{
	}
	void cface_detect_server::stop()
	{
	}
}