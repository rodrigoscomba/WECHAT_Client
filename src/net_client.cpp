#include "net_client.h"

net_client::net_client(std::deque<std::pair<uint8_t, std::string>>& v_msgsToPrint)
	: v_msgsToPrint(v_msgsToPrint), m_socket(m_context)
{
}

net_client::~net_client()
{
	StopContextThread();
}

//Connection
void net_client::Connect(const std::string& host, const uint16_t port)
{
	asio::error_code ec;
	asio::ip::tcp::endpoint endpoint(asio::ip::make_address(host, ec), port);
	m_socket.connect(endpoint, ec);
	if (ec) {
		std::cout << "ERROR CONNECTING TO SERVER: " << ec.message() << "\n\n";
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(3000ms);
		std::cout << "EXITING APP, CHECK INTERNET CONNECTION AND TRY AGAIN LATER\n\n";
		std::this_thread::sleep_for(3000ms);
		exit(0);
	}
		
}

void net_client::RunContextThread()
{
	m_thrContext = std::thread([&]() { m_context.run(); });
}

void net_client::GrabSomeData(uint8_t& Ctype, uint8_t& Cid, uint8_t& Cgid, uint8_t& Cbody_size)
{
	if (m_grab && m_socket.is_open())
	{
		m_Header.resize(0);
		m_Header.resize(4);

		asio::async_read(m_socket, asio::buffer(m_Header, sizeof(uint32_t)),
			[this, &Ctype, &Cid, &Cgid, &Cbody_size](std::error_code ec, std::size_t length)
			{
				if (!ec)
				{
					Ctype = m_Header[0];
					Cid = m_Header[1];
					Cgid = m_Header[2];
					Cbody_size = m_Header[3];
					if ((Ctype == 'F' || Ctype == 'G') && Cbody_size == 0)
					{
						std::string empty = "";
						PushToUpdates(Ctype, 0, 0, 0, empty);
					}
						
					m_Header.erase(m_Header.begin(), m_Header.end());
					m_Header.resize(0);
					m_Header.resize(500);
					if (Cbody_size > 0)
					{
						ReadBody(Ctype, Cid, Cgid, Cbody_size);
					}
					else
					{
						auto timer = std::make_shared<asio::steady_timer>(m_socket.get_executor());
						timer->expires_after(std::chrono::milliseconds(50)); // Adjust delay as needed
						timer->async_wait([this, &Ctype, &Cid, &Cgid, &Cbody_size, timer](std::error_code ec)
							{
								if (!ec)
								{
									GrabSomeData(Ctype, Cid, Cgid, Cbody_size);
								}
								else
								{
									std::cout << "[CLIENT] Timer Error: " << ec.message() << std::endl;
								}
							});
					}
						
				}
				else
				{
					std::cout << "[CLIENT] Async Read Error: " << ec.message() << std::endl;
				}
			});
	}
}
void net_client::ReadBody(uint8_t& Ctype, uint8_t& Cid, uint8_t& Cgid, uint8_t& Cbody_size)
{
	m_Body.resize(0);
	m_Body.resize(500);

	asio::async_read(m_socket, asio::buffer(m_Body.data(), Cbody_size),
		[this, &Ctype, &Cid, &Cgid, &Cbody_size](std::error_code ec, std::size_t length)
		{
			if (!ec)
			{
				std::string msg;
				for (int i = 0; i < Cbody_size; i++)
					msg.push_back(m_Body[i]);
				if(Ctype == 'S')
					PushToMsgsToPrint(Cid, msg);
				if (Ctype == 'F' || Ctype == 'G')
					PushToUpdates(Ctype, Cid, Cgid, Cbody_size, msg);
			}

			m_Body.erase(m_Body.begin(), m_Body.end());
			m_Body.resize(0);
			m_Body.resize(500);
			auto timer = std::make_shared<asio::steady_timer>(m_socket.get_executor());
			timer->expires_after(std::chrono::milliseconds(50)); // Adjust delay as needed
			timer->async_wait([this, &Ctype, &Cid, &Cgid, &Cbody_size, timer](std::error_code ec)
				{
					if (!ec)
					{
						GrabSomeData(Ctype, Cid, Cgid, Cbody_size);
					}
					else
					{
						std::cout << "[CLIENT] Timer Error: " << ec.message() << std::endl;
					}
				});
			
		});
}

void net_client::StopContextThread()
{
	m_socket.release();
	m_context.stop();
	if (m_thrContext.joinable()) m_thrContext.join();
}

void net_client::Sendmsg(std::string& msg)
{
	asio::error_code ec;
	asio::write(m_socket, asio::buffer(msg, msg.size()), ec);
	if (ec) std::cout << "Failed to send msg : " << ec.message() << std::endl;
}

uint8_t net_client::GetID()
{
	return m_ID;
}

void net_client::SetID(uint8_t ID)
{
	m_ID = ID;
}

void net_client::Read(std::vector<uint8_t>& vector, uint8_t size)
{
	asio::error_code ec;
	asio::read(m_socket, asio::buffer(vector.data(), size), ec);
	if (ec) std::cout << "Failed to send msg : " << ec.message() << std::endl;
}

void net_client::SetGrabBool(bool value)
{
	m_grab = value;
}

void net_client::PushToMsgsToPrint(uint8_t id, std::string& msg)
{
	std::pair <uint8_t, std::string> msg_to_print(id, msg);
	v_msgsToPrint.push_back(msg_to_print);
}
void net_client::PushToUpdates(uint8_t Ctype, uint8_t Cid, uint8_t Cgid, uint8_t Cbody_size, std::string& msg)
{
	Message message{ Ctype, Cid, Cgid, Cbody_size, msg };
	v_msgsToUpdate.push_back(message);
}

bool net_client::UpdateIsEmpty()
{
	if (v_msgsToUpdate.empty()) return 1;
	else return 0;
}

Message net_client::getUpdateMsg()
{
	Message update = v_msgsToUpdate.front();
	v_msgsToUpdate.pop_front();
	return update;	
}
bool  net_client::IsConnected()
{
	if (m_socket.is_open())
	{
		return true;
	}
	else
		return false;
}
