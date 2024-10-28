/*--------------------------------------------------------------------------*
 *	�t�@�C����	�F CmThread.h
 *	�T�v		�F ���ʃX���b�h�w�b�_
 *	�쐬��		�F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *	�C������	�F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/

#ifndef _CMTHREAD_H_
#define _CMTHREAD_H_

/*--------------------------------------------------------------------------*
 *  �N���X��    �F CCmCriticalSection
 *  �p���N���X  �F
 *	�T�v		�F ���ʃN���e�B�J���Z�N�V�����N���X
 *  �쐬��      �F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *  �C������    �F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/
class CCmCriticalSection
{
public:
	CCmCriticalSection() { ::InitializeCriticalSection( &cs ); }
	~CCmCriticalSection() { ::DeleteCriticalSection( &cs ); }

	void Lock() { ::EnterCriticalSection( &cs ); }
	void Unlock() { ::LeaveCriticalSection( &cs ); }

protected:
	CRITICAL_SECTION cs;
};

/*--------------------------------------------------------------------------*
 *  �N���X��    �F CCmLock
 *  �p���N���X  �F
 *	�T�v		�F ���ʃ��b�N�N���X
 *  �쐬��      �F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *  �C������    �F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/
class CCmLock
{
public:
	CCmLock( CCmCriticalSection &_cs ): cs( _cs ) { cs.Lock(); }
	~CCmLock() { cs.Unlock(); }

protected:
	CCmCriticalSection &cs;
} ;

/*--------------------------------------------------------------------------*
 *  �N���X��    �F CCmThread
 *  �p���N���X  �F CCmCriticalSection
 *	�T�v		�F ���ʃ��b�N�N���X
 *  �쐬��      �F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *  �C������    �F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/
class CCmThread : public CCmCriticalSection
{
	friend DWORD WINAPI ThreadEntry( LPVOID param );
protected:
	virtual long Exec() = 0;

public:
	CCmThread();
	~CCmThread();

	virtual void KillThread();

	BOOL Create( DWORD flags = 0, DWORD stack = 0, LPSECURITY_ATTRIBUTES sa = NULL );
	virtual void OnBeforThread(){}

	operator HANDLE() const;

	int GetThreadPriority();
	BOOL SetThreadPriority( int nPriority );

	DWORD SuspendThread();
	DWORD ResumeThread();

public:
	HANDLE		m_EventWait;
	HANDLE		m_EventKill;
	HANDLE		m_EventDead;
	HANDLE		m_hThread;
	DWORD		m_ThreadID;
};

/*--------------------------------------------------------------------------*
 * �֐���   �F operator HANDLE
 * �@�\     �F �X���b�h�n���h���I�y���[�^
 * �߂�l   �F HANDLE
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
inline CCmThread::operator HANDLE() const
{
	return this == NULL? NULL: m_hThread;
}

/*--------------------------------------------------------------------------*
 * �֐���   �F SetThreadPriority
 * �@�\     �F �X���b�h�D��ݒ�
 * ����1	�F �D�揇��
 * �߂�l   �F HANDLE
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
inline BOOL CCmThread::SetThreadPriority( int nPriority )
{
	return ::SetThreadPriority( m_hThread, nPriority );
}

/*--------------------------------------------------------------------------*
 * �֐���   �F GetThreadPriority
 * �@�\     �F �X���b�h�D��擾
 * �߂�l   �F HANDLE
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
inline int CCmThread::GetThreadPriority()
{
	return ::GetThreadPriority( m_hThread );
}

/*--------------------------------------------------------------------------*
 * �֐���   �F ResumeThread
 * �@�\     �F �X���b�h���f
 * �߂�l   �F �T�X�y���h�J�E���g
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
inline DWORD CCmThread::ResumeThread()
{
	return ::ResumeThread( m_hThread );
}

/*--------------------------------------------------------------------------*
 * �֐���   �F SuspendThread
 * �@�\     �F �X���b�h�ĊJ
 * �߂�l   �F �T�X�y���h�J�E���g
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
inline DWORD CCmThread::SuspendThread()
{
	return ::SuspendThread( m_hThread );
}

#endif	// End of _CMTHREAD_H_

/*--------------------------------------------------------------------------*
 *						�t�@�C��(CmThread.h)�̏I���
 *--------------------------------------------------------------------------*/

