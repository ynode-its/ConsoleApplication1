/*--------------------------------------------------------------------------*
 *	�t�@�C����	�F CmUtil.cpp
 *	�T�v		�F ���[�e�B���e�B
 *	�쐬��		�F ITS Y.Node
 *	�쐬��		�F 2015.06.15
 *	�C������	�F
 *	2015.06.15 ITS Y.Node  ---------  �V�K�쐬
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
 * �C���N���[�h
 *--------------------------------------------------------------------------*/
#include "StdAfx.h"

/*--------------------------------------------------------------------------*
 * �֐���   �F CmPostMessage
 * �@�\     �F �Ώۂ̃��W���[���փ��b�Z�[�W�𑗐M����
 * �߂�l   �F TRUE�͑��M�����AFALSE�͑��M�ł��Ȃ�����
 * ����1    �F HWND �E�B���h�E�n���h��
 * ����2    �F UINT Msg ���M���b�Z�[�WID
 * ����3    �F WPARAM wParam ���M�p�����[�^
 * ����4    �F LPARAM lParam ���M�p�����[�^
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
 *--------------------------------------------------------------------------*/
BOOL CmPostMessage( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
	return ::PostMessage( hWnd, Msg, wParam, lParam );
}

/*--------------------------------------------------------------------------*
 * �֐���   �F CmBsearch
 * �@�\     �F �o�C�i���[����
 * ����1    �F �����L�[
 * ����2    �F �����Ώ�
 * ����3    �F ����
 * ����4    �F �T�C�Y
 * ����5    �F ��r�֐�
 * �߂�l   �F �������ʁ@������Ȃ�����NULL
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2015.06.15
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
 * �֐���   �F CmUtf16ToSjis
 * �@�\     �F UNICODE����SJIS�ϊ�
 * �߂�l   �F SJIS
 * ����1    �F char* dst_buf �ϊ��i�[�o�b�t�@
 * ����2    �F LPCWSTR wszSrc �ϊ��O������
 * ����3    �F int size �ϊ��i�[�o�b�t�@�T�C�Y
 * ����4    �F endianLE TRUE:���g���G���f�B�A���AFALSE:�r�b�N�G���f�B�A��
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2011.06.02
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
 * �֐���   �F CmUtf8ToSjis
 * �@�\     �F UNICODE����SJIS�ϊ�
 * �߂�l   �F SJIS
 * ����1    �F utf8	 �ϊ��i�[�o�b�t�@
 * ����2    �F pSjis �ϊ��O������
 * ����3    �F size �ϊ��i�[�o�b�t�@�T�C�Y
 * �쐬��   �F ITS Y.Node
 * �쐬��   �F 2011.06.02
 *--------------------------------------------------------------------------*/
LPCSTR CmUtf8ToSjis( char* pSjis, const char* utf8, int size, int* outSize )
{
	LPWSTR utf16 = NULL;

	if ( utf8 == NULL )
	{
		return NULL;
	}

	//==============================//
	//	UTF8=��UTF16�ϊ�			//
	//==============================//
	const int wlen = MultiByteToWideChar( CP_UTF8, 0, utf8, -1, NULL, 0 );
	if ( !wlen )
	{
		return NULL;
	}

	//	utf16�̗̈�m��
	utf16 = new wchar_t[ wlen + 1 ];
	if ( utf16 == NULL )
	{
		return NULL;
	}

	//	utf16�ϊ�
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
	//	UTF16=��ShiftJIS�ϊ�		//
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

	//	utf16�ϊ�
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
 *						�t�@�C��(CmUtil.cpp)�̏I���
 *--------------------------------------------------------------------------*/
