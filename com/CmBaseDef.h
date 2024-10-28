/*--------------------------------------------------------------------------*
 *	ファイル名	： CmBaseDef.h
 *	概要		： 定義ヘッダ
 *	作成者		： ITS Y.Node
 *	作成日		： 2015.06.15
 *	(C)Copyright 2015  Automotive & Industrial Systems Company, Panasonic Co.,Ltd
 *	修正履歴	：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
 *--------------------------------------------------------------------------*/

#ifndef _CMBASEDEF_H_
#define _CMBASEDEF_H_

#include <assert.h>
#include "CmBase.h"
#include "CmBaseApplication.h"
#include "CmBaseState.h"
#include "CmBaseControl.h"
#include "CmBaseStateManager.h"
#include "CmThread.h"
#include "CmLog.h"
#include "Audio/CmDefType.h"

typedef int ( *Compare )( const void *, const void * );

#define __NFILE__ ( strrchr( __FILE__, '\\' ) + 1 )

#define DELETE_ALLOC( var ) if( NULL != var ){ delete var; var = NULL; }

#define ISTRUE( var ) if( TRUE == var )
#define NOTTRUE( var ) if( TRUE != var )

#define ISFALSE( var ) if( FALSE == var )
#define NOTFALSE( var ) if( FALSE != var )

#define ISNULL( var ) if( NULL == var )
#define NOTNULL( var ) if( NULL != var )

#define ISERROR( var ) if( CM_E_ERROR == var )
#define NOTERROR( var ) if( CM_E_ERROR != var )

#define ISERRORS( var ) if( CM_E_ERROR >= var )
#define NOTERRORS( var ) if( CM_E_ERROR < var )

#define ISOK( var ) if( CM_S_OK == var )
#define NOTOK( var ) if( CM_S_OK != var )

#define ISOKS( var ) if( CM_S_OK <= var )
#define NOTOKS( var ) if( CM_S_OK > var )

#define ISRESOK( var ) if( RES_OK == var )
#define NOTRESOK( var ) if( RES_OK != var )

#define ISRESNG( var ) if( RES_NG == var )
#define NOTRESNG( var ) if( RES_NG != var )

#define CM_SUCCESS_USER( s ) CM_RESULT_USER( s )				// エラー以外
#define CM_ERROR_USER( e ) ( CM_RESULT_USER( e ) * ( -1 )	)	// エラー
#define CM_RESULT_USER( id ) ( 1000 + id )						// ユーザ定義

#define RES_NG	0
#define RES_OK	1

extern LPCSTR CmUtf16ToSjis( char* dst_buf, LPCWSTR wszSrc, int size, BOOL endianLE = TRUE );
extern LPCSTR CmUtf8ToSjis( char* pSjis, const char* utf8, int size, int* outSize = 0 );
extern BOOL CmPostMessage( HWND hWnd, UINT Msg, WPARAM wParam = 0, LPARAM lParam = 0 );
extern void* CmBsearch ( const void* key, const void *base, size_t num, size_t width, int ( *_Cmp_ )( const void *, const void * ) );
extern const char* bin2str( char* buff, int strSize, LPBYTE pb, int pbSize );
extern const wchar_t* bin2wstr( wchar_t* buff, int strSize, LPBYTE pb, int pbSize );

#endif	// End of _CMBASEDEF_H_

/*--------------------------------------------------------------------------*
 *						ファイル(CmBaseDef.h))の終わり
 *--------------------------------------------------------------------------*/
