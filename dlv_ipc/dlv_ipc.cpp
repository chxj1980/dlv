#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winBase.h>
#include <iostream>
#include <tchar.h>
#include <time.h>
#include <io.h>
#include <direct.h>
#include <process.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dlv_ipc.h"

// ��ipc
#include "../include/dh_ipc/dhnetsdk.h"
#include "../include/dh_ipc/dhconfigsdk.h"

// redis
#include "../include/acl/acl_cpp/lib_acl.hpp"
#include "../include/acl/lib_acl.h"
using namespace std;

// logger
#include "../logger/Logger.h"
using namespace LOGGER;
CLogger g_log(LogLevel_Info, CLogger::GetAppPathA().append("ipc_log\\"));

const LPCWSTR  lpTitle = TEXT("IPC_SERVICE V1.0.190823");
bool g_isExit = false;
const DWORD g_lsize = 4294967290;           // ��netsdk�ص�����(fTimeDownLoadPosCB)�����һ�η������ش�СdwDownLoadSize = 4294967295
HANDLE g_hEvent[DLV_MAX_THREAD];
DLV_DATACB g_datacb[DLV_MAX_THREAD];
DLV_THREAD_PARAM g_param[DLV_MAX_THREAD];

// ��ȡconsole���ڵĴ��ھ��
HWND GetSelfWindow()
{
	return FindWindow(NULL, lpTitle);
}

// ���ƹ����ʾ״̬
void SetCursor(bool flag)
{
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO CursorInfo;
	GetConsoleCursorInfo(handle, &CursorInfo);    // ��ȡ����̨�����Ϣ
	CursorInfo.bVisible = flag;                   // ���ؿ���̨���
	SetConsoleCursorInfo(handle, &CursorInfo);    // ���ÿ���̨���״̬
}

// У��IP�Ƿ�Ϸ�
int check_ip(char* ip)
{
	char s[256] = { 0 };
	int a = -1, b = -1, c = -1, d = -1;

	sscanf_s(ip, "%d.%d.%d.%d%s", &a, &b, &c, &d, s);
	if (a>255 || a<0 || b>255 || b<0 || c>255 || c<0 || d>255 || d<0)
		return -1;
	if (s[0] != 0) 
		return -1;

	return 0;
}

// ����longֵ����
int GetLongLen(unsigned long x)
{
	int leng = 0;

	while (x)
	{
		x /= 10;
		leng++;
	}

	return leng;
}

// �����ļ���
bool MakeDir(const char* fileName)
{
	const char* tag;
	char buf[256] = { 0 };
	char path[256] = { 0 };

	for (tag = fileName; *tag; tag++)
	{
		if (*tag == '\\')
		{
			strcpy_s(buf, fileName);
			buf[strlen(fileName) - strlen(tag) + 1] = NULL;
			strcpy_s(path, buf);

			if (_access(path, 6) == -1)
				if (_mkdir(path) == -1)
					return false;
		}
	}

	return true;
}

string TcharToChar(TCHAR * tchar)
{
	string sz;

	for (TCHAR * s = tchar; *s; s += _tcslen(s) + 1)
	{
		char * _char = new char[100];
		int iLength;

		// ��ȡ�ֽڳ���    
		iLength = WideCharToMultiByte(CP_ACP, 0, s, -1, NULL, 0, NULL, NULL);
		// ��tcharֵ����_char      
		WideCharToMultiByte(CP_ACP, 0, s, -1, _char, iLength, NULL, NULL);
		string t(_char);
		sz.append(t);

		delete[] _char;
	}

	return sz;
}

string getDriveStrings()
{
	TCHAR szBuf[100];
	memset(szBuf, 0, 100);

	DWORD len = GetLogicalDriveStrings(sizeof(szBuf) / sizeof(TCHAR), szBuf);
	string s = TcharToChar(szBuf);

	return s;
}

int checkDrive(string drives, string userPath)
{
	int position;
	char driveTag = userPath.at(0);

	if (driveTag > 'a' && driveTag < 'z')
		driveTag -= 32;

	if ((driveTag > 'A' && driveTag<'Z') || (driveTag>'a' && driveTag < 'z'))
	{
		position = (int)drives.find(driveTag);
		if (position == drives.npos)        // ������̷�������
			return -1;
	}
	else                                    // �Ƿ��̷�
		return -2;

	return 0;
}

bool checkUserPath(string userPath)
{
	bool isRight = true;
	string::iterator it;

	for (it = userPath.begin(); it != userPath.end(); ++it)
	{
		if (*it == '/')
		{
			isRight = false;
			break;
		}
	}

	return isRight;
}

int check_path(const char* userPath)
{
	string drives = getDriveStrings();
	int ret = checkDrive(drives, userPath);

	if (ret != 0)
		return ret;

	if (checkUserPath(userPath) == false)
		return -3;

	return 0;
}

// �ַ�����ʽʱ��תtime
time_t StringToTime(const char* szTime)
{
	const char* cha = szTime;
	tm tm_;
	int year, month, day, hour, minute, second;

	sscanf_s(cha, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
	tm_.tm_year = year - 1900;    // �꣬����tm�ṹ��洢���Ǵ�1900�꿪ʼ��ʱ�䣬����tm_yearΪint��ʱ������ȥ1900
	tm_.tm_mon = month - 1;       // �£�����tm�ṹ����·ݴ洢��ΧΪ0-11������tm_monΪint��ʱ������ȥ1
	tm_.tm_mday = day;
	tm_.tm_hour = hour;
	tm_.tm_min = minute;
	tm_.tm_sec = second;
	tm_.tm_isdst = 0;             // ������ʱ

	time_t t_ = mktime(&tm_);     // ��tm�ṹ��ת����time_t��ʽ
	return t_;
}

void GetNetTime(time_t tm, NET_TIME& tmStartTime, NET_TIME& tmEndTime)
{
	time_t tt_s, tt_e;
	struct tm t_s;
	struct tm t_e;

	tt_s = tm - 5;
	tt_e = tm + 10;

	localtime_s(&t_s, &tt_s);
	localtime_s(&t_e, &tt_e);

	// ת��Ϊnetskd����ʱ���ʽ
	tmStartTime.dwYear = t_s.tm_year + 1900;
	tmStartTime.dwMonth = t_s.tm_mon + 1;
	tmStartTime.dwDay = t_s.tm_mday;
	tmStartTime.dwHour = t_s.tm_hour;
	tmStartTime.dwMinute = t_s.tm_min;
	tmStartTime.dwSecond = t_s.tm_sec;

	tmEndTime.dwYear = t_e.tm_year + 1900;
	tmEndTime.dwMonth = t_e.tm_mon + 1;
	tmEndTime.dwDay = t_e.tm_mday;
	tmEndTime.dwHour = t_e.tm_hour;
	tmEndTime.dwMinute = t_e.tm_min;
	tmEndTime.dwSecond = t_e.tm_sec;
}

// IPC���߻ص�
void CALL_METHOD Disconnect(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser)
{
	printf_s("->> Receive disconnect message, where ip:[%s] and port:[%d] and lHandle:[%lld].\n",
		pchDVRIP, nDVRPort, lLoginID);
}

// IPC���ػص�
int CALL_METHOD fDataCB(LLONG lRealHandle, DWORD dwDataType, BYTE *pBuffer, DWORD dwBufSize, LDWORD dwUser)
{
	// ��Ҫ,�ص�����Ҫ������ת�����������ж� dwDataType
	if (dwDataType != NET_DATA_CALL_BACK_VALUE + EM_REAL_DATA_TYPE_GBPS
		&& dwDataType != NET_DATA_CALL_BACK_VALUE + EM_REAL_DATA_TYPE_TS
		&& dwDataType != NET_DATA_CALL_BACK_VALUE + EM_REAL_DATA_TYPE_MP4
		&& dwDataType != NET_DATA_CALL_BACK_VALUE + EM_REAL_DATA_TYPE_H264)
	{
		//printf_s("->> �������ʹ���: handle[%lld], datatype[%lld], size[%lld], user[%lld].\n",
		//	lRealHandle, dwDataType, dwBufSize, dwUser);
		return 1;
	}
	if (((DLV_DATACB*)dwUser)->file == NULL)
	{
		printf_s("->> �ļ����Ϊ��: handle[%lld], datatype[%lu], size[%lu], user[%lld].\n",
			lRealHandle, dwDataType, dwBufSize, dwUser);
		return 1;
	}
	if (fwrite(pBuffer, dwBufSize, 1, ((DLV_DATACB*)dwUser)->file) == 0)
	{
		printf_s("->> д�ļ�ʧ��: handle[%lld], datatype[%lu], size[%lu], user[%lld].\n",
			lRealHandle, dwDataType, dwBufSize, dwUser);
		return -1;
	}
	if (((DLV_DATACB*)dwUser)->fileFlushFlag++ % 40 == 0)
	{
		((DLV_DATACB*)dwUser)->fileFlushFlag = 1;
		fflush(((DLV_DATACB*)dwUser)->file);

		printf_s("--> д��־λ handle[%lld], datatype[%lu], size[%lu], user[%lld].\n",
			lRealHandle, dwDataType, dwBufSize, dwUser);
	}
	return 1;
}

// ��ʱ��طŽ��Ȼص�����ԭ��
void CALL_METHOD fTimeDownLoadPosCB(LLONG lPlayHandle, DWORD dwTotalSize, DWORD dwDownLoadSize, int index, NET_RECORDFILE_INFO recordfileinfo, LDWORD dwUser)
{	
	if (dwDownLoadSize > g_lsize)
		SetEvent((HANDLE)dwUser);

	return;
}

// redis
// ��֤����
bool redis_auth(acl::redis_connection& redis, const char* szpwd)
{
	acl::string passwd(szpwd);

	redis.clear();
	if (redis.auth(passwd.c_str()) == false)
		return false;

	return true;
}

bool redis_select(acl::redis_connection& redis, int n)
{
	redis.clear();
	if (redis.select(n) == false)
		return false;

	return true;
}

bool redis_hmget(acl::redis_hash& redis, const char* key_ipc, DLV_IPC_LOGIN& dlv_ipc)
{
	acl::string key(key_ipc);
	acl::string attr0, attr1, attr2, attr3, attr4, attr5;
	const char* attrs[6];
	std::vector<acl::string> result;

	attr0.format("ip");
	attr1.format("port");
	attr2.format("user");
	attr3.format("pwd");
	attr4.format("region");
	attr5.format("devcode");
	attrs[0] = attr0.c_str();
	attrs[1] = attr1.c_str();
	attrs[2] = attr2.c_str();
	attrs[3] = attr3.c_str();
	attrs[4] = attr4.c_str();
	attrs[5] = attr5.c_str();

	result.clear();
	redis.clear();
	if (redis.hmget(key, attrs, 6, &result) == false)
		return false;

	if (redis.result_value(0) == NULL)
		return false;

	memcpy(dlv_ipc.szIpAddr, redis.result_value(0), sizeof(dlv_ipc.szIpAddr));
	dlv_ipc.nPort = atoi(redis.result_value(1));
	memcpy(dlv_ipc.szUser, redis.result_value(2), sizeof(dlv_ipc.szUser));
	memcpy(dlv_ipc.szPwd, redis.result_value(3), sizeof(dlv_ipc.szPwd));
	dlv_ipc.nChan = 0;
	memcpy(dlv_ipc.szRegion, redis.result_value(4), sizeof(dlv_ipc.szRegion));
	memcpy(dlv_ipc.szDevCode, redis.result_value(5), sizeof(dlv_ipc.szDevCode));

	return true;
}

bool redis_rpop(acl::redis_list &redis, const char* key_rec, char* szTime, size_t size)
{
	int ret;
	acl::string buf, __key(key_rec);

	redis.clear();
	buf.clear();
	ret = redis.rpop(__key, buf);
	if (ret <= 0)
		return false;

	memcpy(szTime, buf.c_str(), size);

	return true;
}
// redis

unsigned int __stdcall ThreadFun(PVOID pM)
{
	DLV_THREAD_PARAM* pParam = (DLV_THREAD_PARAM*)pM;
	acl::string key_ipc, key_rec;
	acl::string __key_ipc("ipc");
	acl::string __key_rec("rec");
	acl::string addr(pParam->szRedisIpAddr), err;	
	int conn_timeout = 10, rw_timeout = 10;
	bool cluster_mode = false;
	bool slice_req = false;

	acl::acl_cpp_init();
	acl::log::stdout_open(false);
	acl::redis_client client(addr.c_str(), conn_timeout, rw_timeout);
	client.set_slice_request(slice_req);
	acl::redis_connection redis_con(&client);
	err = redis_con.result_error();

	if (err != "�����ɹ���ɡ�")
	{
	    printf_s("->> channel[%-2d], redis���Ӵ���: [%s], [������(E)�˳�����, ���³���].\n", pParam->nQueue, err.c_str());
		return 0;
    }
	else
		printf_s("--> channel[%-2d], redis����: [%s].\n", pParam->nQueue, err.c_str());

	if (redis_auth(redis_con, pParam->szRedisPwd) == false)
	{
		printf_s("->> channel[%-2d], redis������֤����. [������(E)�˳�����, ���³���].\n", pParam->nQueue);
		return 0;
	}
	else
		printf_s("--> channel[%-2d], redis������֤�ɹ�.\n", pParam->nQueue);

	if (redis_select(redis_con, pParam->nRedisDB) == false)
	{
		printf_s("->> channel[%-2d], redisѡ�����ݿ�[%d]����. [������(E)�˳�����, ���³���].\n", pParam->nQueue, pParam->nRedisDB);
		return 0;
	}
	else
		printf_s("--> channel[%-2d], redisѡ�����ݿ�[%d]�ɹ�.\n", pParam->nQueue, pParam->nRedisDB);

	int nErrcode = 0;
	int nSuccess = 0, nFail = 0;
	DWORD lastErr = 0;
	NET_DEVICEINFO_Ex stLoginInfo = { 0 };
	DLV_IPC_LOGIN dlv_ipc[DLV_MAX_IPC];
	for (int i = 0; i < DLV_MAX_IPC; i++)
	{
		// ��ʼ���ṹ��
		dlv_ipc[i].lLoginHandle = 0;
		dlv_ipc[i].nChan = 0;
		dlv_ipc[i].nPort = 37777;
		dlv_ipc[i].bDisconn = false;
		memcpy_s(dlv_ipc[i].szIpAddr, sizeof(dlv_ipc[i].szIpAddr), "", sizeof(dlv_ipc[i].szIpAddr));
		memcpy_s(dlv_ipc[i].szPwd, sizeof(dlv_ipc[i].szPwd), "", sizeof(dlv_ipc[i].szPwd));
		memcpy_s(dlv_ipc[i].szUser, sizeof(dlv_ipc[i].szUser), "", sizeof(dlv_ipc[i].szUser));
		memcpy_s(dlv_ipc[i].szRegion, sizeof(dlv_ipc[i].szRegion), "", sizeof(dlv_ipc[i].szRegion));
		memcpy_s(dlv_ipc[i].szDevCode, sizeof(dlv_ipc[i].szDevCode), "", sizeof(dlv_ipc[i].szDevCode));
	}

	acl::redis_hash redis_h;
	redis_h.set_client(&client);
	for (int i = 0; i < DLV_MAX_IPC; i++)
	{
		// ��ȡIPC��¼��Ϣ
		key_ipc.format("%s_%d_%d", __key_ipc.c_str(), pParam->nQueue, i);
		if (redis_hmget(redis_h, key_ipc.c_str(), dlv_ipc[i]) == true)
		{
			// ��¼IPC
			dlv_ipc[i].lLoginHandle = CLIENT_LoginEx2(dlv_ipc[i].szIpAddr, dlv_ipc[i].nPort, dlv_ipc[i].szUser,
				                                      dlv_ipc[i].szPwd, (EM_LOGIN_SPAC_CAP_TYPE)0, NULL, &stLoginInfo, &nErrcode);
			if (dlv_ipc[i].lLoginHandle != 0)
			{
				nSuccess++;
				printf_s("--> channel[%-2d], IPC[%s]��¼�ɹ�, handle[%lld].\n", pParam->nQueue, dlv_ipc[i].szIpAddr, dlv_ipc[i].lLoginHandle);
			}
			else
			{
				nFail++;
				printf_s("->> channel[%-2d], IPC[%s]��¼ʧ��, ������[%lu].\n", pParam->nQueue, dlv_ipc[i].szIpAddr, CLIENT_GetLastError());
			}	
		}
		else
		{
			printf_s("--> channel[%-2d], ������IPC[%d]̨, ������[%d]̨, ��¼�ɹ�[%d]̨, ��¼ʧ��[%d]̨.\n", 
				          pParam->nQueue, DLV_MAX_IPC, nSuccess + nFail, nSuccess, nFail);
			break;
		}	
	}
	
	char* p = NULL;
	char* ptr = NULL;
	char szIp[32] = "";
	char szRec[64] = "";
	char szRecTime[32] = "";
	char szFilePath[256] = "";
	int nQueue = -1;
	int nY, nM, nD, nH, nMin, nS;
	LLONG lHandle = 0;
	time_t tt_start, tt_cur;
	time_t tt = 0;
	NET_TIME tmStartTime = { 0 };
	NET_TIME tmEndTime = { 0 };
	NET_IN_DOWNLOAD_BY_DATA_TYPE stIn = { sizeof(stIn) };
	NET_OUT_DOWNLOAD_BY_DATA_TYPE stOut = { sizeof(stOut) };
	EM_REAL_DATA_TYPE emSCtype = EM_REAL_DATA_TYPE_MP4;
	stIn.fDownLoadDataCallBack = fDataCB;
	stIn.cbDownLoadPos = fTimeDownLoadPosCB;
	stIn.emDataType = emSCtype;
	stIn.dwPosUser = (LDWORD)g_hEvent[pParam->nQueue];
	stIn.dwDataUser = (LDWORD)&g_datacb[pParam->nQueue];

	key_rec.format("%s_%d", __key_rec.c_str(), pParam->nQueue);

	// ��redis��Ϣ�����в�ͣ��ȡ��¼, ������Ƶ
	acl::redis_list redis_li(&client);
	for (;;)
	{
		if (g_isExit == true)
		{
			printf_s("--> channel[%-2d], �յ��˳�ָ��, �����˳�...\n", pParam->nQueue);
			break;
		}

		if (redis_rpop(redis_li, key_rec.c_str(), szRec, sizeof(szRec)) == false)        // ��ȡlist�м�¼
		{
			err = redis_li.result_error();        // ���redis�Ƿ����
			if (err == "NOAUTH Authentication required.")
			{
				redis_auth(redis_con, pParam->szRedisPwd);
				redis_select(redis_con, pParam->nRedisDB);
				printf_s("--> channel[%-2d], redis_rpop������֤����ɹ�.\n", pParam->nQueue);

				err = redis_li.result_error();
			}
			if (err != "�����ɹ���ɡ�")
				printf_s("->> channel[%-2d], redis_rpop����: [%s]\n", pParam->nQueue, err.c_str());

			Sleep(DLV_EMPTY_WAIT);
			continue;
		}
		else
			printf_s("--> channel[%-2d], redis_rpop���ؼ�¼[%s], ��ʼ����...\n", pParam->nQueue, szRec);

		p = NULL;
		ptr = NULL;
		ptr = strtok_s(szRec, ",", &p);
		if (ptr != NULL)
			memcpy(szIp, ptr, sizeof(szIp));
		else
			continue;
		ptr = strtok_s(NULL, ",", &p);
		if (ptr != NULL)
			memcpy(szRecTime, ptr, sizeof(szRecTime));
		else
			continue;

		// ƥ�����ؼ�¼�ж�Ӧ��IPC
		nQueue = -1;
		for (int i = 0; i < DLV_MAX_IPC; i++)
		{
			if (strcmp(dlv_ipc[i].szIpAddr, szIp) == 0)
			{
				nQueue = i;
				break;
			}	
		}
		if (nQueue == -1)
			continue;

		// ����ʱ��
		tt = StringToTime(szRecTime);
		GetNetTime(tt, tmStartTime, tmEndTime);
		// ����·��
		sscanf_s(szRecTime, "%d-%d-%d %d:%d:%d", &nY, &nM, &nD, &nH, &nMin, &nS);
		if (pParam->szFilePath[strlen(pParam->szFilePath) - 1] == '\\')
			sprintf_s(szFilePath, sizeof(szFilePath), "%s%s\\%s\\%04d%02d%02d\\%04d%02d%02d%02d%02d%02d.mp4", 
			pParam->szFilePath, dlv_ipc[nQueue].szRegion, dlv_ipc[nQueue].szDevCode, nY, nM, nD, nY, nM, nD, nH, nMin, nS);
		else
			sprintf_s(szFilePath, sizeof(szFilePath), "%s\\%s\\%s\\%04d%02d%02d\\%04d%02d%02d%02d%02d%02d.mp4", 
			pParam->szFilePath, dlv_ipc[nQueue].szRegion, dlv_ipc[nQueue].szDevCode, nY, nM, nD, nY, nM, nD, nH, nMin, nS);

		if (MakeDir(szFilePath) == false)
		{
			printf_s("->> channel[%-2d], �޷���������·��[%s].\n", pParam->nQueue, szFilePath);
			// ���log
			g_log.TraceError("�Ƿ�·��. lpush rec_%d \"%s,%s\"\n", pParam->nQueue, dlv_ipc[nQueue].szIpAddr, szRecTime);
			continue;
		}

		// �ж��Ƿ�����IPC
		if (dlv_ipc[nQueue].bDisconn == true)
		{
			dlv_ipc[nQueue].lLoginHandle = CLIENT_LoginEx2(dlv_ipc[nQueue].szIpAddr, dlv_ipc[nQueue].nPort, dlv_ipc[nQueue].szUser,
				dlv_ipc[nQueue].szPwd, (EM_LOGIN_SPAC_CAP_TYPE)0, NULL, &stLoginInfo, &nErrcode);
			if (dlv_ipc[nQueue].lLoginHandle != 0)
			{
				dlv_ipc[nQueue].bDisconn = false;
				printf_s("--> channel[%-2d], IPC[%s]�����ɹ�, handle[%lld].\n", pParam->nQueue, dlv_ipc[nQueue].szIpAddr, dlv_ipc[nQueue].lLoginHandle);
			}
			else
			{
				lastErr = CLIENT_GetLastError();
				printf_s("->> channel[%-2d], IPC[%s]����ʧ��, �������[%lu].\n", pParam->nQueue, dlv_ipc[nQueue].szIpAddr, lastErr);
				// ���log
				g_log.TraceError("����ʧ��. lpush rec_%d \"%s,%s\"\n", pParam->nQueue, dlv_ipc[nQueue].szIpAddr, szRecTime);

				continue;
			}	
		}

		// ������Ƶ
		lHandle = 0;
		g_datacb[pParam->nQueue].file = NULL;
		stIn.stStartTime = tmStartTime;
		stIn.stStopTime = tmEndTime;
		stIn.szSavedFileName = szFilePath;
		stIn.nChannelID = 0;
		time(&tt_start);

		lHandle = CLIENT_DownloadByDataType(dlv_ipc[nQueue].lLoginHandle, &stIn, &stOut, DLV_MAX_WAITEVENT);
		if (lHandle == 0)
		{
			lastErr = CLIENT_GetLastError();
            if (lastErr == NET_NO_RECORD_FOUND)
			{
				printf_s("->> channel[%-2d], IPC[%s], ������Ƶ[%s]ʧ��, �������[%lu]:[��ѯ����¼��].\n", pParam->nQueue, dlv_ipc[nQueue].szIpAddr, szFilePath, lastErr);
				// ���log
				g_log.TraceError("����¼��. rec_%d \"%s,%s\"\n", pParam->nQueue, dlv_ipc[nQueue].szIpAddr, szRecTime);
			}
			else
			{
				if (dlv_ipc[nQueue].lLoginHandle != 0)
					CLIENT_Logout(dlv_ipc[nQueue].lLoginHandle);
				dlv_ipc[nQueue].lLoginHandle = 0;
				dlv_ipc[nQueue].bDisconn = true;

				printf_s("->> channel[%-2d], IPC[%s], ������Ƶ[%s]ʧ��, �������[%lu]:[IPC�ѵ���].\n", pParam->nQueue, dlv_ipc[nQueue].szIpAddr, szFilePath, lastErr);
				// ���log
				g_log.TraceError("�豸����. lpush rec_%d \"%s,%s\"\n", pParam->nQueue, dlv_ipc[nQueue].szIpAddr, szRecTime);
			}
		}
		else
		{
			if (WaitForSingleObject(g_hEvent[pParam->nQueue], DLV_MAX_WAITEVENT) == WAIT_OBJECT_0)
			{
				time(&tt_cur);
				ResetEvent(g_hEvent[pParam->nQueue]);
				printf_s("--> channel[%-2d], IPC[%s], ������Ƶ[%s]�ɹ�, ��ʱ[%lld]��.\n", pParam->nQueue, dlv_ipc[nQueue].szIpAddr, szFilePath, tt_cur - tt_start);
			}
			else
			{
				lastErr = CLIENT_GetLastError();
				printf_s("->> channel[%-2d], IPC[%s], ������Ƶ[%s]��ʱ, handle[%lld], �������[%lu].\n", pParam->nQueue, dlv_ipc[nQueue].szIpAddr, szFilePath, lHandle, lastErr);
				// ���log
				g_log.TraceError("���س�ʱ. lpush rec_%d \"%s,%s\"\n", pParam->nQueue, dlv_ipc[nQueue].szIpAddr, szRecTime);
			}
				
			if (CLIENT_StopDownload(lHandle) == false)
				printf_s("->> channel[%-2d], IPC[%s], StopDownloadʧ��, �������[%lu].\n", pParam->nQueue, dlv_ipc[nQueue].szIpAddr, CLIENT_GetLastError());
		}
		// ������Ƶ
	}

	for (int i = 0; i < nSuccess; i++)
		if (dlv_ipc[i].lLoginHandle != 0)
			CLIENT_Logout(dlv_ipc[i].lLoginHandle);

	return 1;
}

int main(int argc, char* argv[])
{
	SetConsoleTitle(lpTitle);
	
	time_t tt_start;
	time(&tt_start);

	char cmd = 0;
	char szIpAddr[64] = { 0 };
	char szPwd[32] = { 0 };
	char szFilePath[64] = { 0 };
	char szPort[8] = { 0 };
	char szDb[8] = { 0 };
	char szQueue[8] = { 0 };
	int nDB = 0, nPort = 0, nThread_Num = 0;
	size_t size = 0;
	HANDLE handle[DLV_MAX_THREAD];

	getenv_s(&size, szIpAddr, sizeof(szIpAddr), "redis_ip");
	if (size == 0)
	{
		printf_s("<-- ������redis��ip��ַ:\n");
		for (;;)
		{
			scanf_s("%s", szIpAddr, sizeof(szIpAddr));
			scanf_s("%*[^\n]");
			if (check_ip(szIpAddr) == 0)
				break;
			else
				printf_s("->> ip��ʽ����. ������redis��ip��ַ:\n");
		}
	}

	getenv_s(&size, szPort, sizeof(szPort), "redis_port");
	if (size == 0)
	{
		printf_s("<-- ������redis�Ķ˿ں�:\n");
		for (;;)
		{
			scanf_s("%d", &nPort);
			scanf_s("%*[^\n]");
			if (nPort <= 0 || nPort > 65535)
				printf_s("->> �˿�ֵ����. ������redis�Ķ˿ں�:\n");
			else
				break;
		}
		// ���acl�����ַ���
		sprintf_s(szIpAddr, sizeof(szIpAddr), "%s:%d", szIpAddr, nPort);
	}
	else
		sprintf_s(szIpAddr, sizeof(szIpAddr), "%s:%s", szIpAddr, szPort);


	getenv_s(&size, szPwd, sizeof(szPwd), "redis_pwd");
	if (size == 0)
	{
		printf_s("<-- ������redis����֤����(1~32���ַ�):\n");
		for (;;)
		{
			scanf_s("%s", szPwd, sizeof(szPwd));
			scanf_s("%*[^\n]");
			if (strlen(szPwd) == 0)
				printf_s("->> ���볤�ȴ���. ������redis����֤����(1~32���ַ�):\n");
			else
				break;
		}
	}

	getenv_s(&size, szDb, sizeof(szDb), "redis_db");
	if (size == 0)
	{
		printf_s("<-- ������redis�����ݿ���(0~15):\n");
		for (;;)
		{
			int ret = scanf_s("%d", &nDB);
			scanf_s("%*[^\n]");
			if (ret == 0 || nDB > 15 || nDB < 0)
				printf_s("->> ���ݿ��Ŵ���. ������redis�����ݿ���(0~15):\n");
			else
				break;
		}
	}
	else
		nDB = atoi(szDb);

	getenv_s(&size, szQueue, sizeof(szQueue), "redis_queue");
	if (size == 0)
	{
		printf_s("<-- ������redis��Ϣ��������(1~20):\n");
		for (;;)
		{
			scanf_s("%d", &nThread_Num);
			scanf_s("%*[^\n]");
			if (nThread_Num > 20 || nThread_Num < 1)
				printf_s("->> ��Ϣ������������. ������redis��Ϣ��������(1~20):\n");
			else
				break;
		}
	}
	else
		nThread_Num = atoi(szQueue);

	getenv_s(&size, szFilePath, sizeof(szFilePath), "video_path");
	if (size == 0)
	{
		printf_s("<-- ������¼���ļ��洢��Ŀ¼·��(1~64�ַ�):\n");
		for (;;)
		{
			scanf_s("%s", szFilePath, sizeof(szFilePath));
			scanf_s("%*[^\n]");

			int ret = check_path(szFilePath);
			if (ret == -1)
				printf_s("->> �̷�������. ������¼���ļ��洢��Ŀ¼·��(2~64�ַ�):\n");
			else if (ret == -2)
				printf_s("->> �Ƿ��̷�. ������¼���ļ��洢��Ŀ¼·��(2~64�ַ�):\n");
			else if (ret == -3)
				printf_s("->> �Ƿ�·���ַ�. ������¼���ļ��洢��Ŀ¼·��(2~64�ַ�):\n");
			else
			{
				if (strlen(szFilePath) == 1)
					sprintf_s(szFilePath, sizeof(szFilePath), "%s:", szFilePath);
				break;
			}
		}
	}

	printf_s("--> [IPC_SERVICE]������������...\n");	
	printf_s("--> �˳�����������(E)\n");

	// ��ʼ��SDK��Դ,���ö��߻ص�����
	CLIENT_Init(Disconnect, NULL);

	for (int i = 0; i < nThread_Num; i++)
	{
		g_hEvent[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
		g_datacb[i].file = NULL;
		g_datacb[i].fileFlushFlag = 1;

		memcpy(g_param[i].szRedisIpAddr, szIpAddr, sizeof(g_param[i].szRedisIpAddr));
		memcpy(g_param[i].szRedisPwd, szPwd, sizeof(g_param[i].szRedisPwd));
		memcpy(g_param[i].szFilePath, szFilePath, sizeof(g_param[i].szFilePath));
		g_param[i].nRedisDB = nDB;
		g_param[i].nQueue = i;

		handle[i] = (HANDLE)_beginthreadex(NULL, 0, ThreadFun, &g_param[i], 0, NULL);
	}

	for (;;)
	{
		cmd = getchar();
		if (cmd == 'e' || cmd == 'E')
		{
			printf_s("--> [IPC_SERVICE]���������˳� ...\n");
			g_isExit = true;
			break;
		}
		else if (cmd == 't' || cmd == 'T')
		{
			time_t tt_cur;
			time(&tt_cur);

			tt_cur = tt_cur - tt_start;
			printf_s("--> ��������������[%lld]Сʱ[%lld]��[%lld]��.\n", tt_cur / 3600, (tt_cur%3600)/60, tt_cur%3600%60);
		}
	}

	// �ȴ������߳̽���
	WaitForMultipleObjects(nThread_Num, handle, true, INFINITE);
	// �����߳���Դ
	for (int i = 0; i < nThread_Num; i++)
	{
		if (handle[i] != 0)
		    CloseHandle(handle[i]);
		if (g_hEvent[i] != 0)
		    CloseHandle(g_hEvent[i]);
	}	

	// ����SDK��Դ
	CLIENT_Cleanup();

	return 0;
}
