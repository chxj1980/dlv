#ifndef	DLV_DSS_INCLUDE_H
#define	DLV_DSS_INCLUDE_H

# ifdef	__cplusplus
extern "C" {
# endif

#define DLV_MAX_WAITEVENT    60000  // ������Ƶʱ���������ʱ��60��
#define DLV_EMPTY_WAIT		 3000   // redis���п��У��߳�����ʱ��Ĭ��3��
#define DLV_MAX_THREAD       20     // ����Ӧ�ó������֧��20���߳�
#define DLV_MAX_IPC          20     // ���߳����֧��20��IPC

	typedef struct tagDLV_DSS_IPC
	{
		char                 szIpcIp[32];              // 7016������������IPC��ip��ַ
		char                 szChan[32];			   // ICP��7016�ϵ�ͨ����
		char                 szRegion[32];	           // IPC���������������
		char                 szDevCode[32];            // IPC��Ӧ�ſڻ�����
	} DLV_DSS_IPC;

	// �̴߳���ṹ��
	typedef struct tagDLV_THREAD_PARAM
	{
		char                 szRedisIpAddr[32];        // redis����ip��ַ
		char                 szRedisPwd[32];           // redis������֤����
		int                  nRedisDB;                 // redis����ѡ������ݿ���(0~15)
		int                  nQueue;                   // redis������Ϣ����
		char                 szFilePath[64];           // ¼����Ƶ�洢�ĸ�Ŀ¼·��(d:\video)
	} DLV_THREAD_PARAM;

	typedef struct tagDLV_DATACB
	{
		FILE*                file;					   // �����ݻص������ļ����, Ĭ��NULL
		int                  fileFlushFlag;            // �ļ����ˢ�±�־, Ĭ��1
	} DLV_DATACB;

# ifdef	__cplusplus
}
# endif

#endif