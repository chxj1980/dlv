/**
 * @file	dlv_ipc.h
 * @author	jayxu
 * @date	2019-8-01
 * @brief	���ļ��ж����� DLV_IPC_LOGIN��DLV_THREAD_PARAM ��������.
 * @version	1.0
 */

#ifndef	DLV_IPC_INCLUDE_H
#define	DLV_IPC_INCLUDE_H

# ifdef	__cplusplus
extern "C" {
# endif

#define DLV_MAX_THREAD       20		/**< ����console֧�ֵ��߳�����һ����Ϣ����һ���߳� */
#define DLV_MAX_IPC          20     /**< ���߳������IPC���� */
#define DLV_MAX_WAITEVENT    60000  /**< ������Ƶʱ���������ʱ��60�� */
#define DLV_EMPTY_WAIT		 3000   /**< redis���п��У��߳�����ʱ��Ĭ��3�� */
 
/**
 * IPC��¼��Ϣ�ṹ��
 */
typedef struct tagDLV_IPC_LOGIN
{
	__int64              lLoginHandle;    /**< IPC��¼��� */
	char 				 szIpAddr[32];    /**< IP��ַ */
	char 				 szUser[32];      /**< ��¼�û� */
	char 				 szPwd[32];       /**< ��¼���� */
	int                  nPort;           /**< �˿ں�, Ĭ��37777 */
	int                  nChan;			  /**< ¼��ͨ��, Ĭ��0 */
	bool				 bDisconn;        /**< IPC�Ƿ���� */
	char                 szRegion[32];	  /**< IPC��������������� */
	char                 szDevCode[32];   /**< IPC��Ӧ�ſڻ����� */
} DLV_IPC_LOGIN;

/**
 * �̴߳���ṹ��
 */
typedef struct tagDLV_THREAD_PARAM
{
	char                 szRedisIpAddr[32];        /**< redis����ip��ַ */
	char                 szRedisPwd[32];           /**< redis������֤���� */
	int                  nRedisDB;                 /**< redis����ѡ������ݿ���(0~15) */
	int                  nQueue;                   /**< redis������Ϣ���� */
	char                 szFilePath[64];           /**< ¼����Ƶ�洢�ĸ�Ŀ¼·��(d:\video) */
} DLV_THREAD_PARAM;

typedef struct tagDLV_DATACB
{
	FILE*                file;					   /**< �����ݻص������ļ����, Ĭ��NULL */
	int                  fileFlushFlag;            /**< �ļ����ˢ�±�־, Ĭ��1 */
} DLV_DATACB;

# ifdef	__cplusplus
}
# endif

#endif