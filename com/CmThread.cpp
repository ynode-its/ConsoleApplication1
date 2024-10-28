#include "StdAfx.h"
#include "CmThread.h"

// �X���b�h�N�����̏��

struct THREAD_STARTUP
{
	CCmThread *thread;

	HANDLE hEvent;
	HANDLE hEvent2;
	bool error;
};

/*--------------------------------------------------------------------------*
 * �֐���   �F ThreadEntry
 * �@�\     �F �X���b�h�G���g���[�|�C���g
 * ����1    �F LPVOID param �p�����[�^
 * �߂�l   �F DWORD
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F CCmThread
 * �@�\     �F �R���X�g���N�^
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
CCmThread::CCmThread()
	:m_hThread( 0 )
{
	m_EventKill = ::CreateEvent( NULL, TRUE, FALSE, NULL );
	m_EventDead = ::CreateEvent( NULL, TRUE, FALSE, NULL );
	m_EventWait = ::CreateEvent( NULL, TRUE, FALSE, NULL );
}

// �f�X�g���N�^

/*--------------------------------------------------------------------------*
 * �֐���   �F ~CCmThread
 * �@�\     �F �f�X�g���N�^
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F Create
 * �@�\     �F �X���b�h�쐬
 * ����1    �F DWORD flags �쐬�I�v�V����
 * ����2    �F DWORD stack �����̃X�^�b�N�T�C�Y
 * ����3    �F LPSECURITY_ATTRIBUTES sa �Z�L�����e�B�L�q�q
 * �߂�l   �F BOOL
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F KillThread
 * �@�\     �F �X���b�h��~
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
