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
#include "cnet_mgr.h"
#include "clog.h"
#include "cnet_msg_reserve.h"
#include "cnet_msg_header.h"
#include <boost/bind.hpp>
namespace chen {

	cnet_mgr::cconnect_para::cconnect_para(cservice& io_service)
		: port(0), timer(io_service), busy(false)
	{}

	cnet_mgr::cnet_mgr()
		: m_name("")
		, m_work(m_io_service)
        , m_pool_ptr(NULL)
		, m_connect_callback(NULL)
		, m_msg_callback(NULL)
		, m_disconnect_callback(NULL)
		, m_shuting(false)
		, m_acceptor_ptr(NULL)
		, m_listening_num(0)
		, m_listening_max(0)
		, m_msg_id_key(0)
		, m_msg_size_key(0)
		, m_max_received_msg_size(0)
		, m_wan(false)
		, m_reconnect_second(0)
		, m_paras()
        , m_msgs(NULL)
	{
	}
	cnet_mgr::~cnet_mgr()
	{
	}
	cnet_mgr * cnet_mgr::construct()
	{
		return new cnet_mgr();
	}
	void cnet_mgr::destroy(cnet_mgr * ptr)
	{
		delete ptr;
	}
	bool cnet_mgr::init(const std::string & name, int32 client_session, int32 max_session, uint32 send_buff_size, uint32 recv_buff_size, uint32 pool_size)
	{
		if (client_session > max_session)
		{
			LOG_ERROR << " invalid para client_session=" << client_session;
			return false;
		}
		if (max_session < 1)
		{
			LOG_ERROR << "invalid para max_session = " << max_session;
			return false;
		}
		if (send_buff_size < MSG_HEADER_SIZE || send_buff_size > 50 * 1024 * 1024)
		{
			LOG_ERROR << "invalid para send_buff_size=" << send_buff_size;
			return false;
		}

		m_pool_ptr = new cmem_pool(true);
		if (!m_pool_ptr)
		{
			LOG_ERROR << "memory error";
			return false;
		}

		if (!m_pool_ptr->init(pool_size))
		{
			LOG_ERROR << "pool init error";
			return false;
		}

		//m_sessions.reserve(max_session);
		for (int32 i = 0; i < max_session; ++i)
		{ 
			m_sessions.push_back(new cnet_session(m_io_service, *this, i));
		}

		if (m_sessions.size() != max_session)
		{
			LOG_ERROR << "new session failed";
			return false;
		}

		for (cnet_session* session : m_sessions)
		{
			if (!session->init(send_buff_size, recv_buff_size))
			{
				return false;
			}
		}

		//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ê¼ï¿½ï¿½
		if (max_session > client_session)
		{
			int32 server_session_num = max_session - client_session;
			m_available_sessions.reserve(server_session_num);

			for (int32 i = max_session -1 ; i >= client_session; --i)
			{ 
				m_available_sessions.push_back(m_sessions[i]);
			}

			if (m_available_sessions.size() != server_session_num)
			{
				LOG_ERROR << "m_available_sessions push failed";
				return false;
			}

			m_acceptor_ptr = new cacceptor(m_io_service);
			if (!m_acceptor_ptr)
			{
				LOG_ERROR << "new wacceptor failed";
				return false;
			}
		}

		//ï¿½Í»ï¿½ï¿½Ë³ï¿½Ê¼ï¿½ï¿½
		if (client_session > 0)
		{
			m_paras.reserve(client_session);

			for (int32 i = 0; i < client_session; ++i)
			{
				m_paras.emplace_back(new cconnect_para(m_io_service));
			}

			if (m_paras.size() != client_session)
			{
				LOG_ERROR << "new socket failed";
				return false;
			}
		}

		m_msgs = new cnet_msg_queue();
		if (!m_msgs)
		{
			LOG_ERROR << "new wnet_msg_queue failed";
			return false;
		}
		m_name = name;
		return true;
	}
	void cnet_mgr::shutdown()
	{
		m_shuting.store(true);
		if (m_acceptor_ptr)
		{
			if (m_acceptor_ptr->is_open())
			{
				cerror_code ec;
				m_acceptor_ptr->cancel(ec);
				m_acceptor_ptr->close(ec);
			}
			delete m_acceptor_ptr;
			m_acceptor_ptr = nullptr;
		}

		if (!m_io_service.stopped())
		{
			// ï¿½Ø±Õ¹ï¿½ï¿½ï¿½ï¿½ß³ï¿½
			m_io_service.stop();
		}

		for (std::thread& thread : m_threads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
		m_threads.clear();
	}
	void cnet_mgr::destroy()
	{
		for (cnet_session* session : m_sessions)
		{
			session->destroy();
			delete session;
		}
		m_sessions.clear();
		m_available_sessions.clear();

		for (cconnect_para* para : m_paras)
		{
			delete para;
		}
		m_paras.clear();

		if (m_msgs)
		{
			cnet_msg *p = m_msgs->dequeue();
			cnet_msg* tmp = nullptr;
			while (p)
			{
				tmp = p;
				p = p->get_next_msg();
				m_pool_ptr->free(tmp);
			}

			delete m_msgs;
			m_msgs = nullptr;
		}

		if (m_pool_ptr)
		{
			m_pool_ptr->destroy();
			delete m_pool_ptr;
			m_pool_ptr = nullptr;
		}
	}
	bool cnet_mgr::startup(uint32 thread_num, const char * ip, uint16 port)
	{
		m_listening_max = thread_num;
		if (!m_available_sessions.empty())
		{
			//has server
			if (!_start_listen(ip, port, thread_num))
			{
				return false;
			}
		}

		
		for (uint32 i = 0; i < thread_num; ++i)
		{
			m_threads.emplace_back(&cnet_mgr::_worker_thread, this);
		}
		return true;
	}
	bool cnet_mgr::connect_to(uint32 index, const std::string & ip_address, uint16 port)
	{
		if (index >= static_cast<uint32>(m_paras.size()))
		{
			LOG_ERROR << "invalid index " << index;
			return false;
		}
		cconnect_para* para_ptr = m_paras[index];
		if (para_ptr->busy)
		{
			LOG_ERROR << "index " << index << " busy";
			return false;
		}

		cnet_session* session_ptr = m_sessions[index];
		if (!session_ptr->is_status_init())
		{
			LOG_ERROR << "session status is not init, index=" << index;
			return false;
		}

		para_ptr->ip.assign(ip_address);
		para_ptr->port = port;
		para_ptr->busy.store(true);

		cendpoint endpoint = cendpoint(boost::asio::ip::address::from_string(ip_address), port);

		session_ptr->set_connect_status();
		session_ptr->get_socket().async_connect(endpoint,
			boost::bind(&cnet_mgr::_handle_connect, this, boost::asio::placeholders::error, index));

		return true;
	}
	/*void cnet_mgr::set_times_second(uint32 msleep)
	{
	}
	void cnet_mgr::set_reconnection(uint32 msleep)
	{
	}*/
	void cnet_mgr::process_msg()
	{
		cnet_msg *msg_ptr = m_msgs->dequeue();

		while (msg_ptr)
		{
			if (msg_ptr->get_msg_id() == EMIR_Connect)
			{
				cmsg_connect* p = (cmsg_connect*)msg_ptr->get_buf();
				m_connect_callback(msg_ptr->get_session_id(), p->para, p->buf);
			}
			else if (msg_ptr->get_msg_id() == EMIR_Disconnect)
			{
				m_disconnect_callback(msg_ptr->get_session_id());
			}
			else
			{
				m_msg_callback(msg_ptr->get_session_id(), msg_ptr->get_msg_id()
					, msg_ptr->get_buf(), msg_ptr->get_size());
			}
			cnet_msg *tmp_msg = msg_ptr;
			msg_ptr = msg_ptr->get_next_msg();
			tmp_msg->free_me();
		}
	}
	bool cnet_mgr::transfer_msg(uint32 sessionId, cnet_msg & msg)
	{
		return false;
	}
	bool cnet_mgr::send_msg(uint32 sessionId, uint16 msg_id, const void * msg_ptr, uint32 msg_size)
	{
		const uint32 index = get_session_index(sessionId);
		if (index >= m_sessions.size())
		{
			return false;
		}

		return m_sessions[index]->send_msg(sessionId, msg_id, msg_ptr, msg_size);
	}
	bool cnet_mgr::send_msg(uint32 sessionId, uint16 msg_id, const void * msg_ptr, uint32 msg_size, int32 extra)
	{
		return false;
	}
	void cnet_mgr::close(uint32 session_id)
	{
		const uint32 index = get_session_index(session_id);
		if (index >= m_sessions.size())
		{
			return;
		}

		return m_sessions[index]->close(session_id);
	}
	void cnet_mgr::show_info()
	{
		LOG_INFO << m_name << " info";
		LOG_INFO << "m_listening_num=" << m_listening_num;
		{
			clock_guard guard(m_avail_session_mutex);
			LOG_INFO << "available_sessions=" << m_available_sessions.size();
		}
		if (m_pool_ptr)
		{
			m_pool_ptr->show_info();
		}
	}
	void cnet_mgr::msg_push(cnet_msg * msg_ptr)
	{
		m_msgs->enqueue(msg_ptr);
	}
	void cnet_mgr::post_disconnect(cnet_session * session_ptr)
	{
		m_io_service.post(boost::bind(&cnet_mgr::_handle_close, this, session_ptr));
	}
	void cnet_mgr::_worker_thread()
	{
		for (;;)
		{
			try
			{
				m_io_service.run();
				LOG_INFO << m_name << " thread exit";
				break;
			}
			catch (boost::system::system_error& e)
			{
				LOG_ERROR << "work thread exception, code = " << e.code().value();
			}
		}
	}
	/*void cnet_mgr::_clearup_session(cnet_session * psession)
	{
	}
	void cnet_mgr::_new_connect()
	{
	}*/
	bool cnet_mgr::_start_listen(const std::string & ip, uint16 port, uint32 thread_num)
	{
		cendpoint endpoint(boost::asio::ip::address::from_string(ip), port);

		cerror_code ec;
		m_acceptor_ptr->open(endpoint.protocol(), ec);

		if (ec)
		{
			LOG_ERROR << "open socket error " << ec.value();
			return false;
		}

				
		{
			m_acceptor_ptr->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);

			if (ec)
			{
				LOG_ERROR << "set_option reuse_address error, " << ec.value();
				return false;
			}
		}

		m_acceptor_ptr->bind(endpoint, ec);

		if (ec)
		{
			LOG_ERROR << "bind error, " << ec.value();
			return false;
		}

		m_acceptor_ptr->listen(boost::asio::socket_base::max_connections, ec);

		if (ec)
		{
			LOG_ERROR << "listen error, " << ec.value() << ", port = " << port;
			return false;
		}

		for (uint32 i = 0; i < thread_num; ++i)
		{
			_post_accept();
		}

		LOG_INFO << "listen on ip=" << ip << " port=" << port;
		return true;
	}
	void cnet_mgr::_post_accept()
	{
		if (m_listening_num >= m_listening_max || m_shuting)
		{
			return;
		}

		cnet_session* session_ptr = _get_available_session();
		if (!session_ptr)
		{
			return;
		}

		++m_listening_num;

		session_ptr->set_accept_status();

		m_acceptor_ptr->async_accept(session_ptr->get_socket()
			, boost::bind(&cnet_mgr::_handle_accept, this, boost::asio::placeholders::error, session_ptr)
		);
	}
	void cnet_mgr::_handle_accept(const cerror_code & error, cnet_session * session_ptr)
	{
		--m_listening_num;
		cnet_msg* msg_ptr = NULL;
		if (!error)
		{
			msg_ptr = _create_accept_msg(session_ptr->get_socket());
		}

		if (!msg_ptr)
		{
			session_ptr->set_status_init();
			if (session_ptr->get_socket().is_open())
			{
				cerror_code ec;
				session_ptr->get_socket().close(ec);
			}
			_return_session(session_ptr);
			_post_accept();
		}
		else
		{
			_post_accept();
			session_ptr->connected(msg_ptr);
		}
	}
	cnet_msg * cnet_mgr::_create_accept_msg(const csocket_type & socket)
	{
		cerror_code ec;
		cendpoint remote_endpoint = socket.remote_endpoint(ec);

		std::string address = remote_endpoint.address().to_string(ec);
		const uint32 msg_size = static_cast<uint32>(sizeof(cmsg_connect) + address.length());

		//new msg
		cnet_msg* msg_ptr = create_msg(EMIR_Connect, msg_size);
		if (!msg_ptr)
		{
			LOG_ERROR << "alloc msg EMIR_ListenConnect failed";
			return NULL;
		}

		cmsg_connect* connect_msg_ptr = (cmsg_connect*)(msg_ptr->get_buf());
		
		connect_msg_ptr->para = static_cast<uint32>(address.length());
		memcpy(connect_msg_ptr->buf, address.data(), address.length());
		connect_msg_ptr->buf[address.length()] = 0;
		return msg_ptr;
	}
	/*void cnet_mgr::_listen_start(const char * ip, uint16 port)
	{
	}*/
	bool cnet_mgr::_connect_start(uint32 index, const char * ip, uint16 port)
	{
		return false;
	}
	void cnet_mgr::_handle_connect(const cerror_code & error, uint32 index)
	{
		cconnect_para* para_ptr = m_paras[index];
		cnet_session* session_ptr = m_sessions[index];

		if (error)
		{
			if (session_ptr->get_socket().is_open())
			{
				cerror_code ec;
				session_ptr->get_socket().close(ec);
			}

			if (m_reconnect_second != 0)
			{
				LOG_WARN << "connect to " << para_ptr->ip << ":" << para_ptr->port
					<< " failed, code=" << error.value() << ", reconnect " << m_reconnect_second << " second later";

				para_ptr->timer.expires_from_now(boost::posix_time::seconds(m_reconnect_second));
				para_ptr->timer.async_wait(boost::bind(&cnet_mgr::_handle_reconnect, this, index));
				return;
			}
			cnet_msg* msg_ptr = _create_connect_msg(error.value());
			if (!msg_ptr)
			{
				LOG_WARN << "connect to " << para_ptr->ip << ":" << para_ptr->port
					<< " failed, memory not enough, retry 5 second later";
				para_ptr->timer.expires_from_now(boost::posix_time::seconds(5));
				para_ptr->timer.async_wait(boost::bind(&cnet_mgr::_handle_reconnect, this, index));
				return;
			}

			para_ptr->busy.store(false);
			session_ptr->set_status_init();
			msg_push(msg_ptr);
		}
		else
		{
			cnet_msg* msg_ptr = _create_connect_msg(0);
			if (!msg_ptr)
			{
				if (session_ptr->get_socket().is_open())
				{
					cerror_code ec;
					session_ptr->get_socket().close(ec);
				}
				LOG_WARN << "connect to " << para_ptr->ip << ":" << para_ptr->port
					<< " success, but memory not enough, retry 5 second later";
				para_ptr->timer.expires_from_now(boost::posix_time::seconds(5));
				para_ptr->timer.async_wait(boost::bind(&cnet_mgr::_handle_reconnect, this, index));
				return;
			}
			para_ptr->busy.store(false);
			session_ptr->connected(msg_ptr);
		}
	}
	void cnet_mgr::_handle_reconnect(uint32 index)
	{
		cconnect_para* para_ptr = m_paras[index];
		cnet_session* session_ptr = m_sessions[index];

		cendpoint endpoint = cendpoint(boost::asio::ip::address::from_string(para_ptr->ip), para_ptr->port);

		session_ptr->get_socket().async_connect(endpoint,
			boost::bind(&cnet_mgr::_handle_connect, this, boost::asio::placeholders::error, index));
	}
	cnet_msg * cnet_mgr::_create_connect_msg(uint32 error_code)
	{
		const uint32 msg_size = sizeof(cmsg_connect);

		cnet_msg* msg_ptr = create_msg(EMIR_Connect, msg_size);
		if (!msg_ptr)
		{
			LOG_ERROR << "alloc msg EMIR_ConnectTo failed";
			return NULL;
		}

		cmsg_connect* connect_msg_ptr = (cmsg_connect*)(msg_ptr->get_buf());
		connect_msg_ptr->para = error_code;
		connect_msg_ptr->buf[0] = 0;
		return msg_ptr;
	}
	cnet_msg * cnet_mgr::create_msg(uint16 msg_id, uint32 msg_size)
	{
		return  cnet_msg::alloc_me(m_pool_ptr, msg_id, msg_size);
	}
	cnet_session * cnet_mgr::_get_available_session()
	{
		cnet_session* temp_ptr = NULL;
		{
			clock_guard guard(m_avail_session_mutex);
			if (m_available_sessions.empty())
			{
				return NULL;
			}
			temp_ptr = m_available_sessions.back();
			m_available_sessions.pop_back();
		}
		return temp_ptr;
	}
	void cnet_mgr::_return_session(cnet_session * session_ptr)
	{
		clock_guard guard(m_avail_session_mutex);
		m_available_sessions.push_back(session_ptr);
	}

	void cnet_mgr::_handle_close(cnet_session * session_ptr)
	{
		cnet_msg* msg_ptr = create_msg(EMIR_Disconnect, 0);
		if (!msg_ptr)
		{
			LOG_ERROR << "alloc msg EMIR_Disconnect failed, name=" << get_name();
			post_disconnect(session_ptr);
			return;
		}
		const uint32 session_id = session_ptr->get_session_id();
		msg_ptr->set_session_id(session_id);
		session_ptr->reset();
		msg_push(msg_ptr);
		const uint32 index = get_session_index(session_id);
		if (_is_server_index(index))
		{
			_return_session(session_ptr);
			_post_accept();
		}
		else if (m_reconnect_second > 0)
		{
			//ï¿½ï¿½ï¿½ï¿½
			cconnect_para* para_ptr = m_paras[index];
			para_ptr->busy.store(true);
			session_ptr->set_connect_status();
			para_ptr->timer.expires_from_now(boost::posix_time::seconds(m_reconnect_second));
			para_ptr->timer.async_wait(boost::bind(&cnet_mgr::_handle_reconnect, this, index));
		}
	}

	bool cnet_mgr::_is_server_index(uint32 index) const
	{
		return (index >= static_cast<uint32>(m_paras.size()));
	}
	
} // !namespace chen