/*--------------------------------------------------------------------------*
 *	ファイル名	： CCmBaseApplication.cpp
 *	概要		： アプリケーションクラス
 *	作成者		： ITS Y.Node
 *	作成日		： 2015.06.15
 *	修正履歴	：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 * インクルード
 *--------------------------------------------------------------------------*/
#include "StdAfx.h"
#include "CmBase.h"
#include "CmBaseApplication.h"
#include "B2BControlDef.h"

CCmBaseApplication* pApplication = 0;

#if defined( UNDER_CE )
#	define wWinMain   WinMain // On CE it's always WinMain
#endif

/*--------------------------------------------------------------------------*
 * 関数名： _tWinMain
 * 機能  ： エントリーポイント
 * 戻り値： wParamパラメータ値: WM_QUITメッセージ受信で正常終了する場合
 *       ： 0				  : メッセージループに入る前に終了する場合
 * 引数  ： HINSTANCE hInstance		: 現在のインスタンスのハンドル
 *       ： HINSTANCE hPrevInstance : 以前のインスタンスのハンドル
 *       ： LPTSTR lpCmdLine		: コマンドライン
 *       ： INT    nCmdShow			: 表示状態
 * 作成者： ITS Y.Node
 * 作成日： 2015.06.15
 *--------------------------------------------------------------------------*/
int WINAPI _tWinMain( HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow )
{
	int cmd = -1;

	if ( NULL == pApplication )
	{
		return -1;
	}
	if ( FALSE == pApplication->SnapShotMain() )
	{
		return -2;
	}
	pApplication->m_lpCmdLine = lpCmdLine;

	if ( pApplication->InitInstance( hInstance, nCmdShow ) )
	{
		cmd = pApplication->Run();
		pApplication->ExitInstance();
	}
	else
	{
		pApplication->ExitInstance();
	}

	return cmd;
}

/*--------------------------------------------------------------------------*
 * 関数名： _WndProc
 * 機能  ： メイン ウィンドウのメッセージを処理します。
 * 作成者： ITS Y.Node
 * 作成日： 2015.06.15
 *--------------------------------------------------------------------------*/
LRESULT CALLBACK CCmBaseApplication::_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	return pApplication->WndProc( hWnd, message, wParam, lParam );
}

/*--------------------------------------------------------------------------*
 * 関数名   ： CCmBaseApplication
 * 機能     ： コンストラクタ
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
CCmBaseApplication::CCmBaseApplication()
{
	m_hMainWnd = NULL;
	pApplication = this;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： ~CCmBaseApplication
 * 機能     ： デストラクタ
 * 戻り値   ： なし
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
CCmBaseApplication::~CCmBaseApplication()
{
}

/*--------------------------------------------------------------------------*
 * 関数名   ： Run
 * 機能     ： アプリケーション実行
 * 戻り値   ： 終了コード
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
int CCmBaseApplication::Run()
{
	MSG msg;

	while ( GetMessage( &msg, NULL, 0, 0 ) )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}

	return ( int )msg.wParam;
}

/*--------------------------------------------------------------------------*
 * 関数名： MyRegisterClass
 * 機能  ： ウィンドウ クラスを登録します。
 * 戻り値： ATOM:RegisterClass関数の戻り値
 * 引数  ： HINSTANCE hInstance		: インスタンスハンドル
 * 作成者： ITS Y.Node
 * 作成日： 2015.06.15
 *--------------------------------------------------------------------------*/
ATOM CCmBaseApplication::MyRegisterClass()
{
	WNDCLASS wc;

	wc.hbrBackground = ( HBRUSH )GetStockObject( BLACK_BRUSH );
	wc.style         = 0;
	wc.lpfnWndProc   = ( WNDPROC )CCmBaseApplication::_WndProc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = m_hInstance;
	wc.hIcon         = NULL;
	wc.hCursor       = NULL;
	wc.lpszClassName = GetWndClassName();
	wc.lpszMenuName  = 0;

	return RegisterClass( &wc );
}

/*--------------------------------------------------------------------------*
 * 関数名   ： InitInstance
 * 機能     ： 初期化
 * 引数1	： handle インスタンスハンドル
 * 引数2	： nCmdShow
 * 戻り値   ： TRUE、FALSE
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
BOOL CCmBaseApplication::InitInstance( HINSTANCE handle, int nCmdShow )
{
	m_hInstance = handle;

	MyRegisterClass();

	HWND hWnd = CreateWindow(
		GetWndClassName(),
		GetWndName(),
		WS_SYSMENU,
		0, 0, 0, 0,
		NULL,
		NULL,
		m_hInstance,
		NULL );

	if ( !hWnd )
	{
		return FALSE;
	}

	ShowWindow( hWnd, nCmdShow );

	return TRUE;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： InitInstance
 * 機能     ： 終了
 * 戻り値   ： TRUE、FALSE
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
BOOL CCmBaseApplication::ExitInstance()
{
	return TRUE;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： OnCreate
 * 機能     ： ウィンドウ作成
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmBaseApplication::OnCreate()
{
}

/*--------------------------------------------------------------------------*
 * 関数名   ： OnClose
 * 機能     ： ウィンドウを閉じる
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmBaseApplication::OnClose()
{
	::DestroyWindow( m_hMainWnd );
}

/*--------------------------------------------------------------------------*
 * 関数名   ： SnapShotMain
 * 機能     ： スナップショットメイン
 * 戻り値   ： TRUE、FALSE
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2012.02.21
 *--------------------------------------------------------------------------*/
BOOL CCmBaseApplication::SnapShotMain()
{
	return TRUE;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： OnDestroy
 * 機能     ： アプリケーション破棄
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmBaseApplication::OnDestroy()
{
	PostQuitMessage( 0 );
}

/*--------------------------------------------------------------------------*
 * 関数名   ： OnTimer
 * 機能     ： タイマーイベント
 * 引数1	： eventid 	イベントID
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmBaseApplication::OnTimer( int eventid )
{
}

/*--------------------------------------------------------------------------*
 * 関数名   ： OnRecvProcessMessage
 * 機能     ： シーケンスメッセージ受信
 * 引数1	： message 	受信メッセージ
 * 引数2	： wParam 	受信パラメータ
 * 引数3	： lParam 	受信パラメータ
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmBaseApplication::OnRecvProcessMessage( UINT message, WPARAM wParam, LPARAM lParam )
{
}

/*--------------------------------------------------------------------------*
 * 関数名： WndProc
 * 機能  ： メイン ウィンドウのメッセージを処理します。
 * 作成者： ITS Y.Node
 * 作成日： 2015.06.15
 *--------------------------------------------------------------------------*/
LRESULT CALLBACK CCmBaseApplication::WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	switch ( message )
	{
	case WM_CREATE:
		m_hMainWnd = hWnd;
		OnCreate();
		break;
	case WM_CLOSE:
		OnClose();
		break;
	case WM_DESTROY:
		OnDestroy();
		break;
	case WM_TIMER:
		OnTimer( wParam );
		break;
	default:
		// WM_USER範囲内
		if( WM_USER <= message && message <= 0x7FFF )
		{
			OnRecvProcessMessage( message, wParam, lParam );
			break;
		}
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

/*--------------------------------------------------------------------------*
 *				ファイル(CCmBaseApplication.cpp)の終わり
 *--------------------------------------------------------------------------*/

