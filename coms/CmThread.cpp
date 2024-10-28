#include "StdAfx.h"
#include "CmThread.h"

// スレッド起動時の情報

struct THREAD_STARTUP
{
	CCmThread *thread;

	HANDLE hEvent;
	HANDLE hEvent2;
	bool error;
};

/*--------------------------------------------------------------------------*
 * 関数名   ： ThreadEntry
 * 機能     ： スレッドエントリーポイント
 * 引数1    ： LPVOID param パラメータ
 * 戻り値   ： DWORD
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
DWORD WINAPI ThreadEntry( LPVOID param )
{
	THREAD_STARTUP *startup = ( THREAD_STARTUP * )param;

	CCmThread *thread = startup->thread;
	thread->OnBeforThread();

	HANDLE hEvent2 = startup->hEvent2;
	::SetEvent( startup->hEvent );

	::WaitForSingleObject( hEvent2, INFINITE );
	::CloseHandle( hEvent2 );

	DWORD result = thread->Exec();

	::SetEvent( thread->m_EventDead );

	ExitThread( result );

	return 0;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： CCmThread
 * 機能     ： コンストラクタ
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
CCmThread::CCmThread()
	:m_hThread( 0 )
{
	m_EventKill = ::CreateEvent( NULL, TRUE, FALSE, NULL );
	m_EventDead = ::CreateEvent( NULL, TRUE, FALSE, NULL );
	m_EventWait = ::CreateEvent( NULL, TRUE, FALSE, NULL );
}

// デストラクタ

/*--------------------------------------------------------------------------*
 * 関数名   ： ~CCmThread
 * 機能     ： デストラクタ
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
CCmThread::~CCmThread()
{
	::CloseHandle( m_EventKill );
	::CloseHandle( m_EventDead );
	::CloseHandle( m_EventWait );

	if ( m_hThread )
	{
		::CloseHandle( m_hThread );
	}
}

/*--------------------------------------------------------------------------*
 * 関数名   ： Create
 * 機能     ： スレッド作成
 * 引数1    ： DWORD flags 作成オプション
 * 引数2    ： DWORD stack 初期のスタックサイズ
 * 引数3    ： LPSECURITY_ATTRIBUTES sa セキュリティ記述子
 * 戻り値   ： BOOL
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
BOOL CCmThread::Create( DWORD flags, DWORD stack, LPSECURITY_ATTRIBUTES sa )
{
	Lock();
	if ( m_hThread )
	{
		Unlock();
		return TRUE;
	}

	THREAD_STARTUP startup;
	memset( &startup, 0, sizeof( startup ) );

	startup.thread = this;
	startup.hEvent = ::CreateEvent( NULL, TRUE, FALSE, NULL );
	startup.hEvent2 = ::CreateEvent( NULL, TRUE, FALSE, NULL );

	if ( startup.hEvent == NULL || startup.hEvent2 == NULL )
	{
		if ( startup.hEvent != NULL )
		{
			::CloseHandle( startup.hEvent );
		}
		if ( startup.hEvent2 != NULL )
		{
			::CloseHandle( startup.hEvent2 );
		}
		Unlock();
		return FALSE;
	}
  
	m_hThread = CreateThread( sa, stack, &ThreadEntry, &startup, flags | CREATE_SUSPENDED, &m_ThreadID );
	Unlock();
	if ( m_hThread == NULL )
	{
		return FALSE;
	}

	ResumeThread();
	::WaitForSingleObject( startup.hEvent, INFINITE );
	::CloseHandle( startup.hEvent );

	if ( flags & CREATE_SUSPENDED )
	{
		::SuspendThread( m_hThread );
	}

	if ( startup.error )
	{
		::WaitForSingleObject( m_hThread, INFINITE );
		::CloseHandle( m_hThread );
		m_hThread = NULL;
		::CloseHandle( startup.hEvent2 );
		return FALSE;
	}

	::SetEvent( startup.hEvent2 );
	return TRUE;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： KillThread
 * 機能     ： スレッド停止
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmThread::KillThread()
{
	if ( m_hThread )
	{
		::SetEvent( m_EventWait );
		::SetEvent( m_EventKill );

		SetThreadPriority( THREAD_PRIORITY_ABOVE_NORMAL );
		::WaitForSingleObject( m_EventDead, INFINITE );
		::WaitForSingleObject( m_hThread, INFINITE );
	}
}
