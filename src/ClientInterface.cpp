#include "ClientInterface.h"

ClientInterface::ClientInterface() : client(v_msgsToPrint)
{
	client.Connect("127.0.0.1", 4301);
}
bool ClientInterface::IsConnectedToServer()
{
	if (client.IsConnected())
		return true;
	else
		return false;
}
bool ClientInterface::CreateAccount()
{
	clear_screen();
	while (true) {
		PrintCenter("Type \"/back\" to go back to Account Menu\n\n");
		setColour('y');
		PrintCenter("          --------------------\n");
		PrintCenter("         |   ");
		std::cout << "CREATE ACCOUNT";
		std::cout << "   | \n";
		PrintCenter("          --------------------\n\n\n\n\n");
		PrintCenter("Username: ", 'y');
		std::string login_msg, username, password, password2;
		
		std::getline(std::cin >> std::ws, username, '\n');

		if (FindChar('&', username))
		{
			clear_screen();
			PrintCenter("username cannot contain '&'\n");
			continue;
		}
		if (FindChar(' ', username))
		{
			clear_screen();
			PrintCenter("username cannot contain \" \" (spacebar)\n");
			continue;
		}

		if (username == "/back")
		{
			clear_screen();
			return 0;
		}

		password = writePassword();

		if (FindChar('&', password))
		{
			clear_screen();
			PrintCenter("password cannot contain '&'\n");
			continue;
		}
		if (FindChar(' ', password))
		{
			clear_screen();
			PrintCenter("password cannot contain \" \" (spacebar)\n");
			continue;
		}
		if (password == "/back")
		{
			clear_screen();
			return 0;
		}
		password2 = rewritePassword();
		if (FindChar(' ', password2))
		{
			clear_screen();
			PrintCenter("PASSWORDS DON'T MATCH\n");
			continue;
		}
		if (password == "/back")
		{
			clear_screen();
			return 0;
		}
		if (password2 != password)
		{
			clear_screen();
			PrintCenter("PASSWORDS DON'T MATCH\n");
			continue;
		}
		username.push_back('&');
		username.append(password);
		uint8_t size = username.size();
		login_msg.push_back('U');
		login_msg.push_back(0);
		login_msg.push_back(0);
		login_msg.push_back(size);
		login_msg.append(username);
		client.Sendmsg(login_msg);
		std::vector<uint8_t> ack(4);
		client.Read(ack, 4);
		if (ack[0] == 'U' && ack[1] == 100)
		{
			std::cout << "\n";
			PrintCenter("User Account Created Successfuly!");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1000ms);
			clear_screen();
			return true;
		}
		else
		{
			clear_screen();
			PrintCenter("Username already taken\n");
		}
	}
}
void ClientInterface::AddFriendsMENU(const std::string& myname)
{
	clear_screen();
	bool exit = 0;
	while (!exit) {
		while (!exit) {
			std::cout << std::endl;
			PrintCenter("Type \"/done\" to go back to Account Menu\n");
			PrintCenter("          -----------------\n");
			PrintCenter("         |   ");
			setColour('y');
			std::cout << "ADD FRIENDS";
			setColour('w');
			std::cout << "   | \n";
			PrintCenter("          -----------------\n\n\n\n\n");
			setColour('y');
			PrintCenter("  Your Friends:\n");
			setColour('w');
			for (auto& it : Friends)
			{
				setColour('y');
				PrintCenter("  -> ");
				setColour('w');
				std::cout << it.first + "\n";
			}
			std::string friend_name;
			std::cout << "\n\n\n";
			setColour('y');
			PrintCenter("Friend's username: ");
			setColour('w');
			std::cin >> friend_name;
			if (friend_name == "/done")
			{
				exit = 1;
				clear_screen();
				break;
			}
			else if (friend_name == myname)
			{
				clear_screen();
				PrintCenter(myname + " is your name, you cannot add yourself as a friend\n\n");
				break;
			}
			else {
				if (Friends.find(friend_name) != Friends.end())
				{
					clear_screen();
					PrintCenter("User \"" + friend_name + "\" is already a friend\n\n");
					break;
				}
			}
			std::string add_friend_msg;
			uint8_t size = friend_name.size();
			add_friend_msg.push_back('A');
			add_friend_msg.push_back(0);
			add_friend_msg.push_back(0);
			add_friend_msg.push_back(size);
			add_friend_msg.append(friend_name);
			client.Sendmsg(add_friend_msg);
			while (latest_header[0] != 'A') {}
			if (latest_header[0] == 'A' && latest_header[1] > 0)
			{
				Friends[friend_name] = FriendsRecord{ latest_header[1] };
				clear_screen();
				latest_header.resize(0);
				latest_header.resize(4);
				std::cout << "\n\n";
			}
			else
			{
				clear_screen();
				PrintCenter("No user with the name \"" + friend_name + "\" registered\n\n");
			}
		}
	}
}
bool ClientInterface::Login(uint8_t& myID, std::string& username)
{
	clear_screen();
	while (true) {

		PrintCenter("Type \"/back\" to go back to Account Menu\n\n");
		setColour('y');
		PrintCenter("          -----------\n");
		PrintCenter("         |   ");
		std::cout << "LOGIN";
		std::cout << ("   | \n");
		PrintCenter("          -----------\n\n\n\n");

		std::string login_msg, password, input;
		PrintCenter("Username: ");
		setColour('w');
		std::getline(std::cin >> std::ws, input, '\n');

		if (FindChar('&', input))
		{
			clear_screen();
			PrintCenter("username cannot contain '&'\n");
			continue;
		}
		if (FindChar(' ', input))
		{
			clear_screen();
			PrintCenter("username cannot contain \" \" (spacebar)\n");
			continue;
		}

		if (input == "/back")
		{
			clear_screen();
			return 0;
		}
		password = writePassword();
		if (FindChar('&', password))
		{
			clear_screen();
			PrintCenter("username cannot contain '&'\n");
			continue;
		}
		if (FindChar(' ', password))
		{
			clear_screen();
			PrintCenter("username cannot contain \" \" (spacebar)\n");
			continue;
		}
		if (password == "/back")
		{
			clear_screen();
			return 0;

		}
		username = input;
		input.push_back('&');
		input.append(password);
		uint8_t size = input.size();
		login_msg.push_back('L');
		login_msg.push_back(0);
		login_msg.push_back(0);
		login_msg.push_back(size);
		login_msg.append(input);
		client.Sendmsg(login_msg);
		std::vector<uint8_t> ack(4);
		client.Read(ack, 4);
		if (ack[0] == 'L' && ack[1] > 0)
		{
			m_username = username;
			m_ID = ack[1];
			myID = m_ID;
			client.SetGrabBool(1);
			client.RunContextThread();
			latest_header.resize(0);
			latest_header.resize(4);
			client.GrabSomeData(latest_header[0], latest_header[1], latest_header[2], latest_header[3]);
			clear_screen();
			return 1;
		}
		clear_screen();
		PrintCenter("Username and Passord don't match, try again!\n");
	}
}
bool ClientInterface::CreateGroup(uint8_t& myID, uint8_t& gid, std::string& group_name, std::string& myname)
{
	clear_screen();
	while (true)
	{
		PrintCenter("Type \"/back\" to go back to Account Menu\n");
		setColour('y');
		PrintCenter("          -----------------\n");
		PrintCenter("         |   ");
		std::cout << "CREATE GOUP";
		std::cout << "   |\n";
		PrintCenter("          -----------------\n\n");
		PrintCenter("   You can add people from your list of friends\n\n");
		PrintCenter("List of Friends : \n", 'y');

		for (auto& it : Friends)
		{
			PrintCenter("   -> ", 'y');
			std::cout << it.first + "\n";
		}

		std::cout << "\n\n";
		std::vector<std::string>::iterator it2, it3;
		std::string friends_username;
		PrintCenter("Group-Name: ", 'y');
		std::cin >> group_name;
		if (group_name.find("&") == 0)
		{
			clear_screen();
			PrintCenter("username cannot contain '&'\n");
		}
		if (group_name == "/back")
		{
			clear_screen();
			return 0;
		}
		std::cout << "\n";
		PrintCenter("     (type \"\/done\" to create group with added users)\n\n");
		PrintCenter("Add user to group: ", 'y');
		bool not_friends = 0;
		std::vector<std::string> add_to_group;
		std::string ID_add_to_group;
		while (true)
		{
			std::cin >> friends_username;
			if (friends_username == "/back")
			{
				clear_screen();
				return 0;
			}
			if (friends_username == "/done") break;
			bool not_friends = 0, already_added = 0;
			if (friends_username == myname)
			{
				std::cout << "\n";
				PrintCenter("     " + myname + " is your name, by creating the group you are automatically added to the group\n\n");
				goto NEXT_FRIEND_TO_ADD;
			}
			else if (Friends.find(friends_username) == Friends.end())
			{
				std::cout << "\n";
				PrintCenter("     " + friends_username + " is not a friend, if you want to\n");
				PrintCenter("add " + friends_username + " to this group first add this\n");
				PrintCenter("user to your friends list\n");
				PrintCenter("(Main Menu-> 2. Add Friends)\n\n");
				goto NEXT_FRIEND_TO_ADD;
			}
			else
			{
				for (std::vector<std::string>::iterator it = add_to_group.begin(); it != add_to_group.end(); it++)
				{
					if (*it == friends_username)
					{
						std::cout << "\n";
						PrintCenter("     " + friends_username + " was already added to this group\n\n");
						goto NEXT_FRIEND_TO_ADD;
					}
				}
				add_to_group.push_back(friends_username);
				ID_add_to_group.push_back(Friends.at(friends_username).fid);
			}
		
		NEXT_FRIEND_TO_ADD: 
		setColour('y');
		PrintCenter("Add another user to group: ");
		setColour('w');
		}

		clear_screen();
		PrintCenter(" Type \"/back\" to go back to Account Menu\n");
		setColour('y');
		PrintCenter("          -----------------\n");
		PrintCenter("         |   ");
		std::cout << "CREATE GOUP";
		std::cout << "   |\n";
		PrintCenter("          -----------------\n\n");
		PrintCenter(" Group: ", 'y');
		std::cout << group_name + "\n";

		for (it2 = add_to_group.begin(); it2 != add_to_group.end(); it2++)
		{
			PrintCenter("   -> ", 'y');
			std::cout << *it2 + "\n";
		}

		std::string create_group;
		std::cout << "\n\n";
		PrintCenter("Create Group?", 'y');
		std::cout << "         (Type ";
		setColour('y');
		std::cout << "\"yes\"";
		setColour('w');
		std::cout << " to create group or \n";
		setColour('y');
		PrintCenter("                      \"no\"");
		setColour('w');
		std::cout << " to go back to Main Menu, other\n";
		PrintCenter("                      answers will be considered ");
		setColour('y');
		std::cout << "\"no\"";
		setColour('w');
		std::cout << ")\n";
		PrintCenter("");
		std::cin >> create_group;
		if (create_group != "yes")
		{
			clear_screen();
			return 0;
		}
		std::string create_group_msg;

		//Get IDs from Names and push to all_IDs_to_add

		uint8_t size = group_name.size() + 1 + ID_add_to_group.size();
		create_group_msg.push_back('C');
		create_group_msg.push_back(myID);
		create_group_msg.push_back(0);
		create_group_msg.push_back(size);
		create_group_msg.append(group_name);
		create_group_msg.push_back(0);
		create_group_msg.append(ID_add_to_group);
		client.Sendmsg(create_group_msg);

		while (latest_header[0] != 'C') {}
		if (latest_header[0] == 'C' && latest_header[1] > 0 && latest_header[3] == 0) {
			gid = latest_header[1];
			latest_header.resize(0);
			latest_header.resize(4);
			clear_screen();
			GroupIDName[gid] = group_name;
			PrintCenter("Group " + group_name + " created Successfuly, returning to main page ...");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(1700ms);
			clear_screen();
			return 1;
		}
		else
		{
			clear_screen();
			PrintCenter("Couldn't Create Group!\n");
			//something
			return 0;
		}
	}
}
void ClientInterface::UI_Login_Account_MENU()
{
	clear_screen();
	setColour('y');
	PrintCenter("- - - - - - - - - - - - - - - - -\n");
	PrintCenter("-       ");
	std::cout << "WELCOME TO WECHAT";
	std::cout << "       -\n";
	PrintCenter("- - - - - - - - - - - - - - - - -\n");
	setColour('w');
	PrintCenter("Type \"/exit\" to exit app\n\n\n");
	PrintCenter("MENU:\n\n", 'y');
	PrintCenter("0. Login                   (type \"0\")\n");
	PrintCenter("1. Create User Account     (type \"1\")\n\n");
	PrintCenter("OPTION: ", 'y');
}
void ClientInterface::UI_Create_Account()
{
	clear_screen();
	PrintCenter(" Type \"/back\" to go back to Account Menu\n");
	setColour('y');
	PrintCenter("          -----------------\n");
	PrintCenter("         |   ");
	std::cout << "CREATE GOUP";
	std::cout << "   | \n";
	PrintCenter("          -----------------\n\n");
	setColour('w');
	PrintCenter("     You can add people from your list of friends\n\n");
	PrintCenter("List of Friends : \n", 'y');

	for (auto& it : Friends)
	{
		PrintCenter("   -> ", 'y');
		std::cout << it.first << "\n";
	}
	std::cout << "\n\n";
}
void ClientInterface::UI_MAIN_MENU(const std::string& username)
{
	clear_screen();
	PrintAlertsLeft();
	PrintCenter("Welcome, " + username + "! You're Logged In!\n\n");
	PrintCenter("Type \"/exit\" to exit app\n");
	PrintCenter("Type \"/back\" to LogOut\n\n");
	PrintCenter("You are part of the following groups:\n\n", 'y');
	for (auto& it : GroupIDName)
	{
		PrintCenter("     -> ", 'y');
		std::cout << it.second + "\n";
	}
	PrintCenter("\n\n\n\n\n");
	PrintCenter("MENU:\n\n", 'y');
	PrintCenter("0. Enter Group             (type \"0\")\n");
	PrintCenter("1. Create Group            (type \"1\")\n");
	PrintCenter("2. Add Friends             (type \"2\")\n\n");
	PrintCenter("OPTION: ", 'y');
}
bool ClientInterface::EnterGroup(uint8_t& groupnumber, std::string& groupname, std::uint8_t& myID)
{
	clear_screen();
	while (true) {
		std::cout << "\n\n\n";
		setColour('y');
		PrintCenter("Which group do you wish to enter?\n");
		setColour('w');
		PrintCenter("(Type group-number to enter group)\n\n");
		int i = 0;
		for (auto& it : GroupIDName)
		{
			PrintCenter(std::to_string(i) + ". " + it.second + "\n");
			i++;
		}
		std::cout << "\n";
		setColour('y');
		PrintCenter("OPTION: ");
		setColour('w');
		std::string input;
		std::cin >> input;
		if (input == "/back")
		{
			clear_screen();
			return 0;
		}
		int num;
		try {
			num = stoi(input);
		}
		catch (std::invalid_argument& e) {
			clear_screen();
			PrintCenter("Valid Values 0 - " + std::to_string(i) + "\n");
			continue;
		}
		catch (std::out_of_range& e) {
			clear_screen();
			PrintCenter("Valid Values 0 - " + std::to_string(i) + "\n");
			continue;
		}
		if (num >= 0 && num <= GroupIDName.size() && GroupIDName.size() > 0)
		{
			i = 0;
			for (auto& it : GroupIDName)
			{
				if (i == num)
				{
					groupnumber = it.first;
					m_GID = groupnumber;
					groupname = it.second;
					break;
				}
				i++;
			}
			std::string enter_group_msg;
			enter_group_msg.push_back('E'); //Enter
			enter_group_msg.push_back(myID);
			enter_group_msg.push_back(groupnumber);
			enter_group_msg.push_back(0);
			client.Sendmsg(enter_group_msg);
			while (latest_header[0] != 'E') {}
			if (latest_header[0] == 'E' && latest_header[1] == myID && latest_header[2] == groupnumber)
			{
				latest_header.resize(0);
				latest_header.resize(4);
				clear_screen();
				PrintCenter("ENTERING GROUP...");
				return 1;
			}
			else
			{
				ClearPrintMsgsDeque();
				clear_screen();
				PrintCenter(" Error Connecting to Group, try again later!\n");
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(800ms);
				PrintCenter("Going Back To MAIN MENU...");
				std::this_thread::sleep_for(1500ms);
				clear_screen();
				return 0;
			}
		}
		else
		{
			clear_screen();
			PrintCenter("Valid Values 0 - " + std::to_string(i) + "\n");
		}
	}
}
void ClientInterface::UI_TopChat(const std::string& groupname)
{
	clear_screen();
	PrintCenter("- - - ");
	for (int i = 0; i < (groupname.size() + 8) / 2; i++) std::cout << "- ";
	std::cout << "- - - \n";
	PrintCenter("-     ");
	setColour('y');
	std::cout << "CHAT";
	setColour('w');
	std::cout << " : " + groupname + "    ";
	if ((groupname.size() % 2) == 0)
		std::cout << " ";
	std::cout << "-\n";
	PrintCenter("- - - ");
	for (int i = 0; i < ((groupname.size() + 8) / 2); i++) std::cout << "- ";
	std::cout << "- - - \n";
}
void ClientInterface::Chat(const uint8_t& gID, const uint8_t& myID)
{
	std::string last_sender;
	bool end = 0;
	bool first_msg = 1;
	while (!end)
	{
		char pwd[400];
		int i = 0, count = 0;
		bool print_again = 0;
		for (int i = 0; i < m_columns / 2; i++)
		{
			std::cout << " ";
		}
		if (first_msg)
		{
			setColour('y');
			std::cout << "You: ";
			setColour('w');
		}
		else 
		{
			setColour('w');
			std::cout << "     ";
		}
		while (true)
		{
			pwd[i] = (char)getch_noblock();
			if (StillMsgsToPrint())
			{
				std::cout << "\33[2K\r";
				while (StillMsgsToPrint())
					PrintMsgs(last_sender);
				setColour('y');
				for (int j = 0; j < m_columns / 2; j++)
				{
					std::cout << " ";
				}
				first_msg = 1;
				if (last_sender != m_username)
					std::cout << "You: ";
				else
					std::cout << "     ";
				setColour('w');
				for (int a = 0; a < count; a++)
				{
					std::cout << pwd[a];
				}
			}
			if (pwd[i] == -1)continue;
			if (pwd[i] == 0x0D)break;
			if ((pwd[i] != 0x0D) && (pwd[i] != 0x08))
			{
				std::cout << pwd[i];
				i++;
				count++;
			}
			else if ((pwd[i] == 0x08) && (i > 0))
			{
				std::cout << "\33[2K\r";
				for (int i = 0; i < m_columns / 2; i++)
				{
					std::cout << " ";
				}
				if (last_sender!=m_username)
				{
					setColour('y');
					std::cout << "You: ";
					setColour('w');
				}
				else std::cout << "     ";
				pwd[i] = NULL;
				i--;
				count--;

				for (int a = 0; a < count; a++)
				{
					std::cout << pwd[a];
				}
			}
		}
		pwd[i] = '\0';
		std::string message(pwd);
		std::cout << "\n";
		first_msg = 0;

		if (message == "/back")
		{
			ClearPrintMsgsDeque();
			clear_screen();
			break;
		}
		else
		{
			std::string msg;
			uint8_t size = message.size();
			msg.push_back('S');
			msg.push_back(myID);
			msg.push_back(gID);
			msg.push_back(size); //size);
			msg.append(message);
			client.Sendmsg(msg);
		}
	}
}
void ClientInterface::LeaveGroup(uint8_t& groupID, uint8_t& personalID)
{
	std::string msg;
	msg.push_back('E');
	msg.push_back(personalID);
	msg.push_back(0);
	msg.push_back(0);
	client.Sendmsg(msg);
	while (latest_header[0] != 'E') {}
	if (latest_header[0] == 'E' && latest_header[1] == personalID && latest_header[2] == 0)
	{
		latest_header.resize(0);
		latest_header.resize(4);
		clear_screen();
	}
	else
	{
		ClearPrintMsgsDeque();
		clear_screen();
		PrintCenter("Error exiting group, if you wish to enter a different group please restart WeChat!\n");
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1000ms);
		PrintCenter("Returning to Main Menu...");
		std::this_thread::sleep_for(1500ms);
		clear_screen();
	}

}
void ClientInterface::UpdateFriends(uint8_t& myID)
{
	std::string update_f_msg;
	update_f_msg.push_back('F');
	update_f_msg.push_back(myID);
	update_f_msg.push_back(0);
	update_f_msg.push_back(0);
	client.Sendmsg(update_f_msg);
}
void ClientInterface::UpdateGroups(uint8_t& myID)
{
	std::string update_g_msg;
	update_g_msg.push_back('G');
	update_g_msg.push_back(myID);
	update_g_msg.push_back(0);
	update_g_msg.push_back(0);
	client.Sendmsg(update_g_msg);
}
bool ClientInterface::StillMsgsToPrint()
{
	if (v_msgsToPrint.size() == 0 || v_msgsToPrint.empty())
		return false;
	else
		return true;
}
void ClientInterface::PrintMsgs(std::string& last_sender)
{
	if (!v_msgsToPrint.empty())
	{
		bool skip_friends = 0;
		if (v_msgsToPrint.front().first == m_ID)
		{
			for (int i = 0; i < m_columns / 2; i++)
			{
				std::cout << " ";
			}
			if (last_sender == m_username)
			{
				std::cout << "     ";
			}
			else
			{
				setColour('y');
				std::cout << "You: ";
				setColour('w');
				last_sender = m_username;
			}
			skip_friends = 1;
		}
		bool is_a_friend = 0;
		if (!skip_friends)
		{
			for (auto& it : Friends)
			{
				if (it.second.fid == v_msgsToPrint.front().first) //FRIEND
				{
					if (it.first != last_sender)
					{
						last_sender.resize(0);
						last_sender.resize(500);
						last_sender = it.first;
						std::cout << "\n";
						setColour('y');
						std::cout << it.first << " : ";
						setColour('w');
					}
					else
					{
						for (int i = 0; i < it.first.size() + 3; i++)
							std::cout << " ";
					}
					is_a_friend = 1;
					break;
				}
			}
			if (!is_a_friend && last_sender != m_username) //NOT FRIEND //not going to happen anymore, msgs from unknown users not received
			{
				last_sender.resize(0);
				last_sender.resize(500);
				last_sender = "[unknown_sender]";
				setColour('y');
				std::cout << "[unknown user]" << " : ";
				setColour('w');
			}
		}
		std::cout << v_msgsToPrint.front().second << "\n";
		v_msgsToPrint.pop_front();
	}
}
void ClientInterface::ClearPrintMsgsDeque()
{
	if(!v_msgsToPrint.empty())
		v_msgsToPrint.erase(v_msgsToPrint.begin(), v_msgsToPrint.end());
}
void ClientInterface::checkUpdates()
{
	bool done_reading_friends = 0, done_reading_groups = 0;
	while (true)
	{
		if (!client.UpdateIsEmpty())
		{
			Message update_msg = client.getUpdateMsg();
			if (update_msg.type == 'F' && update_msg.id > 0 && update_msg.body_size > 0)
			{
				done_reading_friends = 0;
				bool done_with_fname = 0;
				std::string friend_name;
				std::vector<uint8_t> g_IDs;
				for (int i = 0; i < update_msg.body_size; i++)
				{
					if (done_with_fname)
					{
						g_IDs.push_back(update_msg.msg[i]);
					}
					else if (update_msg.msg[i] != 0 && !done_with_fname)
					{
						friend_name.push_back(update_msg.msg[i]);
						continue;
					}
					else
					{
						done_with_fname = 1;
						continue;
					}
				}
				if (Friends.find(friend_name) == Friends.end()) //friend doesnt exist in map
				{
					Friends[friend_name] = FriendsRecord{ update_msg.id, g_IDs };
					m_alert.push_back(friend_name + " is now a Friend\n");

					if (m_state == InMainMenu)
					{
						UI_MAIN_MENU(m_username);
					}
					if (m_state != LogIn && m_state !=InMainMenu)
					{
						PrintCenter("[ALERT] ", 'g');
						std::cout << friend_name << " was added as Friend\n";
					}
				}
				else
					Friends[friend_name] = FriendsRecord{ update_msg.id, g_IDs };
			}
			else if (update_msg.type == 'G' && update_msg.gid > 0 && update_msg.body_size > 0)
			{
				done_reading_groups = 0;
				std::string group_name;
				for (int i = 0; i < update_msg.body_size; i++)
				{
					group_name.push_back(update_msg.msg[i]);
				}
				if (GroupIDName.find(update_msg.gid) == GroupIDName.end())
				{
					GroupIDName[update_msg.gid] = group_name;
					m_alert.push_back("you were added to group \"" + group_name + "\"\n");
					
					if (m_state == InMainMenu)
					{
						UI_MAIN_MENU(m_username);
					}
					if (m_state != LogIn && m_state != InMainMenu)
					{
						PrintCenter("[ALERT] ", 'g');
						std::cout << "you were added to group \"" << group_name << "\"\n\n";
					}
				}
			}
			else if (update_msg.type == 'F' && update_msg.id == 0)
			{
				done_reading_friends = 1;
			}
			else if (update_msg.type == 'G' && update_msg.gid == 0)
			{
				done_reading_groups = 1;
			}
		}
		else if (done_reading_friends && done_reading_groups && !m_done_checking_updates)
		{
			m_done_checking_updates = 1;
		}
	}
}
void ClientInterface::WaitForEndOfUpdates()
{
	int tries = 0;
	clock_t now = clock();
	while (true)
	{
		if (m_done_checking_updates) {
			m_done_checking_updates = 0;
			break;
		}
		if (tries > 4)
		{
			clear_screen();
			PrintCenter("Error geting updates from server,\n");
			PrintCenter("loading app without friends and groups data\n\n");
			using namespace std::chrono_literals;
			std::this_thread::sleep_for(2000ms);
			m_done_checking_updates = 0;
			break;
		}
		if (clock()-now > 4000)
		{
			std::cout << "TRY AGAIN\n";
			now = clock();
			tries++;
			UpdateFriends(m_ID);
			UpdateGroups(m_ID);
		}
	}
}
void ClientInterface::SetState(STATE state)
{
	m_state = state;
}
void ClientInterface::SetUpdateFlagsToZero()
{
	m_done_checking_updates = 0;
}
void ClientInterface::UpdateWindwSize()
{
	while (true)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		int columns, rows;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
		columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(100ms);
		CONSOLE_SCREEN_BUFFER_INFO csbi2;
		int new_columns, new_rows;
		GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi2);
		new_columns = csbi2.srWindow.Right - csbi2.srWindow.Left + 1;
		new_rows = csbi2.srWindow.Bottom - csbi2.srWindow.Top + 1;

		if ((columns != m_columns || rows != m_rows) && new_columns == columns && new_rows==rows)
		{
			m_columns = columns;
			m_rows = rows;
			if (m_state == NewUser)
			{
				clear_screen();
				UI_Login_Account_MENU();
				m_state = RefreshNewUser;
			}
			if (m_state == InMainMenu)
			{
				clear_screen();
				UI_MAIN_MENU(m_username);
				m_state = RefreshInMainMenu;
			}
			if (m_state == InGroup) //not yet implemented
			{
				clear_screen();
				UI_TopChat(GroupIDName.at(m_GID));
				ClearPrintMsgsDeque();
				std::string enter_group_msg;
				enter_group_msg.push_back('E'); //Enter
				enter_group_msg.push_back(m_ID);
				enter_group_msg.push_back(m_GID);
				enter_group_msg.push_back(0);
				client.Sendmsg(enter_group_msg);
			}
		}
	}
}
void ClientInterface::PrintLine(const int& dist_begin, const int& dist_end)
{
	if (dist_end < m_columns)
	{
		for (int i = 0; i < dist_begin; i++)
		{
			std::cout << " ";
		}
		for (int i = dist_end; i < m_columns; i++)
		{
			std::cout << "-";
		}
	}
}
void ClientInterface::PrintAlertsLeft()
{
	if (m_alert.empty())
		return;
	for (auto& it : m_alert)
	{
		PrintCenter("[ALERT] ", 'g');
		std::cout << it << "\n";
	}
	std::cout << "\n";
}
void ClientInterface::PrintCenter(std::string print)
{
	for (int i = 0; i < ((m_columns / 2) - 22); i++)
	{
		std::cout << " ";
	}
	std::cout << print;
}
void ClientInterface::PrintCenter(std::string print, char ch)
{
	setColour(ch);
	for (int i = 0; i < ((m_columns / 2) - 22); i++)
	{
		std::cout << " ";
	}
	std::cout << print;
	setColour('w');
}
void ClientInterface::PrintAlertsCenter()
{
	if (m_alert.empty())
		return;
	if ((m_columns / 2 - 10) >= 0)
	{
		for (auto& it : m_alert)
		{
			std::cout << "\n";
			for (int i = 0; i < ((m_columns / 2) - (it.size() / 2) - 4); i++)
			{
				std::cout << " ";
			}
			setColour('g');
			std::cout << "[ALERT] ";
			setColour('w');
			std::cout << it << "\n";
		}
		std::cout << "\n";
	}
	else
		PrintAlertsLeft();
	
}
std::string ClientInterface::writePassword()
{
	char pwd[50];
	int i = 0, count = 0;

	setColour('y');
	PrintCenter("Password: ");
	setColour('w');
	while ((pwd[i] = _getch()) != 0x0D)
	{
		if ((pwd[i] != 0x0D) && (pwd[i] != 0x08))
		{
			std::cout << "*";
			i++;
			count++;
		}
		else if ((pwd[i] == 0x08) && (i > 0))
		{
			std::cout << "\33[2K\r";
			setColour('y');
			PrintCenter("Password: ");
			setColour('w');
			pwd[i] = NULL;
			i--;
			count--;

			for (int a = 0; a < count; a++)
			{
				std::cout << "*";
			}
		}
	}
	pwd[i] = '\0';
	std::cout << "\n";

	std::string password(pwd);

	return password;
};
std::string ClientInterface::rewritePassword()
{
	char pwd[50];
	int i = 0, count = 0;
	setColour('y');
	PrintCenter("Rewrite Password: ");
	setColour('w');

	while ((pwd[i] = _getch()) != 0x0D)
	{
		if ((pwd[i] != 0x0D) && (pwd[i] != 0x08))
		{
			std::cout << "*";
			i++;
			count++;
		}
		else if ((pwd[i] == 0x08) && (i > 0))
		{
			std::cout << "\33[2K\r";
			setColour('y');
			PrintCenter("Rewrite Password: ");
			setColour('w');
			pwd[i] = NULL;
			i--;
			count--;

			for (int a = 0; a < count; a++)
			{
				std::cout << "*";
			}
		}
	}
	pwd[i] = '\0';
	std::cout << "\n";

	std::string password(pwd);

	return password;
};
std::string ClientInterface::CinOverwriteRefresh()
{
	int i = 0;
	char ch[50];
	while (true)
	{
		ch[i] = getch_noblock();

		if (ch[i] != -1)
		{
			if ((ch[i] != 0x0D) && (ch[i] != 0x08))
			{
				std::cout << ch[i];
				i++;
			}
			else if ((ch[i] == 0x08) && (i > 0))
			{
				std::cout << "\33[2K\r";

				setColour('y');
				PrintCenter("OPTION: ");
				setColour('w');
				ch[i] = NULL;
				i--;
				for (int a = 0; a < i; a++)
				{
					std::cout << ch[a];
				}
			}
			else if(ch[i] == 0x0D)
			{
				ch[i] = '\0';
				std::string final(ch);
				return final;
			}
		}
		if (m_state == RefreshNewUser)
		{
			for (int a = 0; a < i; a++)
			{
				std::cout << ch[a];
			}
			m_state = NewUser;
		}
		if (m_state == RefreshInMainMenu)
		{
			for (int a = 0; a < i; a++)
			{
				std::cout << ch[a];
			}
			m_state = InMainMenu;
		}
	}
}

inline void clear_screen()
{
#ifdef _WIN32
	system("CLS");
#endif // _WIN32
#ifdef __unix__
	system("CLS");
#endif // _posix
}
void eraseLine(int count) {
	if (count > 0) {
		for (int i = 0; i < count; i++) {
			std::cout
				<< "\x1b[1A";
		}
		std::cout << "\x1b[2K"; // Delete current line
		std::cout << "\r"; // Resume the cursor at beginning of line
	}
}
int getch_noblock() {
	if (_kbhit())
		return _getch();
	else
		return -1;
}
bool FindChar(char ch, std::string& string)
{
	for (std::string::iterator it = string.begin(); it != string.end(); it++)
	{
		if (*it == ch)
		{
			return 1;
		}
	}
	return 0;
}
void setColour(char ch)
{
	if (ch == 'w')
	{
		std::cout << "\033[1;37m";
	}
	else if (ch == 'y')
	{
		std::cout << "\033[1;33m";
	}
	else if (ch == 'g')
	{
		std::cout << "\033[1;32m";
	}
	else if (ch == 'b')
	{
		std::cout << "\033[1;36m";
	}
	else if (ch == 'g')
	{
		std::cout << "\033[1;47m";
	}
	else if (ch == 'r')
	{
		std::cout << "\033[1;31m";
	}
}
