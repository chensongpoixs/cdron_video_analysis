/***********************************************************************************************
created: 		2019-03-01

author:			chensong

purpose:		net_session


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
#include "cnet_session.h"
#include "clog.h"
#include "cnet_msg.h"
#include "cnet_mgr.h"
#include "cnet_msg_header.h"
#include "cnet_msg_reserve.h"
#include <boost/bind.hpp>
#include <cassert>
#include <assert.h>
namespace chen {
	cnet_session::cnet_session(cservice &service, cnet_mgr & ptr, uint32 session_id)
		: m_session_mgr(ptr)
		, m_session_id(0X00010000 | session_id)
		, m_status(ENSS_None)
		, m_socket(service)
		, m_reading_atomic(false)
		, m_writeing_atomic(false)
		, m_recv_buf_ptr(NULL)
		, m_recv_ptr(NULL)
		, m_recv_size(0)
		, m_wait_send_head_ptr(NULL)
		, m_wait_send_tail_ptr(NULL)
		, m_send_buf_ptr(NULL)
		, m_sending_buf_ptr(NULL)
		, m_send_size(0)
	{
	}
	cnet_session::~cnet_session()
	{
	}
	bool cnet_session::init(uint32 send_buff_size, uint32 recv_buff_size)
	{
		m_recv_buf_ptr = cmsg_receive_buffer::alloc_me(recv_buff_size);
		if (!m_recv_buf_ptr)
		{
			ERROR_EX_LOG("alloc m_recv_buf_ptr memory error!");
			return false;
		}
		m_sending_buf_ptr = cmsg_send_buffer::alloc_me(send_buff_size);
		if (!m_sending_buf_ptr)
		{
			ERROR_EX_LOG("alloc sending_buff_size memory error!");
			return false;
		}
		m_send_buf_ptr = cmsg_send_buffer::alloc_me(send_buff_size);
		if (!m_sending_buf_ptr)
		{
			ERROR_EX_LOG("alloc send_buff_size memory error!");
			return false;
		}
		m_status = ENSS_Init;
		return true;
	}
	void cnet_session::destroy()
	{
		m_status = ENSS_None;
		if (m_recv_buf_ptr)
		{
			cmsg_receive_buffer::free_me(m_recv_buf_ptr);
			m_recv_buf_ptr = NULL;
		}
		if (m_recv_ptr)
		{
			m_recv_ptr->free_me();
			m_recv_ptr = NULL;
		}
		
		if (m_send_buf_ptr)
		{
			cmsg_send_buffer::free_me(m_send_buf_ptr);
			m_send_buf_ptr = NULL;
		}
		if (m_sending_buf_ptr)
		{
			cmsg_send_buffer::free_me(m_sending_buf_ptr);
			m_sending_buf_ptr = NULL;
		}

		if (m_wait_send_tail_ptr)
		{
			_free_wait_queue_msg();
		}
	}
	void cnet_session::set_status_init()
	{
		if (m_status != ENSS_Accept && m_status != ENSS_ConnectTo)
		{
			LOG_ERROR << "status is " << m_status << ", name=" << m_session_mgr.get_name();
		}
		m_status = ENSS_Init;
	}
	void cnet_session::set_accept_status()
	{
		if (ENSS_Init != m_status)
		{
			LOG_ERROR << "status is not ENSS_Init,  name =" << m_session_mgr.get_name();
		}
		m_status = ENSS_Accept;
	}
	void cnet_session::set_connect_status()
	{
		if (ENSS_Init != m_status)
		{
			LOG_ERROR << "status is not ENSS_Init,  name =" << m_session_mgr.get_name();
		}
		
		m_status = ENSS_ConnectTo;
	}
	void cnet_session::connected(cnet_msg * msg)
	{
		if (m_status != ENSS_Accept && m_status != ENSS_ConnectTo)
		{
			LOG_ERROR << "status is " << m_status << ", name=" << m_session_mgr.get_name();
		}
		m_status = ENSS_Open;
		m_reading_atomic.store(true);
		msg->set_session_id(m_session_id);
		m_session_mgr.msg_push(msg);
		
		m_socket.async_read_some(boost::asio::buffer(/*m_buf, 1024*/
			m_recv_buf_ptr->get_buf(), m_recv_buf_ptr->max_size()),
			boost::bind( &cnet_session::_handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	void cnet_session::reset()
	{
		if (m_status != ENSS_Close)
		{
			LOG_ERROR << "status is not ENSS_CLOSE, name=" << m_session_mgr.get_name();
			return;
		}
		if ((m_session_id & 0XFFFF0000) == 0XFFFF0000)
		{
			m_session_id += 0X00020000;
		}
		else
		{
			m_session_id += 0X00010000;
		}

		m_reading_atomic = false;
		m_writeing_atomic = false;

		m_recv_size = 0;

		if (m_recv_ptr)
		{
			m_recv_ptr->free_me();
			m_recv_ptr = NULL;
		}

		if (m_send_buf_ptr)
		{
			m_send_buf_ptr->reset();
		}

		if (m_sending_buf_ptr)
		{
			m_sending_buf_ptr->reset();
		}

		if (m_wait_send_head_ptr)
		{
			_free_wait_queue_msg();
		}

		m_status = ENSS_Init;
	}
	bool cnet_session::process_msg()
	{
		return true;
	}
	
	
	bool cnet_session::send_msg(uint32 session_id, uint16 msg_id, const void * msg_ptr, uint32 msg_size)
	{
		if (m_session_id != session_id || 0 == msg_id || ENSS_Open != m_status)
		{
			return false;
		}
		const uint16 code_msg_id = msg_id ^ m_session_mgr.get_msg_id_key();
		const uint32 code_msg_size = msg_size ^ m_session_mgr.get_msg_size_key();

		{
			//ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ì²½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ÝµÄ°ï¿½
			clock_guard lock(m_mutex);
			if (m_session_id != session_id  || ENSS_Open != m_status )
			{
				return false;
			}
			if (m_wait_send_head_ptr)
			{
				return push_queue_msg(msg_id, msg_size, msg_ptr);
			}

			if (m_send_buf_ptr->avail() >= sizeof(MSG_HEADER_SIZE))
			{
				m_send_buf_ptr->append(&code_msg_id, sizeof(code_msg_id));
				m_send_buf_ptr->append(&code_msg_size, sizeof(code_msg_size));
				uint32 write_byte = m_send_buf_ptr->append(msg_ptr, msg_size);
				//ï¿½Ð¶ï¿½ï¿½Ç·ï¿½È«ï¿½ï¿½Ð´ï¿½ëµ½ï¿½ï¿½ï¿½Í»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
				if (write_byte < msg_size)
				{
					if (!_push_to_queue((char*)msg_ptr + write_byte, msg_size - write_byte, msg_id))
					{
						m_send_buf_ptr->deducted(MSG_HEADER_SIZE + write_byte);
						return false;
					}
					m_send_size = MSG_HEADER_SIZE;
				}
			}
			else if (!_push_to_queue(msg_ptr, msg_id, msg_size))
			{
				return false;
			}
			bool write_bool = false;

			if (!m_writeing_atomic.compare_exchange_strong(write_bool, true))
			{
				return true;
			}
			std::swap(m_sending_buf_ptr, m_send_buf_ptr);
		}

		
		m_socket.async_write_some(boost::asio::buffer(
		m_sending_buf_ptr->get_buf(),m_sending_buf_ptr->size()), 
		boost::bind(&cnet_session::_handle_write, this,
			boost::asio::placeholders::error, 
			boost::asio::placeholders::bytes_transferred));

		return true;
	}
	
	bool cnet_session::push_queue_msg(uint16 msg_id, uint32 msg_size, const void * msg_ptr)
	{
		cnet_msg *p = cnet_msg::alloc_me(m_session_mgr.get_pool(), msg_id, msg_size);
		if (!p)
		{
			return false;
		}

		p->fill(msg_ptr, msg_size);
		//p->add_referces();
		_push_queue_msg(p);
		return true;
	}
	void cnet_session::close(uint32 session_id)
	{
		if (ENSS_Open != m_status || m_session_id != session_id)
		{
			return ;
		}
		{
			clock_guard lock(m_mutex);
			if (ENSS_Open != m_status || m_session_id != session_id)
			{
				return ;
			}
			m_status = ENSS_Shut;
			boost::system::error_code ec;
			m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			//m_socket.close(ec);
		}
		_release();
		
	}
	void cnet_session::_handle_read(const boost::system::error_code & error, size_t bytes_transferred)
	{
		if (error || bytes_transferred == 0)
		{
			/*SYSTEMTIME st = {0};
			GetLocalTime(&st);  //ï¿½ï¿½È¡ï¿½ï¿½Ç°Ê±ï¿½ï¿½ ï¿½É¾ï¿½È·ï¿½ï¿½ms
			char buffer[1024] = {0};
			sprintf(buffer, "%d-%02d-%02d %02d:%02d:%02d:%02d\n",
				st.wYear,
				st.wMonth,
				st.wDay,
				st.wHour,
				st.wMinute,
				st.wSecond,
				st.wMilliseconds);
				*/
			 
			m_reading_atomic.store(false);
			//LOG_ERROR << "_handle_read =" << error.value() << ", error.message() = " << error.message() << ", bytes_transferred= " << bytes_transferred << ", buffer= " << buffer;
			_close();
			return;
		}

		if (m_status != ENSS_Open)
		{
			m_reading_atomic.store(false);
			_close();
			return;
		}
		m_recv_size += static_cast<uint32>(bytes_transferred);
		
		if (m_recv_ptr)
		{
			if (m_recv_ptr->get_size() <  m_recv_size)
			{
				LOG_ERROR << "recv size too big, recv size=" << m_recv_size
					<< ", expect_size=" << m_recv_ptr->get_size()
					<< ", name=" << m_session_mgr.get_name();
				m_reading_atomic.store(false);
				_close();
				return;
			}
			else if(m_recv_ptr->get_size() == m_recv_size)
			{
				m_session_mgr.msg_push(m_recv_ptr);
				m_recv_ptr = NULL;
				m_recv_size = 0;
			}
		}
		else if (MSG_HEADER_SIZE <= m_recv_size)
		{
			if (!_parse_msg())
			{
				m_reading_atomic.store(false);
				_close();
				return;
			}
		}
		
		if (m_recv_ptr)
		{
			assert(m_recv_ptr->get_size() > m_recv_size);

			m_socket.async_read_some(
				boost::asio::buffer(m_recv_ptr->get_buf() + m_recv_ptr->get_data_size() + m_recv_size, m_recv_ptr->get_size() - m_recv_ptr->get_data_size() - m_recv_size),
				boost::bind(&cnet_session::_handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}
		if (m_recv_buf_ptr)
		{
			assert(m_recv_buf_ptr->max_size() > m_recv_size);
			m_socket.async_read_some(boost::asio::buffer(
			m_recv_buf_ptr->get_buf()+ m_recv_buf_ptr->get_size() + m_recv_size, m_recv_buf_ptr->avail()- m_recv_size
			), boost::bind(&cnet_session::_handle_read, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
		}

	}
	void cnet_session::_handle_write(const boost::system::error_code & error, size_t bytes_transferred)
	{
		if (error || bytes_transferred == 0)
		{
			//LOG_ERROR << "_handle_write = " << error.value() << ", error.message() = " << error.message() ;;
			m_writeing_atomic.store(false);
			_close();
			return;
		}

		if (m_status != ENSS_Open)
		{
			//LOG_ERROR << "_handle_write";
			m_writeing_atomic.store(false);
			_close();
			return;
		}

		const uint32 size = static_cast<uint32>(bytes_transferred);
		m_sending_buf_ptr->set_send_size(size);
		if (m_sending_buf_ptr->get_send_size() == m_sending_buf_ptr->size())
		{
			m_sending_buf_ptr->reset();
			{
				clock_guard lock(m_mutex);
				if (!m_send_buf_ptr->empty())
				{
					//_msg_queue_buf();
					std::swap(m_sending_buf_ptr, m_send_buf_ptr);
				}
				else if (m_wait_send_head_ptr)
				{
					_msg_queue_buf();
				}
			}
			if (m_sending_buf_ptr->empty())
			{
				m_writeing_atomic.store(false);
				if (ENSS_Open != m_status)
				{
					LOG_ERROR << "_handle_write";
					_release();
				}
				return;
			}
		}
		//LOG_ERROR << "_handle_write m_sending_buf_ptr->get_send_size() = " << m_sending_buf_ptr->get_send_size() << ", m_sending_buf_ptr->size() = " << m_sending_buf_ptr->size();
		m_socket.async_write_some(boost::asio::buffer(
		m_sending_buf_ptr->get_buf() + m_sending_buf_ptr->get_send_size(), 
		m_sending_buf_ptr->size() - m_sending_buf_ptr->get_send_size()), boost::bind(&cnet_session::_handle_write, this, 
			boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
	}
	void cnet_session::_close()
	{
		 
		{
			clock_guard lock(m_mutex);
			
			if (ENSS_Open == m_status)
			{
				m_status = ENSS_Shut;
				boost::system::error_code ec;
				m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			}
		}
		_release();
	}
	void cnet_session::_release()
	{
		/*if (m_status != ESS_SHUT)
		{
			LOG_ERROR << "release status = " << m_status;
		}*/
		clock_guard lock(m_mutex);
		if (ENSS_Shut != m_status || m_writeing_atomic || m_reading_atomic)
		{
			return;
		}
		m_status = ENSS_Close;
		if (m_socket.is_open())
		{
			boost::system::error_code ec;
			m_socket.close( ec);
		}
		m_session_mgr.post_disconnect(this);;
	}
	bool cnet_session::_push_to_queue(const void * msg_ptr, uint32 msg_size, uint16 msg_id)
	{
		cnet_msg* p = cnet_msg::alloc_me(m_session_mgr.get_pool(), msg_id, msg_size);
		if (!p)
		{
			LOG_ERROR << "alloc failed, name=" << m_session_mgr.get_name();
			return false;
		}

		p->fill(msg_ptr, msg_size);

		_push_queue_msg(p);
		return true;
	}
	void cnet_session::_push_queue_msg(cnet_msg * msg_ptr)
	{
		if (!m_wait_send_tail_ptr)
		{
			m_wait_send_head_ptr = m_wait_send_tail_ptr = msg_ptr;
		}
		else
		{
			m_wait_send_tail_ptr->set_next_msg(msg_ptr);
			m_wait_send_tail_ptr = msg_ptr;
		}
	}
	void cnet_session::_msg_queue_buf()
	{
		uint16 code_msg_id = 0;
		uint32 code_msg_size = 0;
		uint32 remain_size = 0;
		//cnet_msg * ptr = m_wait_send_head_ptr;
		while (m_wait_send_head_ptr)
		{
			if (m_send_size < MSG_ID_SIZE)
			{
				/*const uint16*/ code_msg_id = m_wait_send_head_ptr->get_msg_id() ^ m_session_mgr.get_msg_id_key();
				remain_size = MSG_ID_SIZE - m_send_size;
				m_send_size += m_sending_buf_ptr->append((char *)(&code_msg_id + m_send_size), remain_size);
				if (m_send_size < MSG_ID_SIZE)
				{
					break;
				}
			}
			if (m_send_size < MSG_HEADER_SIZE)
			{
				/*const uint32*/ code_msg_size = m_wait_send_head_ptr->get_size() ^ m_session_mgr.get_msg_size_key();
				remain_size = MSG_HEADER_SIZE - m_send_size;
				m_send_size += m_sending_buf_ptr->append((char *)(&code_msg_size + m_send_size - MSG_ID_SIZE), remain_size);
				if (m_send_size < MSG_HEADER_SIZE)
				{
					break;
				}
			}
			remain_size = m_wait_send_head_ptr->get_size() - (m_send_size - MSG_HEADER_SIZE);
			if (remain_size > 0)
			{
				m_send_size += m_sending_buf_ptr->append(m_wait_send_head_ptr->get_buf() + (m_send_size - MSG_HEADER_SIZE), 
					remain_size);
			}
			
			if (m_send_size  < m_wait_send_head_ptr->get_size() + MSG_HEADER_SIZE)
			{
				break;
			}
			m_send_size = 0;
			cnet_msg *q = m_wait_send_head_ptr;
			m_wait_send_head_ptr = m_wait_send_head_ptr->get_next_msg();
			q->sub_referces();
			
		}
		if (!m_wait_send_head_ptr)
		{
			m_wait_send_tail_ptr = NULL;
		}
	
	}
	void cnet_session::_free_wait_queue_msg()
	{
		cnet_msg *p = m_wait_send_tail_ptr;
		m_wait_send_tail_ptr = NULL;
		m_wait_send_head_ptr = NULL;
		while (p)
		{
			cnet_msg *q = p;
			p = q->get_next_msg();
			q->free_me();
		}
		m_send_size = 0;
	}
	bool cnet_session::_parse_msg()
	{
		const cmsg_header*	header_ptr;
		uint16			msg_id;
		uint32			msg_size;
		const char * buf = m_recv_buf_ptr->get_buf();
		while (true)
		{
			header_ptr = reinterpret_cast<const cmsg_header*>(buf);
			msg_id = header_ptr->m_msg_id ^ m_session_mgr.get_msg_id_key();
			if (msg_id < EMIR_Max)
			{
				LOG_ERROR << "alloc msg failed, msg_id=" << msg_id
					<< ", name=" << m_session_mgr.get_name();
				return false;
			}
			msg_size = header_ptr->m_msg_size ^ m_session_mgr.get_msg_size_key();
			if (msg_size > m_session_mgr.get_max_msg_size())
			{
				if (m_session_mgr.is_wan())
				{
					LOG_ERROR << "msg size too big, msg_id=" << msg_id << ", msg_size=" << msg_size
						<< ", max_size =" << m_session_mgr.get_max_msg_size()
						<< ", name=" << m_session_mgr.get_name();
					return false;
				}
				else
				{
					LOG_WARN << "msg size too big, msg_id=" << msg_id << ", msg_size=" << msg_size
						<< ", max_size =" << m_session_mgr.get_max_msg_size()
						<< ", name=" << m_session_mgr.get_name();
				}
			}
			m_recv_ptr = cnet_msg::alloc_me(m_session_mgr.get_pool(), msg_id, msg_size);
			if (!m_recv_ptr)
			{
				LOG_ERROR << "alloc msg failed, msg_id=" << msg_id << ", msg_size=" << msg_size
					<< ", name=" << m_session_mgr.get_name();
				return false;
			}
			m_recv_ptr->set_session_id(m_session_id);
			buf += MSG_HEADER_SIZE;
			m_recv_size -= MSG_HEADER_SIZE;
			if (m_recv_size >= msg_size)
			{
				//Ò»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ï¢
				if (msg_size > 0)
				{
					m_recv_ptr->fill(buf, msg_size);
					buf += msg_size;
					m_recv_size -= msg_size;
				}

			
				m_session_mgr.msg_push(m_recv_ptr);
				m_recv_ptr = NULL;

				if (m_recv_size < MSG_HEADER_SIZE)
				{
					memmove(m_recv_buf_ptr->get_buf(), buf, m_recv_size);
					return true;
				}
			}
			else
			{
				// ï¿½ï¿½ï¿½ï¿½Ò»ï¿½ï¿½ï¿½ï¿½Ï¢
				m_recv_ptr->fill(buf, m_recv_size);
				return true;
			}
		}
		return true;
	}
} // !namespace chen