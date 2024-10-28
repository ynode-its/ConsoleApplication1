/*--------------------------------------------------------------------------*
 *	�t�@�C����	�F CCmBaseApplication.cpp
 *	�T�v		�F �A�v���P�[�V�����N���X
 *	�쐬��		�F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	�C������	�F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 * �C���N���[�h
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
 * �֐����F _tWinMain
 * �@�\  �F �G���g���[�|�C���g
 * �߂�l�F wParam�p�����[�^�l: WM_QUIT���b�Z�[�W��M�Ő���I������ꍇ
 *       �F 0				  : ���b�Z�[�W���[�v�ɓ���O�ɏI������ꍇ
 * ����  �F HINSTANCE hInstance		: ���݂̃C���X�^���X�̃n���h��
 *       �F HINSTANCE hPrevInstance : �ȑO�̃C���X�^���X�̃n���h��
 *       �F LPTSTR lpCmdLine		: �R�}���h���C��
 *       �F INT    nCmdShow			: �\�����
 * �쐬�ҁF ITS Y.Node
 * �쐬���F 2015.06.15
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
 * �֐����F _WndProc
 * �@�\  �F ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
 * �쐬�ҁF ITS Y.Node
 * �쐬���F 2015.06.15
 *--------------------------------------------------------------------------*/
LRESULT CALLBACK CCmBaseApplication::_WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	return pApplication->WndProc( hWnd, message, wParam, lParam );
}

/*--------------------------------------------------------------------------*
 * �֐���   �F CCmBaseApplication
 * �@�\     �F �R���X�g���N�^
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
CCmBaseApplication::CCmBaseApplication()
{
	m_hMainWnd = NULL;
	pApplication = this;
}

/*--------------------------------------------------------------------------*
 * �֐���   �F ~CCmBaseApplication
 * �@�\     �F �f�X�g���N�^
 * �߂�l   �F �Ȃ�
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
CCmBaseApplication::~CCmBaseApplication()
{
}

/*--------------------------------------------------------------------------*
 * �֐���   �F Run
 * �@�\     �F �A�v���P�[�V�������s
 * �߂�l   �F �I���R�[�h
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐����F MyRegisterClass
 * �@�\  �F �E�B���h�E �N���X��o�^���܂��B
 * �߂�l�F ATOM:RegisterClass�֐��̖߂�l
 * ����  �F HINSTANCE hInstance		: �C���X�^���X�n���h��
 * �쐬�ҁF ITS Y.Node
 * �쐬���F 2015.06.15
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
 * �֐���   �F InitInstance
 * �@�\     �F ������
 * ����1	�F handle �C���X�^���X�n���h��
 * ����2	�F nCmdShow
 * �߂�l   �F TRUE�AFALSE
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F InitInstance
 * �@�\     �F �I��
 * �߂�l   �F TRUE�AFALSE
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
BOOL CCmBaseApplication::ExitInstance()
{
	return TRUE;
}

/*--------------------------------------------------------------------------*
 * �֐���   �F OnCreate
 * �@�\     �F �E�B���h�E�쐬
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmBaseApplication::OnCreate()
{
}

/*--------------------------------------------------------------------------*
 * �֐���   �F OnClose
 * �@�\     �F �E�B���h�E�����
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmBaseApplication::OnClose()
{
	::DestroyWindow( m_hMainWnd );
}

/*--------------------------------------------------------------------------*
 * �֐���   �F SnapShotMain
 * �@�\     �F �X�i�b�v�V���b�g���C��
 * �߂�l   �F TRUE�AFALSE
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2012.02.21
 *--------------------------------------------------------------------------*/
BOOL CCmBaseApplication::SnapShotMain()
{
	return TRUE;
}

/*--------------------------------------------------------------------------*
 * �֐���   �F OnDestroy
 * �@�\     �F �A�v���P�[�V�����j��
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmBaseApplication::OnDestroy()
{
	PostQuitMessage( 0 );
}

/*--------------------------------------------------------------------------*
 * �֐���   �F OnTimer
 * �@�\     �F �^�C�}�[�C�x���g
 * ����1	�F eventid 	�C�x���gID
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmBaseApplication::OnTimer( int eventid )
{
}

/*--------------------------------------------------------------------------*
 * �֐���   �F OnRecvProcessMessage
 * �@�\     �F �V�[�P���X���b�Z�[�W��M
 * ����1	�F message 	��M���b�Z�[�W
 * ����2	�F wParam 	��M�p�����[�^
 * ����3	�F lParam 	��M�p�����[�^
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
void CCmBaseApplication::OnRecvProcessMessage( UINT message, WPARAM wParam, LPARAM lParam )
{
}

/*--------------------------------------------------------------------------*
 * �֐����F WndProc
 * �@�\  �F ���C�� �E�B���h�E�̃��b�Z�[�W���������܂��B
 * �쐬�ҁF ITS Y.Node
 * �쐬���F 2015.06.15
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
		// WM_USER�͈͓�
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
 *				�t�@�C��(CCmBaseApplication.cpp)�̏I���
 *--------------------------------------------------------------------------*/

