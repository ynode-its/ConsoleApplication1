/*--------------------------------------------------------------------------*
 *	ファイル名	： CmThread.h
 *	概要		： 共通スレッドヘッダ
 *	作成者		： ITS Y.Node
 *	作成日		： 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *	修正履歴	：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
 *--------------------------------------------------------------------------*/

#ifndef _CMTHREAD_H_
#define _CMTHREAD_H_

/*--------------------------------------------------------------------------*
 *  クラス名    ： CCmCriticalSection
 *  継承クラス  ：
 *	概要		： 共通クリティカルセクションクラス
 *  作成者      ： ITS Y.Node
 *	作成日		： 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *  修正履歴    ：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
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
 *  クラス名    ： CCmLock
 *  継承クラス  ：
 *	概要		： 共通ロッククラス
 *  作成者      ： ITS Y.Node
 *	作成日		： 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *  修正履歴    ：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
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
 *  クラス名    ： CCmThread
 *  継承クラス  ： CCmCriticalSection
 *	概要		： 共通ロッククラス
 *  作成者      ： ITS Y.Node
 *	作成日		： 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *  修正履歴    ：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
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
 * 関数名   ： operator HANDLE
 * 機能     ： スレッドハンドルオペレータ
 * 戻り値   ： HANDLE
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
inline CCmThread::operator HANDLE() const
{
	return this == NULL? NULL: m_hThread;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： SetThreadPriority
 * 機能     ： スレッド優先設定
 * 引数1	： 優先順位
 * 戻り値   ： HANDLE
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
inline BOOL CCmThread::SetThreadPriority( int nPriority )
{
	return ::SetThreadPriority( m_hThread, nPriority );
}

/*--------------------------------------------------------------------------*
 * 関数名   ： GetThreadPriority
 * 機能     ： スレッド優先取得
 * 戻り値   ： HANDLE
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
inline int CCmThread::GetThreadPriority()
{
	return ::GetThreadPriority( m_hThread );
}

/*--------------------------------------------------------------------------*
 * 関数名   ： ResumeThread
 * 機能     ： スレッド中断
 * 戻り値   ： サスペンドカウント
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
inline DWORD CCmThread::ResumeThread()
{
	return ::ResumeThread( m_hThread );
}

/*--------------------------------------------------------------------------*
 * 関数名   ： SuspendThread
 * 機能     ： スレッド再開
 * 戻り値   ： サスペンドカウント
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
inline DWORD CCmThread::SuspendThread()
{
	return ::SuspendThread( m_hThread );
}

#endif	// End of _CMTHREAD_H_

/*--------------------------------------------------------------------------*
 *						ファイル(CmThread.h)の終わり
 *--------------------------------------------------------------------------*/

