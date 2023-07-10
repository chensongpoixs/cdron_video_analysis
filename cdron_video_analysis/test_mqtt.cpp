/*
 * mqtt client
 *
 * @build   make examples
 *
 * @test    bin/mqtt_client_test 127.0.0.1 1883 topic payload
 *
 */

#include <hv/mqtt_client.h>
//mqtt_protocol
#include <thread>
#include "cmqtt_mgr.h"
//using namespace hv;

/*
 * @test    MQTTS
 * #define  TEST_SSL 1
 *
 * @build   ./configure --with-mqtt --with-openssl && make clean && make
 *
 */
#define TEST_SSL        0
#define TEST_AUTH       1
#define TEST_RECONNECT  1
#define TEST_QOS        0 

int mqtt_test_main(int argc, char** argv) 
{
	chen::cmqtt_mgr g_mqtt_mgr;
	g_mqtt_mgr.init();
	g_mqtt_mgr.startup();
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	return 0;
//	if (argc < 5) {
//		printf("Usage: %s host port topic payload\n", argv[0]);
//		return -10;
//	}
//	const char* host = argv[1];
//	int port = atoi(argv[2]);
//	const char* topic = argv[3];
//	const char* payload = argv[4];
//
//	MqttClient cli;
//
//	cli.onConnect = [topic, payload](MqttClient* cli) {
//		printf("connected!\n");
//#if TEST_QOS
//		cli->subscribe(topic, 1, [topic, payload](MqttClient* cli) {
//			printf("subscribe OK!\n");
//			cli->publish(topic, payload, 1, 0, [](MqttClient* cli) {
//				printf("publish OK!\n");
//			});
//		});
//#else
//		cli->subscribe(topic);
//		//cli->publish(topic, payload);
//#endif
//	};
//
//	cli.onMessage = [&](MqttClient* cli, mqtt_message_t* msg) {
//		printf("topic: %.*s\n", msg->topic_len, msg->topic);
//		printf("payload: %.*s\n", msg->payload_len, msg->payload);
//
//		//	cli->publish(topic, payload);
//			//cli->disconnect();
//			//cli->stop();
//	};
//
//	cli.onClose = [](MqttClient* cli) {
//		printf("disconnected!\n");
//	};
//
//#if TEST_AUTH
//	cli.setAuth("test", "123456");
//#endif
//
//#if TEST_RECONNECT
//	reconn_setting_t reconn;
//	reconn_setting_init(&reconn);
//	reconn.min_delay = 1000;
//	reconn.max_delay = 10000;
//	reconn.delay_policy = 2;
//	cli.setReconnect(&reconn);
//#endif
//
//	cli.setPingInterval(10);
//
//	int ssl = 0;
//#if TEST_SSL
//	ssl = 1;
//#endif
//	cli.connect(host, port, ssl);
//
//
//	std::thread([&]()
//	{
//
//		while (true)
//		{
//			std::this_thread::sleep_for(std::chrono::microseconds(100));
//			printf("------>\n");
//			cli.publish(topic, payload);
//		}
//	}).detach();
//	cli.run();
	return 0;
}
