#include "ClientInterface.h"

int main()
{
	ClientInterface client;
	client.PrintCenter("LOADING...");
	uint8_t personalID;
	std::string username;
	std::string choiceLU, choiceGFA;
	bool in_app = 1, login_successful = 0;
	bool logout = 0;
	bool exit = 0;

	std::thread thr_windowSize(&ClientInterface::UpdateWindwSize, &client);
	while(!client.IsConnectedToServer()){}
BEGIN:
	while (true)
	{
		clear_screen();
		client.SetState(NewUser);
		client.UI_Login_Account_MENU();
		choiceLU = client.CinOverwriteRefresh();
		if (choiceLU == "/exit") return 0;
		if (choiceLU == "0")
		{
			client.SetState(LogIn);
			if (client.Login(personalID, username)) break;
		}
		else if (choiceLU == "1")
		{
			client.SetState(CreateAccount);
			if (client.CreateAccount())
			{
				client.SetState(LogIn);
				if (client.Login(personalID, username)) break;
			}
		}
	}
	client.PrintCenter("LOADING...");
	client.SetUpdateFlagsToZero();
	std::thread thr_updates(&ClientInterface::checkUpdates, &client);
	client.UpdateFriends(personalID);
	client.UpdateGroups(personalID);
	client.WaitForEndOfUpdates();
	clear_screen();
	while (true)
	{
		client.SetState(InMainMenu);
		client.UI_MAIN_MENU(username);
		choiceGFA = client.CinOverwriteRefresh();
		if (choiceGFA == "0")
		{
			std::string groupName;
			uint8_t groupID;
			client.SetState(EnterGroup);
			if (client.EnterGroup(groupID, groupName, personalID))
			{
				client.UI_TopChat(groupName);
				client.SetState(InGroup);
				client.Chat(groupID, personalID);
				client.SetState(LeaveGroup);
				client.LeaveGroup(groupID, personalID);
			}
			clear_screen();
		}
		else if (choiceGFA == "1")
		{
			uint8_t groupID;
			std::string groupName;
			client.SetState(CreateGroup);
			client.CreateGroup(personalID, groupID, groupName, username);
			clear_screen();
		}
		else if (choiceGFA == "2")
		{
			client.SetState(AddFriend);
			client.AddFriendsMENU(username);
			clear_screen();
		}
		else if (choiceGFA == "/back")
		{
			clear_screen();
			break;
			
		}
		else if (choiceGFA == "/exit")
		{
			exit = 1;
			clear_screen();
			break;
		}
		else
		{
			clear_screen();
			client.PrintCenter("Not a valid value! Valid Values: 0 - 3\n");
		}
	}
	if (exit) return 0;
	goto BEGIN;
}