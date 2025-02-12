#pragma once
#include "net_client.h"

enum STATE { Loading, NewUser, RefreshNewUser, LogIn, CreateAccount, RefreshInMainMenu, InMainMenu, EnterGroup, CreateGroup, AddFriend, InGroup, LeaveGroup, NoRefresh, RefreshChat};

struct FriendsRecord
{
	std::uint8_t fid;
	std::vector<uint8_t> Groups;
};

class ClientInterface
{
public:
	ClientInterface();

public:
	bool IsConnectedToServer();
	bool CreateAccount();
	void AddFriendsMENU(const std::string& myname);
	bool Login(uint8_t& myID, std::string& username);
	bool CreateGroup(uint8_t& myID, uint8_t& gid, std::string& group_name, std::string& myname);
	bool EnterGroup(uint8_t& groupnumber, std::string& groupname, std::uint8_t& myID);
	void UI_TopChat(const std::string& groupname);
	void Chat(const uint8_t& gID, const uint8_t& myID);
	void LeaveGroup(uint8_t& groupID, uint8_t& personalID);
	void UpdateFriends(uint8_t& myID);
	void UpdateGroups(uint8_t& myID);
	void UI_MAIN_MENU(const std::string& username);
	void UI_Create_Account();
	void UI_Login_Account_MENU();
	bool StillMsgsToPrint();
	void PrintMsgs(std::string& last_sender);
	void ClearPrintMsgsDeque();
	void checkUpdates();
	void WaitForEndOfUpdates();
	void SetState(STATE state);
	void SetUpdateFlagsToZero();
	void UpdateWindwSize();
	void PrintLine(const int& dist_begin, const int& dist_end);
	void PrintAlertsLeft();
	void PrintAlertsCenter();
	void PrintCenter(std::string print);
	void PrintCenter(std::string print, char ch);
	std::string writePassword();
	std::string rewritePassword();
	std::string CinOverwriteRefresh();

private:
	std::deque<std::string> m_alert;
	std::string m_username;
	uint8_t m_ID = 0;
	uint8_t m_GID = 0;
	net_client client;
	std::unordered_map<std::string, FriendsRecord> Friends; //first field of map is Friend's Name
	std::unordered_map<uint8_t, std::string> GroupIDName;
	std::deque<std::pair<uint8_t, std::string>> v_msgsToPrint;
	std::vector<uint8_t> latest_header;
	STATE m_state = Loading;
	bool m_done_checking_updates;
	int m_columns=0;
	int m_rows=0;
};

//Extra functions
inline void clear_screen();
void eraseLine(int count);
int getch_noblock();
bool FindChar(char ch, std::string& string);
void setColour(char ch);