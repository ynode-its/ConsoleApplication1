/*--------------------------------------------------------------------------*
 *	ファイル名	： CmLog.cpp
 *	概要		： ログクラス
 *	作成者		： ITS Y.Node
 *	作成日		： 2015.06.15
 *	修正履歴	：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 * インクルード
 *--------------------------------------------------------------------------*/
#include "StdAfx.h"
#include "CmThread.h"
#include "CmLog.h"
#include "ILogManagerLibrary.h"

/*--------------------------------------------------------------------------*
 * 定数
 *--------------------------------------------------------------------------*/
#define MAX_QUEUE	4
#define MAX_LOG		512
#define MAX_DUMP	128

/*--------------------------------------------------------------------------*
 * プロトタイプ
 *--------------------------------------------------------------------------*/
class CLogData;

/*--------------------------------------------------------------------------*
 * 構造体
 *--------------------------------------------------------------------------*/
typedef struct _LOG_DATA
{
	EN_LOGLV	lv;
	DWORD		thid;
	CHAR		log[ MAX_LOG ];
	union
	{
		SYSTEMTIME	time;
		DWORD		tick;
	} LCTM;
	_LOG_DATA*	next;
} LogData, *LPLOGDATA;

typedef struct _DUMP_SWITCH
{
	LPSTR		pSrouce;
	DWORD		no;
} DumpSwitch, *LPDUMPSWITCH;

/*--------------------------------------------------------------------------*
 * 狭義グローバル変数
 *--------------------------------------------------------------------------*/
static CLogData*	g_pMsgQue[ MAX_QUEUE ] = { 0 };
static DumpSwitch	g_DumpArray[ MAX_DUMP ] = { 0 };
static DWORD		g_DumpArrayCnt = 0;
char szELV[] = "E";
char szWLV[] = "W";
char szILV[] = "I";
char szDLV[] = "D";

ILogManagerLibrary* m_DevLog = GetLogManagerInstance();

/*--------------------------------------------------------------------------*
 *  クラス名    ： CLogData
 *  継承クラス  ： CCmCriticalSection
 *	概要		： ログデータクラス
 *  作成者      ： ITS Y.Node
 *	作成日		： 2015.06.15
 *  修正履歴    ：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
 *--------------------------------------------------------------------------*/
class CLogData : public CCmCriticalSection
{
	int			m_size;
	LogData*	m_head;
	LogData*	m_tail;

public:
	CLogData();
	~CLogData(){}

	int size(){ return m_size; }
	LPLOGDATA front();
	void pop();
	void push( LPLOGDATA pData );
};

/*--------------------------------------------------------------------------*
 * 関数名   ： CLogData
 * 機能     ： コンストラクタ
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
CLogData::CLogData()
{
	m_head = NULL;
	m_tail = NULL;
	m_size = 0;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： push
 * 機能     ： キューにデータを入れる
 * 引数1    ： pData ログデータ
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CLogData::push( LPLOGDATA pData )
{
	Lock();
	pData->next = NULL;
	if ( NULL == m_head )
	{
		m_head = pData;
		m_tail = pData;
	}
	else
	{
		m_tail->next = pData;
		m_tail = pData;
	}
	m_size++;
	Unlock();
}

/*--------------------------------------------------------------------------*
 * 関数名   ： front
 * 機能     ： キューの先頭データを取得
 * 戻り値   ： LPLOGDATA
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
LPLOGDATA CLogData::front()
{
	return m_head;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： pop
 * 機能     ： キューの先頭データを削除
 * 戻り値   ： LPLOGDATA
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CLogData::pop()
{
	Lock();
	if ( NULL != m_head )
	{
		_LOG_DATA* p = m_head->next;
		m_head = p;
		m_size--;
	}
	else
	{
		m_size = 0;
	}
	Unlock();
}

/*--------------------------------------------------------------------------*
 * 関数名   ： SortCompare
 * 機能     ： ソート比較
 * 戻り値   ： 同じ(0)、大きい(1)、小さい(-1)
 * 引数1    ： WPARAM wParam
 * 引数2    ： LPARAM lParam
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
int SortCompare( const LPDUMPSWITCH pSrc1, const LPDUMPSWITCH pSrc2 )
{
	int cmp = _stricmp( pSrc1->pSrouce, pSrc2->pSrouce );
	if ( 0 == cmp )
	{
		if ( pSrc1->no < pSrc2->no )
		{
			return -1;
		}
		else if ( pSrc1->no > pSrc2->no )
		{
			return 1;
		}
	}
 	
	return cmp;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： CCmLog
 * 機能     ： コンストラクタ
 * 引数1    ： LPCTSTR lpcCategory カテゴリ
 * 引数2    ： EN_LOGMODE mode ログモード
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
CCmLog::CCmLog( LPCTSTR lpcCategory, EN_LOGMODE mode, BOOL suspend ) : m_hFile( INVALID_HANDLE_VALUE ), m_hDevFile( INVALID_HANDLE_VALUE )
{
	m_Idx = -1;
	m_mode = mode;
	m_lv = E_LOG_NONE;
	m_blSerial = FALSE;

	assert( _tcslen( lpcCategory ) );

	Lock();
	for ( int i = 0; i < MAX_QUEUE; i++ )
	{
		ISNULL( g_pMsgQue[ i ] )
		{
			g_pMsgQue[ i ] = new CLogData;
			m_Idx = i;
			break;
		}
	}
	Unlock();

	if ( FALSE == suspend )
	{
		logStart( lpcCategory );
	}
}

/*--------------------------------------------------------------------------*
 * 関数名   ： ~CCmLog
 * 機能     ： デストラクタ
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
CCmLog::~CCmLog()
{
	KillThread();

	if ( INVALID_HANDLE_VALUE != m_hFile )
	{
		CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}
	if ( INVALID_HANDLE_VALUE != m_hDevFile )
	{
		CloseHandle( m_hDevFile );
		m_hDevFile = INVALID_HANDLE_VALUE;
	}

	Lock();
	for ( DWORD i = 0; i < g_DumpArrayCnt; i++ )
	{
		if ( NULL != g_DumpArray[ i ].pSrouce )
		{
			delete[] g_DumpArray[ i ].pSrouce;
			g_DumpArray[ i ].pSrouce = NULL;
		}
	}
	g_DumpArrayCnt = 0;
	Unlock();
	
	DELETE_ALLOC( g_pMsgQue[ m_Idx ] );
}

/*--------------------------------------------------------------------------*
 * 関数名   ： Start
 * 機能     ： ログスタート
 * 引数1    ： LPCTSTR lpcCategory カテゴリ
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmLog::Start( LPCTSTR lpcCategory )
{
	assert( _tcslen( lpcCategory ) );

	if ( INVALID_HANDLE_VALUE != m_hFile ) return;

	logStart( lpcCategory );
}

/*--------------------------------------------------------------------------*
 * 関数名   ： Stop
 * 機能     ： ログストップ
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmLog::Stop()
{
	KillThread();
	
	if ( INVALID_HANDLE_VALUE != m_hFile )
	{
		CloseHandle( m_hFile );
		m_hFile = INVALID_HANDLE_VALUE;
	}
	if ( INVALID_HANDLE_VALUE != m_hDevFile )
	{
		CloseHandle( m_hDevFile );
		m_hDevFile = INVALID_HANDLE_VALUE;
	}
}

/*--------------------------------------------------------------------------*
 * 関数名   ： logStart
 * 機能     ： ログスタート
 * 引数1    ： LPCTSTR lpcCategory カテゴリ
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmLog::logStart( LPCTSTR lpcCategory )
{
	TCHAR  szLogFileName[ MAX_PATH ];
	TCHAR  szCategory[ MAX_PATH ];
	TCHAR  szId[ 16 ];

	_tcscpy( szCategory, lpcCategory );
	_tcscat( szCategory, _itow( GetCurrentThreadId(), szId, 16 ) );

	LPCTSTR flg = CreateLogFileName( lpcCategory, szCategory, szLogFileName );
	if ( 0 >= _tcslen( flg ) )
	{
		return;
	}

	m_hFile = CreateFile( szLogFileName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( INVALID_HANDLE_VALUE == m_hFile )
	{
		return;
	}
	SetFilePointer( m_hFile, NULL, NULL, FILE_END );

//#if defined( UNDER_CE ) && ( _WIN32_WCE == 0x700 )
//	m_hDevFile = rotation( INVALID_HANDLE_VALUE );
//#endif

	CCmThread::Create();
}

/*--------------------------------------------------------------------------*
 * 関数名   ： rotation
 * 機能     ： ローテーション
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2013.02.28
 *--------------------------------------------------------------------------*/
//HANDLE CCmLog::rotation( HANDLE hFile )
//{
//	Lock();
//	const TCHAR LogDisk[] = L"\\DevLogDisk\\MemCtl.log";
//
//	while( 1 )
//	{
//		if ( INVALID_HANDLE_VALUE == hFile )
//		{
//			hFile = CreateFile( LogDisk, GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
//			if ( INVALID_HANDLE_VALUE == hFile )
//			{
//				break;
//			}
//			SetFilePointer( hFile, NULL, NULL, FILE_END );
//		}
//
//		DWORD dwSize = GetFileSize( hFile, NULL );
//		if ( -1 == dwSize )
//		{
//			break;
//		}
//
//		if ( 0xA00000 < dwSize ) // 2M
//		{
//			CloseHandle( hFile );
//			hFile = INVALID_HANDLE_VALUE;
//			LPTSTR pNameA = new TCHAR[ 256 ];
//			LPTSTR pNameB = new TCHAR[ 256 ];
//			LPTSTR pNameX;
//
//			BOOL bl;
//			DWORD dwCnt = 3;
//			wsprintf( pNameA, L"%s.%d", LogDisk, dwCnt );
//			if ( -1 != GetFileAttributes( pNameA ) )
//			{
//				bl = DeleteFile( pNameA );
//				//RETAILMSG( 1, ( L"CCmLog::rotation DeleteFile(%d %d:%s)\r\n", bl, dwCnt, pNameA ) );
//			}
//			//else
//			//{
//			//	RETAILMSG( 1, ( L"CCmLog::rotation DeleteFile File Not Found(%s)\r\n", pNameA ) );
//			//}
//			while ( 1 < dwCnt )
//			{
//				dwCnt--;
//				wsprintf( pNameB, L"%s.%d", LogDisk, dwCnt );
//				if ( -1 != GetFileAttributes( pNameB ) )
//				{
//					bl = MoveFile( pNameB, pNameA );
//					//RETAILMSG( 1, ( L"CCmLog::rotation MoveFile(%d %d:%s %d:%s)\r\n", bl, dwCnt, pNameB, dwCnt + 1, pNameA ) );
//				}
//				//else
//				//{
//				//	RETAILMSG( 1, ( L"CCmLog::rotation MoveFile File Not Found(%s)\r\n", pNameB ) );
//				//}
//				pNameX = pNameA;
//				pNameA = pNameB;
//				pNameB = pNameX;
//			}
//			if ( -1 != GetFileAttributes( LogDisk ) )
//			{
//				bl = MoveFile( LogDisk, pNameA );
//				//RETAILMSG( 1, ( L"CCmLog::rotation MoveFile(%d L:%s 1:%s)\r\n", bl, LogDisk, pNameA ) );
//			}
//			//else
//			//{
//			//	RETAILMSG( 1, ( L"CCmLog::rotation MoveFile File Not Found(%s)\r\n", pNameA ) );
//			//}
//			delete[] pNameA;
//			delete[] pNameB;
//			//RETAILMSG( 1, ( L"CCmLog::rotation End\r\n" ) );
//			continue;
//		}
//		break;
//	}
//	Unlock();
//
//	return hFile;
//}

/*--------------------------------------------------------------------------*
 * 関数名   ： CreateLogFileName
 * 機能     ： ログファイル名作成
 * 引数1    ： LPCTSTR lpcCategory カテゴリ
 * 引数2    ： LPCTSTR lpcCategoryId カテゴリ+スレッドID
 * 引数3    ： LPTSTR lpStr ログファイルフルパスバッファ
 * 戻り値   ： ログファイルフルパス
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
LPCTSTR CCmLog::CreateLogFileName( LPCTSTR lpcCategory, LPCTSTR lpcCategoryId, LPTSTR lpStr )
{
	TCHAR szModulePath[ MAX_PATH ];
	TCHAR szModuleName[ _MAX_PATH ];
	TCHAR szLogPath[ _MAX_PATH ];

	ZeroMemory( szLogPath, sizeof( szLogPath ) );

	GetModuleFileName( GetModuleHandle( NULL ), szModulePath, sizeof( szModulePath ) / sizeof( TCHAR ) );
	
	LPCTSTR lpfn = _tcsrchr( szModulePath, _T( '\\' ) );
	if ( NULL != lpfn ) // バックスラッシュを含んでれば、それ以降をを使用する。
	{
		_tcscpy( szModuleName, ( lpfn + 1 ) );
	}
	else
	{
		_tcscpy( szModuleName, szModulePath );
	}

	lpfn = _tcsrchr( szModuleName, _T( '.' ) );
	NOTNULL( lpfn )
	{
		memset( ( void* )( lpfn ), _T( '\0' ), sizeof( TCHAR ) );
	}
	
	lpfn = _tcsrchr( szModulePath, _T( '\\' ) );
	NOTNULL( lpfn )
	{
		memset( ( void* )( lpfn ), _T( '\0' ), sizeof( TCHAR ) );

		Lock();
		BOOL bread = ReadIni( szModulePath, lpcCategory, szLogPath );
		Unlock();


		lpStr[ 0 ] = _T( '\0' );

		if ( bread )
		{
			if ( 0 >= wcslen( szLogPath ) )
			{
				_tcscpy( szLogPath, szModulePath );
				_tcscat( szLogPath, _T( "\\MemLog" ) );
			}
			CreateDirectory( szLogPath, NULL );

			SYSTEMTIME tm;
			GetLocalTime( &tm );
			wsprintf( lpStr, TEXT( "%s\\%s_%s_%04d%02d%02d%02d%02d%02d.log" ), szLogPath, szModuleName, lpcCategoryId, tm.wYear, tm.wMonth, tm.wDay, tm.wHour, tm.wMinute, tm.wSecond );
			//wsprintf( lpStr, TEXT( "%s\\%s_%s_%04d%02d%02d.log" ), szModulePath, szModuleName, lpcCategoryId, tm.wYear, tm.wMonth, tm.wDay );
		}
		
	}
	
	return lpStr;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： ReadIni
 * 機能     ： 設定ファイル読込
 * 引数1    ： path 設定ファイルパス
 * 引数2    ： lpcCategory カテゴリ
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2011.06.16
 *--------------------------------------------------------------------------*/
BOOL CCmLog::ReadIni( LPCTSTR path, LPCTSTR lpcCategory, LPTSTR lpLogPath )
{
	TCHAR	szModulePath[ MAX_PATH ];
	TCHAR	wszCategory[ 16 ];
	CHAR	szCategory[ 16 ];
	CHAR	szLogLv[ 16 ];
	CHAR	szLogSerial[ 16 ];
	CHAR	szLogPath[ 16 ];
	BOOL	bResult = FALSE;

	_tcscpy( wszCategory, lpcCategory );
	CmUtf16ToSjis( szCategory, wszCategory, sizeof( szCategory ) );
	strcpy( szLogLv, szCategory );
	strcat( szLogLv, "Lv=" );
	strcpy( szLogSerial, szCategory );
	strcat( szLogSerial, "Serial=" );
	strcpy( szLogPath, szCategory );
	strcat( szLogPath, "Path=" );

	_tcscpy( szModulePath, path );
	_tcscat( szModulePath, _T( "\\_MemLog.ini" ) );

	HANDLE hFile = CreateFile( szModulePath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		return bResult;
	}
	
	DWORD size = GetFileSize( hFile, NULL );
	if ( 0 < size )
	{
		LPBYTE pBuff = new BYTE[ size + 1 ];
		ZeroMemory( pBuff, size + 1 );
		DWORD s1 = size;
		DWORD s2;

		BOOL b = ReadFile( hFile, pBuff, s1, &s2, NULL );

		char* p = strtok( ( char* )pBuff, "\r\n" );
		while ( p )
		{
			int i = strncmp( p, szLogLv, strlen( szLogLv ) );
			if ( i == 0 )
			{
				char* pp = strtok( p, "=" );
				pp = strtok( NULL, "=" );
				if ( NULL != pp )
				{
					m_lv = ( EN_LOGLV )atoi( pp );
					bResult = TRUE;
					break;
				}
			}
			p = strtok( NULL, "\r\n" );
		}
		if ( bResult )
		{
			SetFilePointer( hFile, 0, NULL, FILE_BEGIN );
			b = ReadFile( hFile, pBuff, s1, &s2, NULL );

			p = strtok( ( char* )pBuff, "\r\n" );
			while ( p )
			{
				int i = strncmp( p, szLogSerial, strlen( szLogSerial ) );
				if ( i == 0 )
				{
					char* pp = strtok( p, "=" );
					pp = strtok( NULL, "=" );
					if( NULL != pp )
					{
						m_blSerial = atoi( pp );
						break;
					}
				}
				p = strtok( NULL, "\r\n" );
			}
		}
		if ( bResult )
		{
			SetFilePointer( hFile, 0, NULL, FILE_BEGIN );
			b = ReadFile( hFile, pBuff, s1, &s2, NULL );

			p = strtok( ( char* )pBuff, "\r\n" );
			while ( p )
			{
				int i = strncmp( p, szLogPath, strlen( szLogPath ) );
				if ( i == 0 )
				{
					char* pp = strtok( p, "=" );
					pp = strtok( NULL, "=" );

					DWORD len = MultiByteToWideChar( CP_ACP, 0, pp, -1, NULL, 0 );
					LPTSTR wstrDest = new TCHAR[ len ];
					MultiByteToWideChar( CP_ACP, 0, pp, -1, wstrDest, len );
					wcscpy( lpLogPath, wstrDest );
					delete[] wstrDest;
					break;
				}
				p = strtok( NULL, "\r\n" );
			}
		}
		if ( 0 == g_DumpArrayCnt )
		{
			SetFilePointer( hFile, 0, NULL, FILE_BEGIN );
			b = ReadFile( hFile, pBuff, s1, &s2, NULL );

			p = strtok( ( char* )pBuff, "\r\n" );
			while ( p )
			{
				char* pStrLine = new char[ strlen( p ) + 1 ];
				strcpy( pStrLine, p );
				int i = strncmp( pStrLine, "dump", 4 );
				if ( i == 0 )
				{
					char* pp = strstr( pStrLine, "=" );
					if ( pp )
					{
						*pp = '\0';
						pp++;
						char* pSrc = pp;
						pp = strstr( pp, "," );
						if ( pp )
						{
							*pp = '\0';
							pp++;
							char* pNo = pp;
							
							g_DumpArray[ g_DumpArrayCnt ].pSrouce = new char[ strlen( pSrc ) + 1 ];
							strcpy( g_DumpArray[ g_DumpArrayCnt ].pSrouce, pSrc );
							g_DumpArray[ g_DumpArrayCnt ].no = atoi( pNo );
							g_DumpArrayCnt++;
							if ( MAX_DUMP <= g_DumpArrayCnt )
							{
								delete[] pStrLine;
								break;
							}
						}
					}
				}
				delete[] pStrLine;
				p = strtok( NULL, "\r\n" );
			}

			qsort( g_DumpArray, g_DumpArrayCnt, sizeof( DumpSwitch ), ( Compare )SortCompare );
		}

		delete[] pBuff;
	}

	CloseHandle( hFile );

	return bResult;
}

const char* CCmLog::GetLvChar( EN_LOGLV eLv )
{
	static const char szLVDefault[] = " ";
	const char* pLV = szLVDefault;
	switch( eLv )
	{
	case E_LOG_E1LV:
	case E_LOG_E2LV:
		pLV = szELV;
		break;
	case E_LOG_W1LV:
	case E_LOG_W2LV:
		pLV = szWLV;
		break;
	case E_LOG_I1LV:
	case E_LOG_I2LV:
		pLV = szILV;
		break;
	case E_LOG_D1LV:
	case E_LOG_D2LV:
	case E_LOG_D3LV:
	case E_LOG_NONE:
		pLV = szDLV;
		break;
	}

	return pLV;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： NextLog
 * 機能     ： ログファイル名作成
 * 引数1    ： LPSTR lpBuff 書込みバッファ
 * 引数2    ： DWORD& nNumberOfBytesToWrite 書込みバッファサイズ
 * 戻り値   ： ログキュー取得
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
BOOL CCmLog::NextLog( LPSTR lpBuff, DWORD& nNumberOfBytesToWrite )
{
	LPLOGDATA pData = NULL;

	if ( NULL == g_pMsgQue[ m_Idx ] )
	{
		return FALSE;
	}

	if ( 0 < g_pMsgQue[ m_Idx ]->size() )
	{
		pData = g_pMsgQue[ m_Idx ]->front();
		NOTNULL( pData ) g_pMsgQue[ m_Idx ]->pop();
	}

	ISNULL( pData ) return FALSE;

	const char* pLV = GetLvChar( pData->lv );

#ifdef _WINPC
	nNumberOfBytesToWrite = sprintf( lpBuff,
			//"[%04d/%02d/%02d %02d:%02d:%02d.%03d][%s,%08X] %s\n",
			"[%08X %02d:%02d:%02d.%03d %s] %s\n",
			//pData->LCTM.time.wYear, pData->LCTM.time.wMonth, pData->LCTM.time.wDay,
			pData->thid, pData->LCTM.time.wHour, pData->LCTM.time.wMinute, pData->LCTM.time.wSecond, pData->LCTM.time.wMilliseconds, pLV, pData->log );
#else
	nNumberOfBytesToWrite = sprintf( lpBuff,
			"[%010d %s] %s\n",
			pData->LCTM.tick, pLV, pData->log );
#endif

	delete pData;
	pData = NULL;

	return TRUE;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： Dump
 * 機能     ： ダンプ出力(フィルタリング機能付)
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmLog::Dump( const char* pFile, DWORD no, LPCSTR fmt, ... )
{
	DumpSwitch dmp;
	dmp.pSrouce = ( char* )pFile;
	dmp.no = no;

	Lock();
	if ( 0 == g_DumpArrayCnt )
	{
		Unlock();
		return;
	}
	LPDUMPSWITCH p = ( LPDUMPSWITCH )CmBsearch( &dmp, g_DumpArray, g_DumpArrayCnt, sizeof( DumpSwitch ), ( Compare )SortCompare );
	Unlock();
	if ( p )
	{
		va_list argList;
		va_start( argList, fmt ); /* pgr0834 CRTの中なのでなので、問題なし */
		Add( E_LOG_NONE, fmt, argList );
		va_end( argList );
	}
}

/*--------------------------------------------------------------------------*
 * 関数名   ： Dump
 * 機能     ： ダンプ出力(フィルタリング機能付)
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmLog::Dump( const char* pFile, DWORD no, const LPBYTE pData, DWORD dwSize )
{
	DumpSwitch dmp;
	dmp.pSrouce = ( char* )pFile;
	dmp.no = no;

	Lock();
	if ( 0 == g_DumpArrayCnt )
	{
		Unlock();
		return;
	}
	LPDUMPSWITCH p = ( LPDUMPSWITCH )CmBsearch( &dmp, g_DumpArray, g_DumpArrayCnt, sizeof( DumpSwitch ), ( Compare )SortCompare );
	Unlock();
	if ( p )
	{
		DWORD cnt = 0;
		while ( cnt < dwSize )
		{
			LPLOGDATA pLogData = new LogData;

			cnt = PrintHex( pLogData->log, pData, dwSize, cnt );

			pLogData->lv = E_LOG_NONE;
			pLogData->thid = GetCurrentThreadId();
#ifdef _WINPC
			GetLocalTime( &pLogData->LCTM.time );
#else
			pLogData->LCTM.tick = GetTickCount();
#endif
			g_pMsgQue[ m_Idx ]->push( pLogData );
		}
	}
}	/* pgr0840 pLogDataは、push先で管理する(最終的にCCmLog::NextLog()関数でdeleteする) */

/*--------------------------------------------------------------------------*
 * 関数名   ： Byte2Hex
 * 機能     ： バイトを文字に変換
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmLog::Byte2Hex( BYTE data, LPSTR hi, LPSTR lo )
{
	*hi = ( ( data >> 4 ) & 0x0F ) + 0x30;
	if ( 0x3A <= *hi ) *hi += 7;
	*lo = ( data & 0x0F ) + 0x30;
	if ( 0x3A <= *lo ) *lo += 7;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： PrintHex
 * 機能     ： バイナリを文字列に変換
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
DWORD CCmLog::PrintHex( LPSTR line, const LPBYTE pData, DWORD dwSize, DWORD cnt )
{
	LPBYTE p = pData;

	line[ 0 ] = '0';
	line[ 1 ] = 'X';
	Byte2Hex( ( BYTE )( ( cnt & 0xFF00 ) >> 8 ), &line[ 2 ], &line[ 3 ] );
	Byte2Hex( ( BYTE )( cnt & 0x00FF ), &line[ 4 ], &line[ 5 ] );
	line[ 6 ] = '[';
	for ( int i = 0; i < 16; i++)
	{
		Byte2Hex( *p, &line[ i * 3 + 7 ], &line[ i * 3 + 8 ] );
		line[ i * 3 + 9 ] = 0x20;
		p++;
		cnt++;
		if ( cnt >= dwSize )
		{
			line[ i * 3 + 9 ] = ']';
			line[ i * 3 + 10 ] = '\0';
			break;
		}
	}
	line[ 54 ] = ']';
	line[ 55 ] = '\0';

	return cnt;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： Put
 * 機能     ： ログ出力
 * 引数1    ： EN_LOGLV lv ログレベル
 * 引数2    ： LPCSTR fmt 可変引数フォーマット
 * 引数3    ： 可変引数
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmLog::Put( EN_LOGLV lv, LPCSTR fmt, ... )
{
	if ( E_LOGMODE != m_mode ) return;
	if ( E_LOG_I1LV < lv && m_lv < lv ) return;

	va_list argList;
	va_start( argList, fmt ); /* pgr0834 CRTの中なのでなので、問題なし */
	Add( lv, fmt, argList );
	va_end( argList );
}

/*--------------------------------------------------------------------------*
 * 関数名   ： Add
 * 機能     ： ログ出力
 * 引数1    ： EN_LOGLV lv ログレベル
 * 引数2    ： LPCSTR fmt 可変引数フォーマット
 * 引数3    ： argList 可変引数
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmLog::Add( EN_LOGLV lv, LPCSTR fmt, va_list argList )
{
	LPLOGDATA pData = new LogData;

	int len = _vsnprintf( pData->log, sizeof( pData->log ), fmt, argList );
	pData->log[ sizeof( pData->log ) - 1 ] = '\0';

	pData->lv = lv;
	pData->thid = GetCurrentThreadId();
#ifdef _WINPC
	GetLocalTime( &pData->LCTM.time );
#else
	pData->LCTM.tick = GetTickCount();
#endif

	if ( m_blSerial || E_LOG_I1LV >= lv )
	{
		const char* pLV = GetLvChar( pData->lv );
		LPSTR lpBuff = new CHAR[ 1024 ];
		if ( NULL != lpBuff )
		{
			if ( E_LOG_E1LV == lv )
			{
				sprintf( lpBuff, "[%010d %s] ##### %s #####\n", pData->LCTM.tick, pLV, pData->log );
			}
			else
			{
				sprintf( lpBuff, "[%010d %s] %s\n", pData->LCTM.tick, pLV, pData->log );
			}
			int cnt = MultiByteToWideChar( CP_ACP, 0, lpBuff, -1, NULL, 0 );
			if ( 0 < cnt )
			{
				LPTSTR wstrDest = new TCHAR[ cnt + 1 ];
				if ( NULL != wstrDest )
				{
					MultiByteToWideChar( CP_ACP, 0, lpBuff, -1, wstrDest, cnt );
					wstrDest[ cnt ] = L'\0';
					if ( NULL != m_DevLog )
					{
						m_DevLog->DebugMsg( wstrDest );
						//OutputDebugString( wstrDest );
					}
//#if defined( UNDER_CE ) && ( _WIN32_WCE == 0x700 )
//					DWORD nNumberOfBytesWritten;
//					m_hDevFile = rotation( m_hDevFile );
//					WriteFile( m_hDevFile, wstrDest, sizeof( TCHAR ) * wcslen( wstrDest ), &nNumberOfBytesWritten, NULL );
//#endif
					delete[] wstrDest;
				}
			}
			delete[] lpBuff;
		}
	}

	if ( E_LOG_NONE == lv )
	{
		delete pData;
		return;
	}

	if ( m_lv >= lv )
	{
		g_pMsgQue[ m_Idx ]->push( pData );
	}
	else
	{
		delete pData;
	}
}	/* pgr0840 pDataは、push先で管理する(最終的にCCmLog::NextLog()関数でdeleteする) */

/*--------------------------------------------------------------------------*
 * 関数名   ： ErrorFormatMessage
 * 機能     ： エラーフォーマットログ出力
 * 引数1    ： EN_LOGLV lv ログレベル
 * 引数2    ： LPCSTR func 関数名
 * 引数2    ： DWORD lasterrno ラストエラー番号
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
//void CCmLog::ErrorFormatMessage( EN_LOGLV lv, LPCSTR func, DWORD lasterrno )
//{
//	if ( E_LOGMODE != m_mode ) return;
//	if ( m_lv < lv ) return;
//
//	LPVOID lpMessageBuffer;
//  
//	FormatMessage(
//		FORMAT_MESSAGE_ALLOCATE_BUFFER |
//		FORMAT_MESSAGE_FROM_SYSTEM,
//		NULL,
//		lasterrno,
//		MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
//		( LPTSTR ) &lpMessageBuffer,
//		0,
//		NULL );
//
//	LPLOGDATA pData = new LogData;
//
//	char* buff = new char[ 1024 ];
//	CmUtf16ToSjis( buff, ( LPTSTR )lpMessageBuffer, 1024 );
//
//	sprintf( pData->log, "[%s(0x%08X)]%s", func, lasterrno, buff );
//	delete[] buff;
//
//	size_t len = strlen( pData->log );
//	if ( 3 < len ) pData->log[ len - 2 ] = '\0';
//	LocalFree( lpMessageBuffer );
//
//	pData->lv = lv;
//	pData->thid = GetCurrentThreadId();
//#ifdef _WINPC
//	GetLocalTime( &pData->LCTM.time );
//#else
//	pData->LCTM.tick = GetTickCount();
//#endif
//	g_pMsgQue[ m_Idx ]->push( pData );
//}

/*--------------------------------------------------------------------------*
 * 関数名   ： Exec
 * 機能     ： ログ書込みスレッド
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
long CCmLog::Exec()
{
	DWORD nNumberOfBytesToWrite;
	DWORD nNumberOfBytesWritten;
	char szBuff[ MAX_LOG + 256 ];
	int cnt;

	while ( WAIT_TIMEOUT == WaitForSingleObject( m_EventKill, 1 ) )
	{
		cnt = 0;
		while ( 0 < NextLog( szBuff, nNumberOfBytesToWrite ) )
		{
			WriteFile( m_hFile, szBuff, nNumberOfBytesToWrite, &nNumberOfBytesWritten, NULL );
			if ( cnt > 10 ) break;
			cnt++;
		}
	}

	while ( 0 < NextLog( szBuff, nNumberOfBytesToWrite ) )
	{
		WriteFile( m_hFile, szBuff, nNumberOfBytesToWrite, &nNumberOfBytesWritten, NULL );
	}
	
	if ( E_LOGMODE == m_mode )
	{
		WriteFile( m_hFile, "[END]\r\n", ( DWORD )strlen( "[END]\r\n" ), &nNumberOfBytesWritten, NULL );
	}

	::FlushFileBuffers( m_hFile );

	return CM_S_OK;
}

/*--------------------------------------------------------------------------*
 *						ファイル(CmLog.cpp)の終わり
 *--------------------------------------------------------------------------*/
