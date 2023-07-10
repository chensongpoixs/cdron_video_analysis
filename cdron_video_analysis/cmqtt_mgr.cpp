/***********************************************************************************************
created: 		2019-03-02

author:			chensong

purpose:		log
************************************************************************************************/

#include "cmqtt_mgr.h"
#include "clog.h"
#include "ccfg.h"
#include "clog.h"
namespace chen {

	bool cmqtt_mgr::init()
	{
		if (m_mqtt_thread.joinable())
		{
			m_mqtt_thread.join();
		}
		m_mqtt_client_ptr = new hv::MqttClient();
		if (!m_mqtt_client_ptr)
		{
			ERROR_EX_LOG("[mqtt client object alloc failed !!!]\n");
			return false;
		}
		// 注册订阅的消息
		 
		m_mqtt_client_ptr->onConnect = std::bind(&cmqtt_mgr::_register_topic_all, this, std::placeholders::_1);
		m_mqtt_client_ptr->onMessage = std::bind(&cmqtt_mgr::_register_mqtt_receive, this, std::placeholders::_1, std::placeholders::_2);
		m_mqtt_client_ptr->onClose = std::bind(&cmqtt_mgr::_register_mqtt_close, this, std::placeholders::_1);


		//////////////////

#if TEST_AUTH
		cli.setAuth("test", "123456");
#endif

 
		reconn_setting_t reconn;
		reconn_setting_init(&reconn);
		reconn.min_delay = 1000;
		reconn.max_delay = 10000;
		reconn.delay_policy = 2;
		m_mqtt_client_ptr->setReconnect(&reconn);
		m_mqtt_client_ptr->setPingInterval(10);
		return true;
	}

	bool cmqtt_mgr::startup()
	{
		if (m_mqtt_client_ptr)
		{
			NORMAL_EX_LOG("connect mqtt [%s:%u]", g_cfg.get_string(ECI_MqttHost).c_str(), g_cfg.get_uint32(ECI_MqttPort));
			m_mqtt_client_ptr->connect(g_cfg.get_string(ECI_MqttHost).c_str(), g_cfg.get_uint32(ECI_MqttPort), false);
			 
			m_mqtt_thread = std::thread(&cmqtt_mgr::_work_thread, this);


			return true;
		}
		return false;
	}

	void cmqtt_mgr::destroy()
	{
		//stop();
		NORMAL_EX_LOG("");
		if (m_mqtt_thread.joinable())
		{
			NORMAL_EX_LOG("mqtt mgr thread join ");
			m_mqtt_thread.join();
			SYSTEM_LOG("mqtt server thread exit !!!");
		}
		NORMAL_EX_LOG("");
		if (m_mqtt_client_ptr)
		{
			
			delete m_mqtt_client_ptr;
			m_mqtt_client_ptr = NULL;
		}
	}

	void cmqtt_mgr::stop()
	{
		if (m_mqtt_client_ptr)
		{
			m_mqtt_client_ptr->stop();
		}
	}

	void cmqtt_mgr::publish(const std::string & topic, const std::string & payload)
	{
		if (m_mqtt_client_ptr)
		{
			m_mqtt_client_ptr->publish(topic, payload);
		}
	}

	void cmqtt_mgr::_register_topic_all(hv::MqttClient * client_ptr)
	{
		if (!client_ptr)
		{
			NORMAL_EX_LOG("mqtt client ptr == null failed !!!\n");
			return;
		}
		SYSTEM_LOG("mqtt connect ok !!!");
		static const char *VideoAnalysis = "VideoAnalysis";
		client_ptr->subscribe(VideoAnalysis);
	}

	void cmqtt_mgr::_register_mqtt_receive(hv::MqttClient * cli, mqtt_message_t * msg)
	{
		NORMAL_EX_LOG("topic: %.*s\n", msg->topic_len, msg->topic);
		NORMAL_EX_LOG("payload: %.*s\n", msg->payload_len, msg->payload);
	}

	void cmqtt_mgr::_register_mqtt_close(hv::MqttClient * cli)
	{
		NORMAL_EX_LOG("disconnected!\n");
	}

	void cmqtt_mgr::_work_thread()
	{
		if (m_mqtt_client_ptr)
		{
			m_mqtt_client_ptr->run();
			SYSTEM_LOG("mqtt server exit !!!");
		}
		else
		{
			ERROR_EX_LOG("mqtt server run failed !!! m_mqtt_client_ptr == NULL");
		}

	}

}