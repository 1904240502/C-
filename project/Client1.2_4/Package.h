enum DataType
{
	LOGIN,
	LOGIN_RESULT,
	LOGOUT,
	LOGOUT_RESULT,
	NEW_USER_JOIN,
	WEEOR
};
class DataHeader
{
public:
	DataHeader()
	{
		dataLength = sizeof(DataHeader);
		type = WEEOR;
	}
	short dataLength;
	DataType type;
};
class Login :public DataHeader
{
public:
	Login() :userName(""), passWord(""), data("")
	{
		dataLength = sizeof(Login);
		type = LOGIN;
	}
	char userName[32];
	char passWord[32];
	char data[28];
};
class LoginResult :public DataHeader
{
public:
	LoginResult() :result(-1), data("")
	{
		dataLength = sizeof(LoginResult);
		type = LOGIN_RESULT;
	}
	int result;
	char data[88];
};
class Logout :public DataHeader
{
public:
	Logout() :userName(""), data("")
	{
		dataLength = sizeof(Logout);
		type = LOGOUT;
	}
	char userName[32];
	char data[60];
};
class LogoutResult :public DataHeader
{
public:
	LogoutResult() :result(-1), data("")
	{
		dataLength = sizeof(LogoutResult);
		type = LOGOUT_RESULT;
	}
	int result;
	char data[88];
};
class NewUserJoin :public DataHeader
{
public:
	NewUserJoin() :sock(0), data("")
	{
		dataLength = sizeof(NewUserJoin);
		type = NEW_USER_JOIN;
	}
	int sock;
	char data[88];
};