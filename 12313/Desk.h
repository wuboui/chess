#include "UserInfo.h"
class UserInfo;
class Desk 
{
public:
	int DeskNo;
protected:
	std::vector<UserInfo*> m_pUserInfo;
public:
	Desk()
	{

	}
	bool _setUserInfo(UserInfo* user)
	{
		if (m_pUserInfo.size() >= 2)
			return false;
		else
		{
			m_pUserInfo.push_back(user);
			return true;
		}
	};
	bool IsFull()
	{
		if (m_pUserInfo.size() > 2)
			return true;
		else
			return false;
	}
};