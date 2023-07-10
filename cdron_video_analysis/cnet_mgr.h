/***********************************************************************************************
created: 		2019-03-06

author:			chensong

purpose:		net_mgr
输赢不重要，答案对你们有什么意义才重要。

光阴者，百代之过客也，唯有奋力奔跑，方能生风起时，是时代造英雄，英雄存在于时代。或许世人道你轻狂，可你本就年少啊。 看护好，自己的理想和激情。


我可能会遇到很多的人，听他们讲好2多的故事，我来写成故事或编成歌，用我学来的各种乐器演奏它。
然后还可能在一个国家遇到一个心仪我的姑娘，她可能会被我帅气的外表捕获，又会被我深邃的内涵吸引，在某个下雨的夜晚，她会全身淋透然后要在我狭小的住处换身上的湿衣服。
3小时候后她告诉我她其实是这个国家的公主，她愿意向父皇求婚。我不得已告诉她我是穿越而来的男主角，我始终要回到自己的世界。
然后我的身影慢慢消失，我看到她眼里的泪水，心里却没有任何痛苦，我才知道，原来我的心被丢掉了，我游历全世界的原因，就是要找回自己的本心。
于是我开始有意寻找各种各样失去心的人，我变成一块砖头，一颗树，一滴水，一朵白云，去听大家为什么会失去自己的本心。
我发现，刚出生的宝宝，本心还在，慢慢的，他们的本心就会消失，收到了各种黑暗之光的侵蚀。
从一次争论，到嫉妒和悲愤，还有委屈和痛苦，我看到一只只无形的手，把他们的本心扯碎，蒙蔽，偷走，再也回不到主人都身边。
我叫他本心猎手。他可能是和宇宙同在的级别 但是我并不害怕，我仔细回忆自己平淡的一生 寻找本心猎手的痕迹。
沿着自己的回忆，一个个的场景忽闪而过，最后发现，我的本心，在我写代码的时候，会回来。
安静，淡然，代码就是我的一切，写代码就是我本心回归的最好方式，我还没找到本心猎手，但我相信，顺着这个线索，我一定能顺藤摸瓜，把他揪出来。
************************************************************************************************/
#ifndef _C_NET_MGR_H_
#define _C_NET_MGR_H_
#include "cnoncopyable.h"
//#include "creactor.h"
#include "cnet_type.h"
//#include "cacceptor.h"
#include <iostream>
#include <vector>
#include "cnet_session.h"
#include <atomic>
//#include "cqueue.h"
#include <queue>
#include <thread>
#include "cnet_msg.h"
#include "cnet_msg_queue.h"
#include "cnet_define.h"
//#include "csafe_queue.h"
namespace chen {
	class cnet_session;

	class cnet_mgr :public cnoncopyable
	{
	public:
		//Á¬½Ó»Øµ÷
		typedef std::function<void(uint32 session_id, uint32 para, const char* buf)> cconnect_cb;
		//¶Ï¿ªÁ¬½Ó»Øµ÷
		typedef std::function<void(uint32 session_id)> 								cdisconnect_cb;
		//ÐÂÏûÏ¢»Øµ÷
		typedef std::function<void(uint32 session_id, uint16 msg_id, const void* p, uint32 size)> cmsg_cb;
	

	private:
		typedef boost::asio::io_service					cservice;
		typedef boost::asio::io_service::work			cwork;
		typedef boost::asio::ip::tcp::socket			csocket_type;
		typedef boost::asio::ip::tcp::acceptor			cacceptor;
		typedef boost::asio::ip::tcp::endpoint			cendpoint;
		typedef boost::asio::deadline_timer				ctimer;
		typedef boost::system::error_code				cerror_code;
	private:

		typedef std::vector<cnet_session*> 											csessions;
		typedef std::vector<std::thread> 											cthreads;
		typedef std::atomic_bool 													catomic_bool;
		typedef std::atomic_uint64_t												catomic_uint;
		typedef std::mutex															clock_type;
		typedef std::lock_guard<clock_type>											clock_guard;
	private:
		struct cconnect_para
		{
			cconnect_para(cservice& io_service);
			std::string		ip;
			uint16			port;
			ctimer			timer;
			catomic_bool	busy;
		};

		typedef std::vector<cconnect_para*> cparas;
	public:
		explicit	cnet_mgr();
		~cnet_mgr();
	public:
		static cnet_mgr *	construct();
		static void			destroy(cnet_mgr * ptr);
	public:
		/**
		* @param name: ·þÎñµÄÃû³Æ
		* @param client_session:   ×÷Îª¿Í»§¶ËÊÇ×ªÈë 
		* @param max_session : ×÷Îª·þÎñ¶Ë ÊÇ×ªÈë
		* @param send_buff_size: ·¢ËÍ»º³åÇøµÄ´óÐ¡
		* @param recv_buff_size: ½ÓÊÕ»º³åÇøµÄ´óÐ¡
		**/
		bool 		init(const std::string& name, int32 client_session, int32 max_session
			, uint32 send_buff_size, uint32 recv_buff_size, uint32 pool_size);
		/**
		* ¹Ø·þ²Ù×÷
		**/
		void 		shutdown();
		void 		destroy();
	public:
		/**
		*  @param  thread_num ioÏß³ÌÊý
		*  @param  ip
		*  @param  port
		**/
		bool 		startup(uint32 thread_num, const char *ip, uint16 port);
	public:
		//ÏûÏ¢idÃØÔ¿
		void set_msg_id_key(uint16 value) { m_msg_id_key = value; }
		
		//ÏûÏ¢sizeÃØÔ¿
		void set_msg_size_key(uint32 value) { m_msg_size_key = value; }
		
		//×î´ó½ÓÊÕÏûÏ¢´óÐ¡
		void set_max_msg_size(uint32 value) { m_max_received_msg_size = value; }
		
		
		//×Ô¶¯ÖØÁ¬Ê±¼ä
		void set_reconnet_second(uint32 seconds) { m_reconnect_second = seconds; }

		//ÊÇ·ñÍâÍø
		void set_wan() { m_wan = true; }

	public:
		// client
		bool 		connect_to(uint32 index, const std::string& ip_address, uint16 port);
	public:
		//Á¬½Ó»Øµ÷
		void set_connect_callback(cconnect_cb callback) { m_connect_callback = callback; }

		//¶Ï¿ªÁ¬½Ó»Øµ÷
		void set_disconnect_callback(cdisconnect_cb callback) { m_disconnect_callback = callback; }

		//ÏûÏ¢»Øµ÷
		void set_msg_callback(cmsg_cb callback) { m_msg_callback = callback; }
	public:
		//// ÉèÖÃÁ¬½Ó³¬Ê±Ê±¼ä[ÐÄÌø°ü] µ¥Î» Ãë
		//void set_times_second(uint32 msleep);
		////// ¶ÏÏßÖØÁ¬Ê±¼ä µ¥Î» Ãë
		//void set_reconnection(uint32 msleep);
	public:
		void 		process_msg();

		bool		transfer_msg(uint32 sessionId, cnet_msg & msg);

		/**
		*  @param  sessionId : »Ø»°id
		*  @param  msg_id    : ÏûÏ¢id
		*  @param  msg_ptr   : ÏûÏ¢µÄÊý¾Ý
		*  @param  msg_size  : ÏûÏ¢µÄ´óÐ¡
		**/
		bool 		send_msg(uint32 sessionId, uint16 msg_id, const void* msg_ptr, uint32 msg_size);

		// Ö¸¶¨·þÎñÆ÷·¢ËÍ
		bool 		send_msg(uint32 sessionId, uint16 msg_id, const void* msg_ptr, uint32 msg_size, int32 extra);
		/**
		*  @param sessionId  : ¹Ø±ÕÖ¸¶¨µÄ»Ø»°id
		**/
		void 		close(uint32 sessionId);
		// Á¬½Ó¿Í»§¶ËµÄÊýÁ¿ºÍÊý¾Ý ÐÅÏ¢  
		void		show_info();
	public:
		/*
		*   ÏûÏ¢°ü
		*/
		cnet_msg*	create_msg(uint16 msg_id, uint32 msg_size);
		void		msg_push(cnet_msg * msg_ptr);
		
		void		post_disconnect(cnet_session * p);
	public:

		cmem_pool * get_pool() const { return m_pool_ptr; }
		uint16		get_msg_id_key() const { return m_msg_id_key; }
		uint32		get_msg_size_key() const { return m_msg_size_key; }
		uint32		get_max_msg_size() const { return m_max_received_msg_size; }
		const std::string& get_name() const { return m_name; }
		bool		is_wan() const { return m_wan; }
	private:
		void 		_worker_thread();
		///**
		//* @param  psession : Çå³ýÖ¸¶¨µÃµ½session
		//**/
		//void		_clearup_session(cnet_session * psession);

		//// µÃµ½Ò»¸öÐÂµÄ¿Í»§¶ËÁ¬½Ó
		//void		_new_connect();
	private:
		// server api
		// ·þÎñÆ÷Òì²½¼àÌýÊÇ·ñÓÐÐÂµÄ¿Í»§¶ËÁ¬½Ó
		//void		_listen_start(const char *ip, uint16 port);
		bool _start_listen(const std::string& ip, uint16 port, uint32 thread_num);
		void _post_accept();
		void _handle_accept(const cerror_code& error, cnet_session* session_ptr);
		cnet_msg* _create_accept_msg(const csocket_type& socket);
	private:
		// client api
		// Í¬²½Á¬½Ó·þÎñÆ÷
		bool		_connect_start(uint32 index, const char *ip, uint16 port);
	private:
		void		_handle_connect(const cerror_code& error, uint32 index);
		void		_handle_reconnect(uint32 index);
		cnet_msg*	_create_connect_msg(uint32 error_code);
	
	private:
		// È¡µÃÒ»¸ö¿ÉÓÃµÄ»á»°
		cnet_session* _get_available_session();

		// ¹é»¹Ò»¸ö¹Ø±ÕµÄ»á»°
		void _return_session(cnet_session* session_ptr);
	private:
		void _handle_close(cnet_session* session_ptr);
		bool _is_server_index(uint32 index) const;
	private:
		std::string						m_name;
		
		//creactor*						m_reactor;          // ·´Ó¦¶Ñ
															// callback	
		cservice						m_io_service;
		cwork							m_work;
		// 
		cmem_pool*						m_pool_ptr;

		// callback
		cconnect_cb						m_connect_callback;
		cmsg_cb							m_msg_callback;
		cdisconnect_cb					m_disconnect_callback;
		
		csessions						m_sessions;               //ËùÓÐ»á»°
		clock_type						m_avail_session_mutex;
		csessions						m_available_sessions;     //¿ªÊ¹ÓÃµÄ»á»°
		catomic_bool					m_shuting;
		cthreads 						m_threads;


		cacceptor*						m_acceptor_ptr;		// ¼àÌýsocket
		catomic_uint					m_listening_num;
		uint32							m_listening_max;
		//csafe_queue						m_msgs;
		//config
		uint16							m_msg_id_key;
		uint32							m_msg_size_key;
		uint32							m_max_received_msg_size;
		bool							m_wan;  //ÊÇ·ñÊÇÍø¹Ø

												//client
		uint32							m_reconnect_second;
		cparas							m_paras;
		
		//uint32							m_active_num; //·´Ó¦¶ÑµÄÊýÁ¿
		cnet_msg_queue*					m_msgs;
	};
} // chen 

#endif // _C_NET_MGR_H_
