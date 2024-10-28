/*--------------------------------------------------------------------------*
 *	�t�@�C����	�F CmLog.cpp
 *	�T�v		�F ���O�N���X
 *	�쐬��		�F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	�C������	�F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 * �C���N���[�h
 *--------------------------------------------------------------------------*/
#include "StdAfx.h"
#include "CmThread.h"
#include "CmLog.h"
#include "ILogManagerLibrary.h"

/*--------------------------------------------------------------------------*
 * �萔
 *--------------------------------------------------------------------------*/
#define MAX_QUEUE	4
#define MAX_LOG		512
#define MAX_DUMP	128

/*--------------------------------------------------------------------------*
 * �v���g�^�C�v
 *--------------------------------------------------------------------------*/
class CLogData;

/*--------------------------------------------------------------------------*
 * �\����
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
 * ���`�O���[�o���ϐ�
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
 *  �N���X��    �F CLogData
 *  �p���N���X  �F CCmCriticalSection
 *	�T�v		�F ���O�f�[�^�N���X
 *  �쐬��      �F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *  �C������    �F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
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
 * �֐���   �F CLogData
 * �@�\     �F �R���X�g���N�^
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
CLogData::CLogData()
{
	m_head = NULL;
	m_tail = NULL;
	m_size = 0;
}

/*--------------------------------------------------------------------------*
 * �֐���   �F push
 * �@�\     �F �L���[�Ƀf�[�^������
 * ����1    �F pData ���O�f�[�^
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F front
 * �@�\     �F �L���[�̐擪�f�[�^���擾
 * �߂�l   �F LPLOGDATA
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
LPLOGDATA CLogData::front()
{
	return m_head;
}

/*--------------------------------------------------------------------------*
 * �֐���   �F pop
 * �@�\     �F �L���[�̐擪�f�[�^���폜
 * �߂�l   �F LPLOGDATA
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F SortCompare
 * �@�\     �F �\�[�g��r
 * �߂�l   �F ����(0)�A�傫��(1)�A������(-1)
 * ����1    �F WPARAM wParam
 * ����2    �F LPARAM lParam
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F CCmLog
 * �@�\     �F �R���X�g���N�^
 * ����1    �F LPCTSTR lpcCategory �J�e�S��
 * ����2    �F EN_LOGMODE mode ���O���[�h
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F ~CCmLog
 * �@�\     �F �f�X�g���N�^
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F Start
 * �@�\     �F ���O�X�^�[�g
 * ����1    �F LPCTSTR lpcCategory �J�e�S��
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmLog::Start( LPCTSTR lpcCategory )
{
	assert( _tcslen( lpcCategory ) );

	if ( INVALID_HANDLE_VALUE != m_hFile ) return;

	logStart( lpcCategory );
}

/*--------------------------------------------------------------------------*
 * �֐���   �F Stop
 * �@�\     �F ���O�X�g�b�v
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F logStart
 * �@�\     �F ���O�X�^�[�g
 * ����1    �F LPCTSTR lpcCategory �J�e�S��
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F rotation
 * �@�\     �F ���[�e�[�V����
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2013.02.28
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
 * �֐���   �F CreateLogFileName
 * �@�\     �F ���O�t�@�C�����쐬
 * ����1    �F LPCTSTR lpcCategory �J�e�S��
 * ����2    �F LPCTSTR lpcCategoryId �J�e�S��+�X���b�hID
 * ����3    �F LPTSTR lpStr ���O�t�@�C���t���p�X�o�b�t�@
 * �߂�l   �F ���O�t�@�C���t���p�X
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
LPCTSTR CCmLog::CreateLogFileName( LPCTSTR lpcCategory, LPCTSTR lpcCategoryId, LPTSTR lpStr )
{
	TCHAR szModulePath[ MAX_PATH ];
	TCHAR szModuleName[ _MAX_PATH ];
	TCHAR szLogPath[ _MAX_PATH ];

	ZeroMemory( szLogPath, sizeof( szLogPath ) );

	GetModuleFileName( GetModuleHandle( NULL ), szModulePath, sizeof( szModulePath ) / sizeof( TCHAR ) );
	
	LPCTSTR lpfn = _tcsrchr( szModulePath, _T( '\\' ) );
	if ( NULL != lpfn ) // �o�b�N�X���b�V�����܂�ł�΁A����ȍ~�����g�p����B
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
 * �֐���   �F ReadIni
 * �@�\     �F �ݒ�t�@�C���Ǎ�
 * ����1    �F path �ݒ�t�@�C���p�X
 * ����2    �F lpcCategory �J�e�S��
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2011.06.16
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
 * �֐���   �F NextLog
 * �@�\     �F ���O�t�@�C�����쐬
 * ����1    �F LPSTR lpBuff �����݃o�b�t�@
 * ����2    �F DWORD& nNumberOfBytesToWrite �����݃o�b�t�@�T�C�Y
 * �߂�l   �F ���O�L���[�擾
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F Dump
 * �@�\     �F �_���v�o��(�t�B���^�����O�@�\�t)
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
		va_start( argList, fmt ); /* pgr0834 CRT�̒��Ȃ̂łȂ̂ŁA���Ȃ� */
		Add( E_LOG_NONE, fmt, argList );
		va_end( argList );
	}
}

/*--------------------------------------------------------------------------*
 * �֐���   �F Dump
 * �@�\     �F �_���v�o��(�t�B���^�����O�@�\�t)
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
}	/* pgr0840 pLogData�́Apush��ŊǗ�����(�ŏI�I��CCmLog::NextLog()�֐���delete����) */

/*--------------------------------------------------------------------------*
 * �֐���   �F Byte2Hex
 * �@�\     �F �o�C�g�𕶎��ɕϊ�
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmLog::Byte2Hex( BYTE data, LPSTR hi, LPSTR lo )
{
	*hi = ( ( data >> 4 ) & 0x0F ) + 0x30;
	if ( 0x3A <= *hi ) *hi += 7;
	*lo = ( data & 0x0F ) + 0x30;
	if ( 0x3A <= *lo ) *lo += 7;
}

/*--------------------------------------------------------------------------*
 * �֐���   �F PrintHex
 * �@�\     �F �o�C�i���𕶎���ɕϊ�
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F Put
 * �@�\     �F ���O�o��
 * ����1    �F EN_LOGLV lv ���O���x��
 * ����2    �F LPCSTR fmt �ψ����t�H�[�}�b�g
 * ����3    �F �ψ���
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmLog::Put( EN_LOGLV lv, LPCSTR fmt, ... )
{
	if ( E_LOGMODE != m_mode ) return;
	if ( E_LOG_I1LV < lv && m_lv < lv ) return;

	va_list argList;
	va_start( argList, fmt ); /* pgr0834 CRT�̒��Ȃ̂łȂ̂ŁA���Ȃ� */
	Add( lv, fmt, argList );
	va_end( argList );
}

/*--------------------------------------------------------------------------*
 * �֐���   �F Add
 * �@�\     �F ���O�o��
 * ����1    �F EN_LOGLV lv ���O���x��
 * ����2    �F LPCSTR fmt �ψ����t�H�[�}�b�g
 * ����3    �F argList �ψ���
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
}	/* pgr0840 pData�́Apush��ŊǗ�����(�ŏI�I��CCmLog::NextLog()�֐���delete����) */

/*--------------------------------------------------------------------------*
 * �֐���   �F ErrorFormatMessage
 * �@�\     �F �G���[�t�H�[�}�b�g���O�o��
 * ����1    �F EN_LOGLV lv ���O���x��
 * ����2    �F LPCSTR func �֐���
 * ����2    �F DWORD lasterrno ���X�g�G���[�ԍ�
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F Exec
 * �@�\     �F ���O�����݃X���b�h
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 *						�t�@�C��(CmLog.cpp)�̏I���
 *--------------------------------------------------------------------------*/
