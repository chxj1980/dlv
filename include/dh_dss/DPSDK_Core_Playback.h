#ifndef INCLUDED_DPSDK_CORE_PLAYBACK_H
#define INCLUDED_DPSDK_CORE_PLAYBACK_H

/** ����ƽ̨¼��
@param   IN    nPDLLHandle     SDK���
@param   IN    szCameraId      ͨ��ID
@param   IN    streamType      ʵʱ��������
@param   IN    nTimeout        ��ʱʱ������λ����
@return  �������ش������ͣ��ο�dpsdk_retval_e
@remark
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_StartPlatformReocrd(IN int32_t nPDLLHandle,
																  IN const char *szCameraId,
																  IN dpsdk_encdev_stream_e streamType = DPSDK_CORE_STREAM_MAIN,
																  IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ֹͣƽ̨¼��
@param   IN    nPDLLHandle     SDK���
@param   IN    szCameraId      ͨ��ID
@param   IN    nTimeout        ��ʱʱ������λ����
@return  �������ش������ͣ��ο�dpsdk_retval_e
@remark
*/ 
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_StopPlatformReocrd(IN int32_t nPDLLHandle,
																 IN const char *szCameraId,
																 IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );


/** ��ѯ¼��.
 @param   IN	nPDLLHandle		SDK���
 @param   IN	pQueryInfo		��ѯ��Ϣ
 @param   OUT	nRecordCount	¼���¼����
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
   1��nRecordCount���5000����¼
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_QueryRecord( IN int32_t nPDLLHandle, 
														   IN Query_Record_Info_t* pQueryInfo, 
														   OUT int32_t& nRecordCount, 
														   IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ͨ���������Ͳ�ѯ¼��.
 @param   IN	nPDLLHandle				SDK���
 @param   IN	pQueryInfo				��ѯ��Ϣ
 @param   IN	nStreamType				��������
 @param   OUT	nRecordCount			¼���¼����
 @param   IN	nTimeout				��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
   1��nRecordCount���5000����¼
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_QueryRecordByStreamType( IN int32_t nPDLLHandle, 
																		IN Query_Record_Info_t* pQueryInfo, 
																		IN dpsdk_stream_type_e nStreamType,
																		OUT int32_t& nRecordCount, 
																		IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ��ѯ������¼�������.
 @param   IN	nPDLLHandle				SDK���
 @param   IN	cameraId				ͨ��ID
 @param   IN	source					¼����Դ������ѡ��DPSDK_CORE_PB_RECSOURCE_ALL
 @param   IN	recordType				¼�����ͣ�����ѡ��DPSDK_CORE_PB_RECORD_UNKONWN
 @param   IN	Date					���ڣ���λ�룩��������ѡ����Ҫ��ѯ�·����һ��ʱ��㣬��1970��1��1��0ʱ0��0�������
 @param   OUT	days					��¼����죬�Զ��Ÿ���,0������û��¼��1������¼���ϲ���������Ƽ�days = new char[128];
 @param   OUT	LengthofDays			days�ַ����ĳ���
 @param   IN	nTimeout				��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
   1��nRecordCount���5000����¼
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_QueryRecordDaysofTheMonth(int32_t nPDLLHandle, 
																		const char* cameraId, 
																		dpsdk_recsource_type_e source, 
																		dpsdk_record_type_e recordType, 
																		uint64_t Date,
																		char* days,
																		int* LengthofDays,
																		int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT);


/** ��ȡ¼����Ϣ.
 @param   IN	nPDLLHandle		SDK���
 @param   OUT	pRecords		¼����Ϣ
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
   1�������Ȳ�ѯ���ȡ
   2��DPSDK_QueryRecord�᷵�ؼ�¼����,
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_GetRecordInfo( IN int32_t nPDLLHandle, 
															 INOUT Record_Info_t* pRecords );

/** ���ļ�����¼����.
 @param	  IN	nPDLLHandle		SDK���
 @param   OUT	nPlaybackSeq	�ط��������,��Ϊ����������ʶ  
 @param	  IN	pRecordInfo		¼����Ϣ 
 @param   IN    pFun			�����ص�����				
 @param   IN    pUser			�����ص��û�����
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_GetRecordStreamByFile( IN int32_t nPDLLHandle, 
																	 OUT int32_t& nPlaybackSeq, 
																	 IN Get_RecordStream_File_Info_t* pRecordInfo, 
																	 IN fMediaDataCallback pFun, 
																	 IN void* pUser, 
																	 IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ��ʱ������¼����.
 @param	  IN	nPDLLHandle		SDK���
 @param   OUT	nPlaybackSeq	�ط��������,��Ϊ����������ʶ  
 @param	  IN	pRecordInfo		¼����Ϣ 
 @param   IN    pFun			�����ص�����				
 @param   IN    pUser			�����ص��û�����
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_GetRecordStreamByTime( IN int32_t nPDLLHandle, 
																	 OUT int32_t& nPlaybackSeq, 
																	 IN Get_RecordStream_Time_Info_t* pRecordInfo, 
																	 IN fMediaDataCallback pFun, 
																	 IN void* pUser, 
																	 IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** �����������Ͱ�ʱ������¼����.
 @param	  IN	nPDLLHandle		SDK���
 @param   OUT	nPlaybackSeq	�ط��������,��Ϊ����������ʶ  
 @param	  IN	pRecordInfo		¼����Ϣ
 @param   IN	nStreamType		��������
 @param   IN    pFun			�����ص�����				
 @param   IN    pUser			�����ص��û�����
 @param   IN	nTimeout		��ʱʱ������λ����
 @param   IN	nTransMode		����ģʽ��1:TCP 0:UDP
 @param   IN	bBack			�Ƿ񵹷�
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_GetRecordStreamByStreamType( IN int32_t nPDLLHandle, 
																			OUT int32_t& nPlaybackSeq, 
																			IN Get_RecordStream_Time_Info_t* pRecordInfo,
									 										IN dpsdk_stream_type_e nStreamType,
																			IN fMediaDataCallback pFun, 
																			IN void* pUser, 
																			IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT,
																			IN int32_t nTransMode = 1,
																			IN bool bBack = false);

/** ��ͣ¼����.
 @param	  IN	nPDLLHandle		SDK���
 @param   IN	nPlaybackSeq	�ط�������� 
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_PauseRecordStreamBySeq( IN int32_t nPDLLHandle, 
																	  IN int32_t nPlaybackSeq, 
																	  IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** �ָ�¼����.
 @param	  IN	nPDLLHandle		SDK���
 @param   IN	nPlaybackSeq	�ط�������� 
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_ResumeRecordStreamBySeq( IN int32_t nPDLLHandle, 
																	   IN int32_t nPlaybackSeq, 
																	   IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ����¼��������.
 @param	  IN	nPDLLHandle		SDK���
 @param   IN	nPlaybackSeq	�ط�������� 
 @param   IN    nSpeed,         ¼�����ط��ٶ�
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_SetRecordStreamSpeed( IN int32_t nPDLLHandle, 
																	IN int32_t nPlaybackSeq, 
																	IN dpsdk_playback_speed_e nSpeed, 
																	IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ��λ�ط�.
 @param	  IN	nPDLLHandle		SDK���
 @param   IN	nPlaybackSeq	�ط�������� 
 @param   IN    seekBegin,		��λ��ʼֵ.�ļ�ģʽʱ,�Ƕ�λ�����ļ���Сֵ;ʱ��ģʽʱ,�Ƕ�λ����ʱ��ֵ;
 @param   IN    seekEnd,		��λ����ֵ.�ļ�ģʽʱ,������;ʱ��ģʽʱ,���ڴ��Ľ���ʱ��.
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark 
			seekBegin���ļ�ģʽ�µļ��㷽ʽ������:(�ļ���Сֵ)/100*(��λ������ļ��İٷֱ�)  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_SeekRecordStreamBySeq(IN int32_t nPDLLHandle, 
																	IN int32_t nPlaybackSeq, 
																	IN uint64_t seekBegin, 
																	IN uint64_t seekEnd, 
																	IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** �ر�¼����.
 @param	  IN	nPDLLHandle		SDK���
 @param   IN	nPlaybackSeq	�ط�������� 
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_CloseRecordStreamBySeq( IN int32_t nPDLLHandle, 
																	  IN int32_t nPlaybackSeq, 
																	  IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** �ر�¼��ͬ���ȴ�����Ӧ��.
 @param	  IN	nPDLLHandle		SDK���
 @param   IN	nPlaybackSeq	�ط�������� 
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_SynchroCloseRecordStreamBySeq( IN int32_t nPDLLHandle, 
																			  IN int32_t nPlaybackSeq, 
																			  IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ��ͨ���ر�¼����.
 @param	  IN	nPDLLHandle		SDK���
 @param   IN	szCameraId   	ͨ����� 
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_CloseRecordStreamByCameraId( IN int32_t nPDLLHandle, 
																		   IN const char* szCameraId, 
																		   IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ��ͨ���ر�¼������ͬ���ȴ�����Ӧ��.
 @param	  IN	nPDLLHandle		SDK���
 @param   IN	szCameraId   	ͨ����� 
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_SynchroCloseRecordStreamByCameraId( IN int32_t nPDLLHandle, 
																				IN const char* szCameraId, 
																				IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );



/** �ֶ������豸¼��
 @param	  IN	nPDLLHandle		SDK���
 @param	  IN	szDevId			�豸ID
 @param	  IN	szSN			��������
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_StartDeviceRecord( IN int32_t nPDLLHandle, 
																 IN const char* szDevId, 
																 IN const char* szSN,
																 IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** �ֶ�ֹͣ�豸¼��
 @param	  IN	nPDLLHandle		SDK���
 @param	  IN	szDevId			�豸ID
 @param	  IN	szSN			��������
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_StopDeviceRecord( IN int32_t nPDLLHandle, 
															    IN const char* szDevId, 
															    IN const char* szSN,
															    IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ������Ŀ���ƣ�������ʷ��Ƶ�ļ�ID��ѯ¼����Ϣ
 @param   IN	nPDLLHandle		SDK���
 @param   IN	szFileId		��ʷ��Ƶ�ļ�ID
 @param   OUT	szCameraId		����ͷID
 @param   OUT	begTime			��ʼʱ��
 @param   OUT	endTime			����ʱ��
 @param   OUT	szFilePath		��ʷ��Ƶ�ļ�·��
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_QueryRecordByFileId( IN int32_t nPDLLHandle, 
																   IN const char* szFileId,
																   OUT char* szCameraId,
																   OUT uint64_t& begTime,
																   OUT uint64_t& endTime,
																   OUT char* szFilePath,
																   IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT);

/** ¼����.
 @param   IN	nPDLLHandle		SDK���
 @param   INOUT	pTagInfo		¼������Ϣ
 @param	  IN	nOpType			��������1��������2���޸ģ�3��ɾ��
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_OperatorTagInfo( IN int32_t nPDLLHandle, 
															   INOUT Tag_Info_t* pTagInfo,
															   IN dpsdk_operator_type_e nOpType, 
															   IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ���ļ�����¼������ת��Ϊָ���������浽�ļ���ֻ֧�ִ��豸
 @param	  IN	nPDLLHandle		SDK���
 @param   OUT	nPlaybackSeq	�ط��������,��Ϊ����������ʶ  
 @param	  IN	pRecordInfo		¼����Ϣ 
 @param   IN    scType          ����ת����Ŀ������
 @param   IN    pFilePath		�ļ�����·��
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_SaveRecordStreamByFile( IN int32_t nPDLLHandle, 
																	  OUT int32_t& nPlaybackSeq, 
																	  IN Get_RecordStream_File_Info_t* pRecordInfo, 
																	  IN dpsdk_stream_convertor_type_e scType,
																	  IN const char* pFilePath,
																	  IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ��ʱ������¼������ת��Ϊָ���������浽�ļ���ֻ֧�ִ��豸
 @param	  IN	nPDLLHandle		SDK���
 @param   OUT	nPlaybackSeq	�ط��������,��Ϊ����������ʶ  
 @param	  IN	pRecordInfo		¼����Ϣ 
 @param   IN    scType          ����ת����Ŀ������
 @param   IN    pFilePath		�ļ�����·��
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_SaveRecordStreamByTime( IN int32_t nPDLLHandle, 
																	  OUT int32_t& nPlaybackSeq, 
																	  IN Get_RecordStream_Time_Info_t* pRecordInfo,
																	  IN dpsdk_stream_convertor_type_e scType,
																	  IN const char* pFilePath, 
																	  IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ����¼��ת����ɻص�.
 @param   IN	nPDLLHandle		SDK���
 @param   IN	fun				�ص�����
 @param   IN	pUser			�û�����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark		
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD	DPSDK_SetDPSDKSaveRecordFinishedCallback( IN int32_t nPDLLHandle, 
																				  IN fSaveRecordFinishedCallback fun, 
																				  IN void* pUser );

/** ¼������/��������.
 @param   IN	nPDLLHandle		SDK���
 @param   IN	pRecordLockOperationInfo	¼������/������Ϣ
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark		
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_RecordLockOperation( IN int32_t nPDLLHandle, 
																   IN Record_Lock_Operation_Info_t* pRecordLockOperationInfo, 
																   IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ��ѯ¼��������Ϣ.
 @param   IN	nPDLLHandle		SDK���
 @param   IN	pRecordLockQueryInfo	¼��������ѯ��Ϣ
 @param	  OUT	pInfoLen		¼��������Ϣ����			
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark		
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_QueryRecordLock( IN int32_t nPDLLHandle, 
															   IN Record_Lock_Query_Info_t* pRecordLockQueryInfo, 
															   OUT int32_t* pInfoLen, 
															   IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );

/** ��ȡ¼��������Ϣ.
 @param   IN	nPDLLHandle		SDK���
 @param   OUT	pRecordLockInfo	¼��������Ϣ
 @param	  IN	nInfoLen		¼��������Ϣ����			
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
	1�������Ȳ�ѯ���ȡ
	2��DPSDK_QueryRecordLock�᷵����Ϣ���ȣ���Ҫ���ⲿ������ַ����ڴ�
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_GetRecordLockInfo( IN int32_t nPDLLHandle, 
															     OUT char* pRecordLockInfo, 
															     IN int32_t nInfoLen);

/** ��ȡ�ط���Ƶ��URL·��.
 @param	  IN	nPDLLHandle		SDK��� 
 @param	  IN	pRecordStreamUrlInfo	��ѯ¼��Url��Ϣ
 @param   IN	nTimeout		��ʱʱ������λ����
 @return  �������ش������ͣ��ο�dpsdk_retval_e
 @remark  
*/
DPSDK_DLL_API int32_t DPSDK_CALL_METHOD DPSDK_GetPlaybackByTimeUrl( IN int32_t nPDLLHandle, 
																	INOUT Get_RecordStreamUrl_Time_Info_t* pRecordStreamUrlInfo, 
																	IN int32_t nTimeout = DPSDK_CORE_DEFAULT_TIMEOUT );
#endif


