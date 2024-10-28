/*--------------------------------------------------------------------------*
 *	ファイル名	： CmUtil.cpp
 *	概要		： ユーティリティ
 *	作成者		： ITS Y.Node
 *	作成日		： 2015.06.15
 *	修正履歴	：
 *	2015.06.15 ITS Y.Node  ---------  新規作成
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 * インクルード
 *--------------------------------------------------------------------------*/
#include "StdAfx.h"

/*--------------------------------------------------------------------------*
 * 関数名   ： CmPostMessage
 * 機能     ： 対象のモジュールへメッセージを送信する
 * 戻り値   ： TRUEは送信完了、FALSEは送信できなかった
 * 引数1    ： HWND ウィンドウハンドル
 * 引数2    ： UINT Msg 送信メッセージID
 * 引数3    ： WPARAM wParam 送信パラメータ
 * 引数4    ： LPARAM lParam 送信パラメータ
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
BOOL CmPostMessage( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	return ::PostMessage( hWnd, Msg, wParam, lParam );
}

/*--------------------------------------------------------------------------*
 * 関数名   ： CmBsearch
 * 機能     ： バイナリー検索
 * 引数1    ： 検索キー
 * 引数2    ： 検索対象
 * 引数3    ： 件数
 * 引数4    ： サイズ
 * 引数5    ： 比較関数
 * 戻り値   ： 検索結果　見つからない時はNULL
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2015.06.15
 *--------------------------------------------------------------------------*/
void* CmBsearch ( const void* key, const void *base, size_t num, size_t width, int ( *_Cmp_ )( const void *, const void * ) )
{
    char* lo = ( char* )base;
    char* hi = ( char* )base + ( num - 1 ) * width;
    char* mid;
    size_t half;
    int result;

    while (lo <= hi)
    {
        if ( ( half = num / 2 ) != 0 )
        {
            mid = lo + ( num & 1 ? half : ( half - 1 ) ) * width;
            if ( !( result = _Cmp_( key, mid ) ) )
                return( mid );
            else if ( result < 0 )
            {
                hi = mid - width;
                num = num & 1 ? half : half-1;
            }
            else
            {
                lo = mid + width;
                num = half;
            }
        }
        else if ( num )
            return ( _Cmp_( key, lo ) ? NULL : lo );
        else
            break;
    }

    return NULL;
}

char _bin2str( BYTE b )
{
	switch( b )
	{
	case 0:
		return '0';
	case 1:
		return '1';
	case 2:
		return '2';
	case 3:
		return '3';
	case 4:
		return '4';
	case 5:
		return '5';
	case 6:
		return '6';
	case 7:
		return '7';
	case 8:
		return '8';
	case 9:
		return '9';
	case 10:
		return 'A';
	case 11:
		return 'B';
	case 12:
		return 'C';
	case 13:
		return 'D';
	case 14:
		return 'E';
	case 15:
		return 'F';
	}
	return '\0';
}

wchar_t _bin2wstr( BYTE b )
{
	switch( b )
	{
	case 0:
		return L'0';
	case 1:
		return L'1';
	case 2:
		return L'2';
	case 3:
		return L'3';
	case 4:
		return L'4';
	case 5:
		return L'5';
	case 6:
		return L'6';
	case 7:
		return L'7';
	case 8:
		return L'8';
	case 9:
		return L'9';
	case 10:
		return L'A';
	case 11:
		return L'B';
	case 12:
		return L'C';
	case 13:
		return L'D';
	case 14:
		return L'E';
	case 15:
		return L'F';
	}
	return L'\0';
}

const char* bin2str( char* buff, int strSize, LPBYTE pb, int pbSize )
{
	LPBYTE p = pb;
	char* pStr = buff;
	int cnt = 0;

	*pStr = '\0';

	if ( 1 >= strSize )
	{
		return buff;
	}

	for ( int i = 0; i < pbSize; i++ )
	{
		if ( strSize <= ( cnt + 1 ) )
		{
			*pStr = '\0';
			return buff;
		}

		if ( i != 0)
		{
			*pStr = ' ';
		}
		else
		{
			*pStr = '[';
		}
		pStr++;
		cnt++;

		if ( strSize <= ( cnt + 4 ) )
		{
			*pStr = '\0';
			return buff;
		}

		*pStr = '0';
		pStr++;	cnt++;
		*pStr = 'X';
		pStr++;	cnt++;

		*pStr = _bin2str( ( *p & 0xF0 ) >> 4 );
		pStr++;	cnt++;

		*pStr = _bin2str( *p & 0x0F );
		pStr++;	cnt++;
		p++;
	}
	
	if ( strSize <= ( cnt + 2 ) )
	{
		*pStr = '\0';
		return buff;
	}
	*pStr = ']';
	pStr++;
	*pStr = '\0';

	return buff;
}

const wchar_t* bin2wstr( wchar_t* buff, int strSize, LPBYTE pb, int pbSize )
{
	LPBYTE p = pb;
	wchar_t* pStr = buff;
	int cnt = 0;

	buff = L'\0';

	if ( 1 >= strSize )
	{
		return buff;
	}

	for ( int i = 0; i < pbSize; i++ )
	{
		if ( strSize <= ( cnt + 1 ) )
		{
			*pStr = L'\0';
			return buff;
		}

		if ( i != 0)
		{
			*pStr = L' ';
		}
		else
		{
			*pStr = L'[';
		}
		pStr++;
		cnt++;

		if ( strSize <= ( cnt + 4 ) )
		{
			*pStr = L'\0';
			return buff;
		}

		*pStr = L'0';
		pStr++;	cnt++;
		*pStr = L'X';
		pStr++;	cnt++;

		*pStr = _bin2wstr( ( *p & 0xF0 ) >> 4 );
		pStr++;	cnt++;

		*pStr = _bin2wstr( *p & 0x0F );
		pStr++;	cnt++;
		p++;
	}
	
	if ( strSize <= ( cnt + 2 ) )
	{
		*pStr = L'\0';
		return buff;
	}
	*pStr = L']';
	pStr++;
	*pStr = L'\0';

	return buff;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： CmUtf16ToSjis
 * 機能     ： UNICODEからSJIS変換
 * 戻り値   ： SJIS
 * 引数1    ： char* dst_buf 変換格納バッファ
 * 引数2    ： LPCWSTR wszSrc 変換前文字列
 * 引数3    ： int size 変換格納バッファサイズ
 * 引数4    ： endianLE TRUE:リトルエンディアン、FALSE:ビックエンディアン
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2011.06.02
 *--------------------------------------------------------------------------*/
LPCSTR CmUtf16ToSjis( char* dst_buf, LPCWSTR wszSrc, int size, BOOL endianLE )
{
	LPCWSTR lpcStr = wszSrc;
	LPWSTR lpStr = NULL;

	if ( FALSE == endianLE )
	{
		wchar_t tmp;
		int wcLen = wcslen( wszSrc );
		lpStr = new WCHAR[ wcLen + 1 ];
		if ( NULL == lpStr )
		{
			return NULL;
		}
		lpStr[ wcLen ] = L'\0';
		for( int i = 0; i < wcLen; i++ )
		{
			tmp = wszSrc[ i ];
			lpStr[ i ] = ( ( ( tmp & 0xFF00 ) >> 8 ) | ( ( tmp & 0x00FF ) << 8 ) );
		}
		lpcStr = lpStr;
	}

	int nLen = WideCharToMultiByte( CP_ACP, 0, lpcStr, -1, NULL, 0, NULL, NULL );
	if( 0 == nLen )
	{
		delete[] lpStr;
		return NULL;
	}

	char* pSjis = new char[ nLen ];
	ZeroMemory( pSjis, nLen );
	WideCharToMultiByte( CP_ACP, 0, lpcStr, -1, ( LPSTR )pSjis, size - 1, NULL, NULL );

	strncpy( dst_buf, pSjis, size - 1 );
	dst_buf[ size - 1 ] = '\0';

	if ( NULL != lpStr )
	{
		delete[] lpStr;
	}
	delete[] pSjis;

	return ( LPCSTR )dst_buf;
}

/*--------------------------------------------------------------------------*
 * 関数名   ： CmUtf8ToSjis
 * 機能     ： UNICODEからSJIS変換
 * 戻り値   ： SJIS
 * 引数1    ： utf8	 変換格納バッファ
 * 引数2    ： pSjis 変換前文字列
 * 引数3    ： size 変換格納バッファサイズ
 * 作成者   ： ITS Y.Node
 * 作成日   ： 2011.06.02
 *--------------------------------------------------------------------------*/
LPCSTR CmUtf8ToSjis( char* pSjis, const char* utf8, int size, int* outSize )
{
	LPWSTR utf16 = NULL;

	if ( utf8 == NULL )
	{
		return NULL;
	}

	//==============================//
	//	UTF8=＞UTF16変換			//
	//==============================//
	const int wlen = MultiByteToWideChar( CP_UTF8, 0, utf8, -1, NULL, 0 );
	if ( !wlen )
	{
		return NULL;
	}

	//	utf16の領域確保
	utf16 = new wchar_t[ wlen + 1 ];
	if ( utf16 == NULL )
	{
		return NULL;
	}

	//	utf16変換
	if ( MultiByteToWideChar( CP_UTF8, 0, utf8, -1, utf16, wlen ) > 0 )
	{
		utf16[ wlen ] = L'\0';
	}
	else
	{
		delete[] utf16;
		return NULL;
	}

	//==============================//
	//	UTF16=＞ShiftJIS変換		//
	//==============================//
	const int mlen = WideCharToMultiByte( CP_ACP, 0, utf16, -1, NULL, 0, " ", NULL );
	if ( !mlen )
	{
		delete[] utf16;
		return NULL;
	}

	if ( NULL == pSjis )
	{
		if ( outSize )
		{
			*outSize = mlen;
		}
		delete[] utf16;
		return NULL;
	}

	//	utf16変換
	if ( WideCharToMultiByte( CP_ACP, 0, utf16, -1, ( LPSTR )pSjis, size, " ", NULL ) > 0 )
	{
		pSjis[ mlen ] = 0;
	}
	else
	{
		delete[] utf16;
		return NULL;
	}

	delete[] utf16;

	return pSjis;
}

/*--------------------------------------------------------------------------*
 *						ファイル(CmUtil.cpp)の終わり
 *--------------------------------------------------------------------------*/
