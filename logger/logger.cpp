// logger.cpp
#include "logger.h"
#include <time.h>
#include <stdarg.h>
#include <direct.h>
#include <vector>
#include <Dbghelp.h>
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#pragma comment(lib,"Dbghelp.lib")
using std::string;
using std::vector;

namespace LOGGER
{
	CLogger::CLogger(EnumLogLevel nLogLevel, const std::string strLogPath, const std::string strLogName)
		:m_nLogLevel(nLogLevel),
		m_strLogPath(strLogPath),
		m_strLogName(strLogName)
	{
		m_fh = 0;
		m_uiSize = 0;
		m_nLogRow = 0;
		m_pFileStream = NULL;

		if (m_strLogPath.empty())
		{
			m_strLogPath = GetAppPathA();
		}
		if (m_strLogPath[m_strLogPath.length() - 1] != '\\')
		{
			m_strLogPath.append("\\");
		}
		
		MakeSureDirectoryPathExists(m_strLogPath.c_str()); /**< �����ļ��� */
		/**
	     * ������־�ļ�
	     */
		if (m_strLogName.empty())
		{
			time_t curTime;
			time(&curTime);
			tm tm1;
			localtime_s(&tm1, &curTime);
			/**
		     * ��־�����ƣ�201601012130.log
		     */
			m_strLogName = FormatString("%04d%02d%02d%02d%02d%02d.log", tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
		}
		m_strLogName = m_strLogPath + m_strLogName;

		/**
		 * ��׷�ӵķ�ʽ���ļ���
		 */
		errno_t err = _sopen_s(&m_fh, m_strLogName.c_str(), _O_CREAT | _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE);

		InitializeCriticalSection(&m_cs);
	}

	CLogger::~CLogger()
	{
		DeleteCriticalSection(&m_cs); /**< �ͷ��ٽ��� */

		if (m_fh != 0) 
			_close(m_fh); /**< �ر��ļ� */
	}

	const char *CLogger::path_file(const char *path, char splitter)
	{
		return strrchr(path, splitter) ? strrchr(path, splitter) + 1 : path;
	}

	void CLogger::TraceFatal(const char *lpcszFormat, ...)
	{
		if (EnumLogLevel::LogLevel_Fatal > m_nLogLevel)
			return;

		string strResult;
		if (lpcszFormat != NULL)
		{
			va_list marker = NULL;
			va_start(marker, lpcszFormat); /**< ��ʼ���������� */
			size_t nLength = _vscprintf(lpcszFormat, marker) + 1; /**< ��ȡ��ʽ���ַ������� */
			std::vector<char> vBuffer(nLength, '\0'); /**< �������ڴ洢��ʽ���ַ������ַ����� */
			int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, lpcszFormat, marker);
			if (nWritten > 0)
			{
				strResult = &vBuffer[0];
			}
			va_end(marker);
		}
		if (strResult.empty())
			return;

		string strLog = strFatalPrefix;
		strLog.append(GetTime()).append(strResult);

		Trace(strLog);
	}

	void CLogger::TraceError(const char *lpcszFormat, ...)
	{
		if (EnumLogLevel::LogLevel_Error > m_nLogLevel)
			return;

		string strResult;
		if (lpcszFormat != NULL)
		{
			va_list marker = NULL;
			va_start(marker, lpcszFormat);
			size_t nLength = _vscprintf(lpcszFormat, marker) + 1;
			std::vector<char> vBuffer(nLength, '\0');
			int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, lpcszFormat, marker);
			if (nWritten > 0)
			{
				strResult = &vBuffer[0];
			}
			va_end(marker);
		}
		if (strResult.empty())
			return;

		string strLog = strErrorPrefix;
		strLog.append(GetTime()).append(strResult);

		Trace(strLog);
	}

	void CLogger::TraceWarning(const char *lpcszFormat, ...)
	{
		if (EnumLogLevel::LogLevel_Warning > m_nLogLevel)
			return;

		string strResult;
		if (lpcszFormat != NULL)
		{
			va_list marker = NULL;
			va_start(marker, lpcszFormat);
			size_t nLength = _vscprintf(lpcszFormat, marker) + 1;
			std::vector<char> vBuffer(nLength, '\0');
			int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, lpcszFormat, marker);
			if (nWritten > 0)
			{
				strResult = &vBuffer[0];
			}
			va_end(marker);
		}
		if (strResult.empty())
			return;

		string strLog = strWarningPrefix;
		strLog.append(GetTime()).append(strResult);

		Trace(strLog);
	}

	void CLogger::TraceInfo(const char *lpcszFormat, ...)
	{
		if (EnumLogLevel::LogLevel_Info > m_nLogLevel)
			return;

		string strResult;
		if (lpcszFormat != NULL)
		{
			va_list marker = NULL;
			va_start(marker, lpcszFormat);
			size_t nLength = _vscprintf(lpcszFormat, marker) + 1;
			std::vector<char> vBuffer(nLength, '\0');
			int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, lpcszFormat, marker);
			if (nWritten > 0)
			{
				strResult = &vBuffer[0];
			}
			va_end(marker);
		}
		if (strResult.empty())
			return;

		string strLog = strInfoPrefix;
		strLog.append(GetTime()).append(strResult);

		Trace(strLog);
	}

	string CLogger::GetTime()
	{
		time_t curTime;
		time(&curTime);
		tm tm1;
		localtime_s(&tm1, &curTime);

		string strTime = FormatString("%04d-%02d-%02d %02d:%02d:%02d ", 
			tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);

		return strTime;
	}

	void CLogger::ChangeLogLevel(EnumLogLevel nLevel)
	{
		m_nLogLevel = nLevel;
	}

	void CLogger::Trace(const string &strLog)
	{
		try
		{
			EnterCriticalSection(&m_cs); /**< �����ٽ��� */

			if (m_nLogRow >= MAX_ROWS)
			{
				if (m_fh != 0)
					_close(m_fh);
				m_fh = 0;
				/**
                 * ��������־
                 */
				time_t curTime;
				time(&curTime);
				tm tm1;
				localtime_s(&tm1, &curTime);

				m_strLogName = FormatString("%04d%02d%02d%02d%02d%02d.log", 
					tm1.tm_year + 1900, tm1.tm_mon + 1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec);
				m_strLogName = m_strLogPath + m_strLogName;
				_sopen_s(&m_fh, m_strLogName.c_str(), _O_CREAT | _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE);
				if (m_fh != 0)
					m_nLogRow = 0;
			}

			if (m_fh == 0) /**< ȷ���ļ��� */
			{
				errno_t err = _sopen_s(&m_fh, m_strLogName.c_str(), _O_CREAT | _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE);
				if (err != 0)
					return;
			}
			/**
			 * д��־��Ϣ���ļ�
			 */
			m_uiSize = (unsigned int)strLog.length();
			_write(m_fh, strLog.c_str(), m_uiSize);
			m_nLogRow++;

			LeaveCriticalSection(&m_cs); /**< �뿪�ٽ��� */
		}
		/**
		 * �������쳣�������뿪�ٽ�������ֹ����
		 */
		catch (...)
		{
			LeaveCriticalSection(&m_cs);
		}
	}

	string CLogger::GetAppPathA()
	{
		char szFilePath[MAX_PATH] = { 0 }, szDrive[MAX_PATH] = { 0 }, szDir[MAX_PATH] = { 0 }, szFileName[MAX_PATH] = { 0 }, szExt[MAX_PATH] = { 0 };
		GetModuleFileNameA(NULL, szFilePath, sizeof(szFilePath));
		_splitpath_s(szFilePath, szDrive, szDir, szFileName, szExt);

		string str(szDrive);
		str.append(szDir);
		return str;
	}

	string CLogger::FormatString(const char *lpcszFormat, ...)
	{
		string strResult;
		if (lpcszFormat != NULL)
		{
			va_list marker = NULL;
			va_start(marker, lpcszFormat);
			size_t nLength = _vscprintf(lpcszFormat, marker) + 1;
			std::vector<char> vBuffer(nLength, '\0');
			int nWritten = _vsnprintf_s(&vBuffer[0], vBuffer.size(), nLength, lpcszFormat, marker);
			if (nWritten > 0)
			{
				strResult = &vBuffer[0];
			}
			va_end(marker);
		}

		return strResult;
	}
}