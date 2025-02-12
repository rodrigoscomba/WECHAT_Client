#pragma once

#include "includes.h"

struct Message
{
	uint8_t type;
	uint8_t id;
	uint8_t gid;
	uint8_t body_size;
	std::string msg;
};

class net_client
{
public:
	net_client(std::deque<std::pair<uint8_t, std::string>>& v_msgsToPrint);
	virtual ~net_client();

public:
	void Connect(const std::string& host, const uint16_t port);
	void GrabSomeData(uint8_t& type, uint8_t& id, uint8_t& gid, uint8_t& body_size);
	void ReadBody(uint8_t& type, uint8_t& id, uint8_t& gid, uint8_t& size);
	void StopContextThread();
	void Sendmsg(std::string& msg);
	uint8_t GetID();
	void SetID(uint8_t ID);
	void Read(std::vector<uint8_t>& vector, uint8_t size);
	void SetGrabBool(bool value);
	void RunContextThread();
	void PushToMsgsToPrint(uint8_t id, std::string& msg);
	void PushToUpdates(uint8_t Ctype, uint8_t Cid, uint8_t Cgid, uint8_t Cbody_size, std::string& msg);
	bool UpdateIsEmpty();
	Message getUpdateMsg();
	bool IsConnected();

	//uint8_t InState();
protected:
	asio::io_context m_context;
	std::thread m_thrContext;
	asio::ip::tcp::socket m_socket;
	uint8_t m_ID;
	std::vector<uint8_t> m_Header;
	std::vector<uint8_t> m_Body;
	std::vector<uint8_t> m_msg; //header+body
	std::deque<std::pair<uint8_t, std::string>>& v_msgsToPrint;
	std::deque<Message> v_msgsToUpdate;
private:
	bool m_grab = 0;
};
