/*--------------------------------------------------------------------------*
 *	ファイル名	： CmLog.h
 *	概要		： 共通ログ
 *	作成者		： ITS Y.Node
 *	作成日		： 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *	修正履歴	：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
 *--------------------------------------------------------------------------*/

#ifndef _CMLOG_H_
#define _CMLOG_H_

enum EN_LOGLV
{
	E_LOG_NONE = 0,	// 0
	E_LOG_E1LV,		// 1 システム異常系
	E_LOG_W1LV,		// 2 システム警告系
	E_LOG_I1LV,		// 3 システムトレース情報系
	E_LOG_E2LV,		// 4 データ異常系
	E_LOG_W2LV,		// 5 データ警告系
	E_LOG_I2LV,		// 6 トレース情報系
	E_LOG_D1LV,		// 7 送受信メッセージ
	E_LOG_D2LV,		// 8 デバッグトレース
	E_LOG_D3LV,		// 9 メモリダンプ
};

enum EN_LOGMODE
{
	E_LOGMODE = 0,
	E_TRACEMODE,
};

/*--------------------------------------------------------------------------*
 *  クラス名    ： CCmLog
 *  継承クラス  ： CCmThread
 *	概要		： 共通ログクラス
 *  作成者      ： ITS Y.Node
 *	作成日		： 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *  修正履歴    ：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
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
 *						ファイル(CmLog.h)の終わり
 *--------------------------------------------------------------------------*/
