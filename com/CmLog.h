/*--------------------------------------------------------------------------*
 *	�t�@�C����	�F CmLog.h
 *	�T�v		�F ���ʃ��O
 *	�쐬��		�F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *	�C������	�F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/

#ifndef _CMLOG_H_
#define _CMLOG_H_

enum EN_LOGLV
{
	E_LOG_NONE = 0,	// 0
	E_LOG_E1LV,		// 1 �V�X�e���ُ�n
	E_LOG_W1LV,		// 2 �V�X�e���x���n
	E_LOG_I1LV,		// 3 �V�X�e���g���[�X���n
	E_LOG_E2LV,		// 4 �f�[�^�ُ�n
	E_LOG_W2LV,		// 5 �f�[�^�x���n
	E_LOG_I2LV,		// 6 �g���[�X���n
	E_LOG_D1LV,		// 7 ����M���b�Z�[�W
	E_LOG_D2LV,		// 8 �f�o�b�O�g���[�X
	E_LOG_D3LV,		// 9 �������_���v
};

enum EN_LOGMODE
{
	E_LOGMODE = 0,
	E_TRACEMODE,
};

/*--------------------------------------------------------------------------*
 *  �N���X��    �F CCmLog
 *  �p���N���X  �F CCmThread
 *	�T�v		�F ���ʃ��O�N���X
 *  �쐬��      �F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *  �C������    �F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/
class CCmLog : public CCmThread
{
	HANDLE		m_hFile;
	HANDLE		m_hDevFile;
	int			m_Idx;
	EN_LOGMODE	m_mode;
	EN_LOGLV	m_lv;
	BOOL		m_blSerial;

	virtual long Exec();

	LPCTSTR CreateLogFileName( LPCTSTR lpcCategory, LPCTSTR lpcCategoryId, LPTSTR lpStr );
	BOOL NextLog( LPSTR lpBuff, DWORD& nNumberOfBytesToWrite );
	void logStart( LPCTSTR lpcCategory );
	BOOL ReadIni( LPCTSTR path, LPCTSTR lpcCategory, LPTSTR lpLogPath );

	void Add( EN_LOGLV lv, LPCSTR fmt, va_list argList );
	void Byte2Hex( BYTE data, LPSTR hi, LPSTR lo );
	DWORD PrintHex( LPSTR line, const LPBYTE pData, DWORD dwSize, DWORD cnt );
	const char* GetLvChar( EN_LOGLV eLv );
	HANDLE rotation( HANDLE hFile );
public:
	CCmLog( LPCTSTR lpcCategory, EN_LOGMODE mode = E_LOGMODE, BOOL suspend = FALSE );
	virtual ~CCmLog();

	void Start( LPCTSTR lpcCategory );
	void Stop();

	void Put( EN_LOGLV lv, LPCSTR fmt, ... );
	void Dump( const char* pFile, DWORD no, LPCSTR fmt, ... );
	void Dump( const char* pFile, DWORD no, const LPBYTE pData, DWORD dwSize );
private:
	//void ErrorFormatMessage( EN_LOGLV lv, LPCSTR func, DWORD lasterrno );
};

#endif	// End of _CMLOG_H_

/*--------------------------------------------------------------------------*
 *						�t�@�C��(CmLog.h)�̏I���
 *--------------------------------------------------------------------------*/
