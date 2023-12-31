﻿/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/
#ifndef C_MQTT_MGR_H
#define C_MQTT_MGR_H
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
#include "csingleton.h"
namespace chen
{
	struct cmqtt_msg
	{
		std::string m_topic;
		std::string m_payload;
		cmqtt_msg(): m_topic(""), m_payload(""){}
	};
	class cmqtt_mgr
	{
	private:
		typedef std::lock_guard<std::mutex>     clock_guard;
	public:
		explicit cmqtt_mgr() 
		: m_mqtt_client_ptr(NULL)
		, m_msgs(){}
		virtual ~cmqtt_mgr() {}


	public:

		bool init();
		bool startup();
		void destroy();


		void stop();

		void process(std::list<cmqtt_msg>&  msgs);
	public:
		void publish(const std::string& topic, const std::string& payload);
	public:
	private:

		void  _register_mqtt_connect(hv::MqttClient * client_ptr);
		void  _register_mqtt_receive(hv::MqttClient* cli, mqtt_message_t* msg);
		void  _register_mqtt_close(hv::MqttClient* cli);


	private:
		void _work_thread();
	protected:
	private:


		hv::MqttClient*		m_mqtt_client_ptr;
		std::thread			m_mqtt_thread;
		
		std::list<cmqtt_msg> m_msgs;
		std::mutex			m_lock;
	};
#define  s_mqtt_client_mgr   chen::csingleton<chen::cmqtt_mgr>::get_instance()
}
#endif // 