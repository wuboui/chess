#include"curl/curl.h"
#include <openssl/ssl.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include<string>
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"wldap32.lib")
#pragma comment(lib,"ws2_32.lib")

#include "cJSON.h"

char * makeJson()
{
	cJSON *pJsonRoot = NULL;
	pJsonRoot = cJSON_CreateObject();

	if (NULL == pJsonRoot)
	{
		printf("cJson_Create error \n");
		return NULL;
	}
	cJSON_AddStringToObject(pJsonRoot, "header", "hello world");
	cJSON_AddNumberToObject(pJsonRoot, "number", 123);
	cJSON_AddBoolToObject(pJsonRoot, "bool", 1);
	cJSON *pSubJson = NULL;
	pSubJson = cJSON_CreateObject();
	if (NULL == pSubJson)
	{
		cJSON_Delete(pJsonRoot);
		printf("Create Sub Error \n");
		return NULL;
	}
	cJSON_AddStringToObject(pSubJson, "subString", "a sub string");
	cJSON_AddItemToObject(pJsonRoot, "subObj", pSubJson);

	cJSON* pSubArr = NULL;
	pSubArr = cJSON_CreateArray();
	cJSON_InsertItemInArray(pSubArr, 0, cJSON_CreateString("a"));
	cJSON_InsertItemInArray(pSubArr, 1, cJSON_CreateString("b"));
	cJSON_InsertItemInArray(pSubArr, 2, cJSON_CreateString("c"));
	cJSON_InsertItemInArray(pSubArr, 3, cJSON_CreateString("d"));
	cJSON_AddItemToObject(pJsonRoot, "arr", pSubArr);
	char *p = cJSON_Print(pJsonRoot);
	if (NULL == p)
	{
		cJSON_Delete(pJsonRoot);
		return NULL;
	}
	cJSON_Delete(pJsonRoot);
	return p;
}

void parseJson(char * pMsg)
{
	if (NULL == pMsg)
	{
		return;
	}
	cJSON * pJson = cJSON_Parse(pMsg);
	if (NULL == pJson)
	{
		printf("Parse Failed \n");
		return;
	}
    cJSON* pSub = cJSON_GetObjectItem(pJson, "header");

	if (NULL == pSub)
	{
		printf("Get Hello Faild \n");
	}
    else
        printf("obj_1 : %d:%s\n", pSub->type, pSub->valuestring);

	pSub = cJSON_GetObjectItem(pJson, "number");
	if (NULL == pSub)
	{
		printf("Get Number Faild \n");
	}
    else
        printf("obj_2 : %d:%d\n", pSub->type, pSub->valueint);

	pSub = cJSON_GetObjectItem(pJson, "bool");
	if (NULL == pSub)
	{
		printf("Get Bool Faild \n");
	}
    else
        printf("obj_3 : %d:%d\n", pSub->type, pSub->valueint);

	pSub = cJSON_GetObjectItem(pJson, "subObj");
	if (NULL == pSub)
	{
		printf("Get SubObj Faild \n");
	}
    else
    {
        printf("subobj : %d\n", pSub->type);
        cJSON* pSubSub = cJSON_GetObjectItem(pSub, "subString");
        if (NULL == pSubSub)
        {
            printf("Get SubObj Faild \n");
        }
        else
            printf("subobj_1 : %d:%s\n", pSubSub->type, pSubSub->valuestring);
    }
	pSub = cJSON_GetObjectItem(pJson, "arr");
	for (int i = 0; i < cJSON_GetArraySize(pSub); ++i)
	{
		cJSON* pSubArr = cJSON_GetArrayItem(pSub, i);
		if (NULL == pSubArr)
		{
			printf("Get SubObj Faild \n");
		}
		else
			printf("subobj_1 : %d:%s\n", pSubArr->type, pSubArr->valuestring);
	}
	cJSON_Delete(pJson);
}
bool GetIpByDomainName(const char* szHost,  char* szIP)
{
	WSADATA wsaData;
	int nAdapter = 0;
	struct sockaddr_in sAddr;
	HOSTENT *pHostEnt = NULL;
	if (WSAStartup(0x0101, &wsaData))
	{
		printf(" wsaStartUp faild \n");
		return false;
	}
	struct addrinfo  hint;
	memset(&hint, 0, sizeof(hint));
	hint.ai_family = AF_INET;
	hint.ai_socktype = SOCK_STREAM;
	pHostEnt = gethostbyname(szHost);
/*	getaddrinfo(szHost, NULL, &hint, &answer);
	pHostEnt->h_addr_list[nAdapter] = (char *)answer->ai_addr;
	pHostEnt->h_length = answer->ai_addrlen;*/
	if (pHostEnt)
	{
		if (pHostEnt->h_addr_list[nAdapter])
		{
			memcpy(&sAddr.sin_addr.s_addr, pHostEnt->h_addr_list[nAdapter], pHostEnt->h_length);
			 inet_ntop(AF_INET,&sAddr.sin_addr,szIP,16);
		}
	}
	else;
	WSACleanup();
	return true;
}
void sendGetRequest()
{
	//socket³õÊ¼»¯
	WSADATA wsaData;
	::WSAStartup(MAKEWORD(2, 2), &wsaData);
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in SerAddr = { 0 };
	int iRet = 0;
	int iAddrLen = 0;
	HANDLE hThread = 0;
	char *bufSend = "GET /otn/leftTicket/init/otn/leftTicket/query?leftTicketDTO.train_date=2017-10-25&leftTicketDTO.from_station=BJP&leftTicketDTO.to_station=SHH&purpose_codes=ADULT HTTP/1.1\r\n"
		"Host: kyfw.12306.cn\r\n"
		"User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64; rv:56.0) Gecko/20100101 Firefox/56.0\r\n"
		"Accept : */*\r\n"
		"Accept-Language: zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3\r\n"
		"Accept-Encoding: gzip, deflate, br\r\n"
		"If-Modified-Since: 0\r\n"
	    "Referer:http://kyfw.12306.cn/otn/leftTicket/init\r\n"
		"Cache-Control: no-cache\r\n"
		"X-Requested-With: XMLHttpRequest\r\n"
		"Connection:keep-alive\r\n"
		"Pragma:no-cache\r\n";
	char addIp[256] = { 0 };
	GetIpByDomainName("www.12306.cn", addIp);
	inet_pton(AF_INET, addIp, &SerAddr.sin_addr.s_addr);
	SerAddr.sin_port = htons(80);
	SerAddr.sin_family = AF_INET;
	char bufRecv[3096] = { 0 };
	int iErrNo = 0;
	iErrNo = connect(clientSocket, (sockaddr*)&SerAddr, sizeof(SerAddr));
	/*char tmp[10240 + 1];
	char authstr[10240 + 1];
	memset(tmp, 0x0, sizeof(tmp));
	memset(authstr, 0x0, sizeof(authstr));
	//sprintf(tmp, "%s:%s", user, passwd);
	printf("=======%s", tmp);
//	to64frombits((unsigned char*)authstr, (unsigned char*)tmp, strlen(tmp));
	printf("--=====%s", authstr);
	sprintf_s(tmp, sizeof(tmp), "CONNECT %s:%d HTTP/1.0\r\nProxy-Authorization: Basic %s\r\n\r\n", "www.12306.cn", "443", authstr);
	send(clientSocket, tmp, strlen(tmp), 0);
	printf("-=-=-=-=-=wait for recv-=-=-=-=-=\n");

	memset(tmp, 0x0, sizeof(tmp));
	recv(clientSocket, tmp, sizeof(tmp), 0);
	printf("recv=%s", tmp);*/
	if (iErrNo == 0)
	{
		if (send(clientSocket, bufSend, strlen(bufSend), 0) > 0)
		{
			printf("Send Success \n");
		}

		if (recv(clientSocket, bufRecv, 3096, 0) > 0)
		{
			printf("Recv Success \n");
		}
		else
			printf("%d", WSAGetLastError());
	}
	else
		iErrNo = WSAGetLastError();
	::WSACleanup();
}
std::wstring UTF8ToUnicode(const std::string &str)
{
	int len = str.length();
	int unicodelen = ::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), len, NULL, 0);
	wchar_t *pUnicode = new wchar_t[unicodelen + 1];
	memset(pUnicode, 0, (unicodelen + 1)*sizeof(wchar_t));
	::MultiByteToWideChar(CP_UTF8, 0, str.c_str(), len, (LPWSTR)pUnicode, unicodelen);
	std::wstring  rt;
	rt = (wchar_t *)pUnicode;
	delete  pUnicode;

	return  rt;

};
static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* stream)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)stream);
	if (NULL == str || NULL == buffer)
		return -1;
	char* pData = (char*)buffer;
	str->append(pData, size*nmemb);
	return nmemb;
}
bool CurlSendGets(const std::string& strUrl,std::string& strResponse,const char* pCAPath)
{

	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
		return CURLE_FAILED_INIT;
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCAPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, pCAPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 8);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}
int main()
{
	std::string strResponse;
	bool result = CurlSendGets("https://kyfw.12306.cn/otn/leftTicket/query?leftTicketDTO.train_date=2017-10-28&leftTicketDTO.from_station=BJP&leftTicketDTO.to_station=SHH&purpose_codes=ADULT",strResponse,NULL);
	wprintf(UTF8ToUnicode(strResponse).c_str());
	return 0;
}