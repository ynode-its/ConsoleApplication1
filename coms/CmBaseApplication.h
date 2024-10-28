/*--------------------------------------------------------------------------*
 *	ファイル名	： CmBaseApplication.h
 *	概要		： アプリケーションクラス
 *	作成者		： ITS Y.Node
 *	作成日		： 2015.06.15
 *	修正履歴	：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
 *--------------------------------------------------------------------------*/

#ifndef _CMBASEAPPLICATION_H_
#define _CMBASEAPPLICATION_H_


#define D_LOG_SAVE_TIMER_ID (1)
#define D_LOG_ROADCHANGE_TIMER_ID (2)

#define D_LOG_SAVE_INTERVAL (1000)
#define D_LOG_ROADCHANGE_INTERVAL (10)

/*--------------------------------------------------------------------------*
 *  クラス名    ： CCmBaseApplication
 *  継承クラス  ： CCmBase
 *	概要		： アプリケーションクラス
 *  作成者      ： ITS Y.Node
 *  作成日      ： 2015.06.15
 *  修正履歴    ：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
 *--------------------------------------------------------------------------*/
class CCmBaseApplication : public CCmBase
{
protected:
	HINSTANCE	m_hInstance;

public:
	LPCTSTR		m_lpCmdLine;
	HWND		m_hMainWnd;

protected:
	static LRESULT CALLBACK _WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
	virtual LPCTSTR GetWndClassName() = 0;
	virtual LPCTSTR GetWndName() = 0;

	virtual void OnCreate();
	virtual void OnClose();
	virtual void OnDestroy();
	virtual void OnRecvProcessMessage( UINT message, WPARAM wParam, LPARAM lParam );
	virtual void OnTimer( int eventid );

public:
	CCmBaseApplication();
	virtual ~CCmBaseApplication();

	virtual LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );

	virtual BOOL SnapShotMain();
	virtual BOOL InitInstance( HINSTANCE handle, int nCmdShow );
	virtual BOOL ExitInstance();
	ATOM MyRegisterClass();
	int Run();

	operator HINSTANCE() { return m_hInstance; }
	operator HWND() { return m_hMainWnd; }
};

#endif	// End of _CMBASEAPPLICATION_H_

/*--------------------------------------------------------------------------*
 *						ファイル(CmBaseApplication.h)の終わり
 *--------------------------------------------------------------------------*/
