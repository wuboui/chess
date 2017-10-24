#include<stdio.h>
#include <stdlib.h>
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

	cJSON_Delete(pJson);
}
int main()
{
	char* p = makeJson();
	if (NULL == p)
	{
        printf("Make Faild \n");
        return 0;
	}
    printf("%s \n", p);
    parseJson(p);
    free(p);
    return 0;
}