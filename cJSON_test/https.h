#include"openssl/ssl.h"
#include <WinSock2.h>
class HttpsClient
{
public: 
	HttpsClient();
	~HttpsClient();
public:
	bool InitSocket();
	bool initSSL();
	bool SslConnect();
	bool SendPostData();
	void RecvPostData();
	void ParseCookieFromRecvData();
	bool SslGetCipherAndCertification();
public:
	WSADATA *wsaData;
	SOCKET socketClient;
	SOCKADDR_IN *socketAddrClient;
	SSL *ssl;
	SSL_CTX *sslCtx;
	SSL_METHOD *sslMethod;

};