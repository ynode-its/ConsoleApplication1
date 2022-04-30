// ConsoleApplication1.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <float.h>
#include <stdint.h>
#include <io.h>
#include<Windows.h>
#include "xprintf.h"

#define DBL_INTSIZE 309

#define DBL_FRACSIZE 16
#define DBL_FRACSIZE_ DBL_FRACSIZE + 5

struct          s_ifloat
{
    //uint8_t     sign;       // 符号1bitを収納
    //uint8_t     exp;        // 指数部8bitを収納
    //uint32_t    frac;       // 仮数部23bitを収納
    unsigned long long    frac;       // 仮数部52bitを収納
    uint16_t              exp;        // 指数部11bitを収納
    uint8_t               sign;       // 符号1bitを収納
    int8_t      intpart[DBL_INTSIZE];   // 10進数での整数部を保存
    union
    {
        struct
        {
            int8_t      fracpart[DBL_FRACSIZE_]; // 10進数での小数部を保存
        };
        struct
        {
            int8_t      dammy[DBL_FRACSIZE - 1]; // 10進数での小数部を保存
            int8_t      last;
            int8_t      rnd;
        };
    };
};

void            array_add(int8_t* a, int8_t* b, int size)
{
    int		i;

    i = size - 1;
    while (i >= 0)
    {
        a[i] += b[i];
        if (a[i] >= 10 && i != 0)
        {
            a[i] -= 10;
            a[i - 1] += 1;
        }
        i--;
    }
}

void            array_divbytwo(int8_t* n, int size)
{
    int		i;

    i = 0;
    while (i < size - 1)
    {
        n[i + 1] += (n[i] % 2) * 10;
        n[i] /= 2;
        i++;
    }
}

void            array_double(int8_t* n, int size)
{
    int		i;

    i = size - 1;
    while (i >= 0)
    {
        n[i] *= 2;
        if (i < size - 1 && n[i + 1] >= 10)
        {
            n[i] += 1;
            n[i + 1] -= 10;
        }
        i--;
    }
}

void store_ifloat(double num, s_ifloat* ifloat)
{
    unsigned long long  mem;

    memcpy(&mem, &num, sizeof(mem));
    //ifloat.sign = mem >> 31;
    //ifloat.exp = mem >> 23;
    //ifloat.frac = mem << 9;
    ifloat->sign = mem >> 63;
    ifloat->exp = (mem >> 52) & 0x07FF;
    ifloat->frac = mem << 12;
}

void round_away(struct s_ifloat* ifloat)
{
    int i = DBL_FRACSIZE - 1;

    while (0 <= i && ifloat->fracpart[i] == 9)
    {
        ifloat->fracpart[i] = 0;
        i--;
    }

    if (0 <= i)
    {
        ifloat->fracpart[i] += 1;
    }
    else
    {
        i = DBL_INTSIZE - 1;
        while (0 < i && ifloat->intpart[i--] == 9)
        {
            ifloat->intpart[i] = 0;
        }
        if (0 <= i)
        {
            ifloat->intpart[i] += 1;
        }
    }
}

void print_ifloat(char* obuf, struct s_ifloat* ifloat)
{
    int     i;
    int flg = 0;
    char    c;

    if (ifloat->exp == 2047 && ifloat->frac != 0)  // nanの場合
    {
        return;
    }

    if (ifloat->exp == 2047)                      // infの場合
    {
        return;
    }

    // 小数16桁目奇数、下位桁が5以上又は、6以上の場合切り上げ
    if ((0 != ifloat->last % 2 && ifloat->rnd == 5) ||
        6 <= ifloat->rnd)
    {
        round_away(ifloat);
    }

    if (0 != ifloat->sign)
    {
        *obuf++ = '-';
    }

    for (i = 0; i < DBL_INTSIZE; i++)           // 整数部の出力
    {
        if (flg == 0 && ifloat->intpart[i] == 0)
        {
            continue;
        }
        flg = 1;
        c = ifloat->intpart[i] + '0';
        *obuf++ = c;
    }
    if (flg == 0)
    {
        *obuf++ = '0';
    }
    *obuf++ = '.';

    for (i = 0; i < 16; i++)      // 小数部の出力
    {
        c = ifloat->fracpart[i] + '0';
        *obuf++ = c;
    }
    *obuf = 0;
}

void			convert_fracpart(struct s_ifloat* ifloat)
{
    int16_t      i;                  // カウンタ
    unsigned long long    fracpart_bin;       // fracの整数部分を格納
    int8_t      n[DBL_FRACSIZE_];    // 2^(i-1)を格納

    ZeroMemory(ifloat->fracpart, sizeof(ifloat->fracpart));
    ZeroMemory(n, sizeof(n));
    n[0] = 5;                       // 2^(-1)(=0.5)からスタート

    if (ifloat->exp >= 52 + 1023 || (ifloat->exp == 0 && ifloat->frac == 0)) // 小数部0の場合
        return;
    else if (ifloat->exp >= 1023)    // 一部のみ小数部の場合
        fracpart_bin = ifloat->frac << (ifloat->exp - 1023);
    else if (ifloat->exp == 0)      // 非正規数（ケチ表現分bitなし）
        fracpart_bin = ifloat->frac;
    else                            // 全て小数部かつ正規数（ケチ表現分bitを入れる）
        fracpart_bin = ifloat->frac >> 1 | (static_cast<unsigned long long>(1) << 63);

    for (i = 0; i < (1022 - ifloat->exp); i++)
        array_divbytwo(n, DBL_FRACSIZE_);    // あらかじめ指数に合わせて割っておく

    for (i = 0; i < 53; i++)
    {
        if (fracpart_bin & (static_cast<unsigned long long>(1) << (63 - i))) // bitが立っていればfracpartに足していく
            array_add(ifloat->fracpart, n, DBL_FRACSIZE_);
        array_divbytwo(n, DBL_FRACSIZE_);
    }
}
// 整数部の出力
void            convert_intpart(struct s_ifloat* ifloat)
{
    uint32_t     i;                  // カウンタ
    int         offset;             // fracの整数部分までのoffset
    unsigned long long    intpart_bin = 1;        // fracの整数部分を格納
    int8_t      n[DBL_INTSIZE];     // 2^i乗を格納

    ZeroMemory(ifloat->intpart, sizeof(ifloat->intpart));
    ZeroMemory(n, sizeof(n));
    n[DBL_INTSIZE - 1] = 1;     // n=1からスタート(最も右を1の位とする)

    if (ifloat->exp < 1023 || ifloat->exp == 2047) // 整数部は0
        return;
    else if (ifloat->exp < 1023 + 52) // ifloat->fracの一部が整数部
        offset = ifloat->exp - 1023;
    else // ifloat->exp >= 1023 + 52 => ifloat->fracの全てが整数部
        offset = 52;

    if (offset != 0)
        intpart_bin = (ifloat->frac >> (64 - offset)) | (static_cast<unsigned long long>(1) << offset);

    for (i = 0; i < 53; i++)    // 0~53ビットを確認する
    {
        if (intpart_bin & (static_cast<unsigned long long>(1) << i))     // bitが立っていれば2^n乗を足す
            array_add(ifloat->intpart, n, DBL_INTSIZE);
        array_double(n, DBL_INTSIZE);   // nを2倍して次へ
    }
    while (i++ <= ifloat->exp - 1023)    // iがexpより小さければその分2倍していく
        array_double(ifloat->intpart, DBL_INTSIZE);
}

void convert_ifloat(struct s_ifloat* ifloat)
{
    convert_intpart(ifloat);        // 整数部の出力
    convert_fracpart(ifloat);       // 小数部の出力
}

void printfloat(double num)
{
    struct s_ifloat ifloat;
    char buff[360];
    ZeroMemory(buff, sizeof(buff));

    store_ifloat(num, &ifloat);     // 符号部、指数部、仮数部を別々の変数に取り出す
    convert_ifloat(&ifloat);        // 2進数->10進数へ変換
    print_ifloat(buff, &ifloat);           // 標準出力へ出力
    printf("%s\n", buff);
}

void printcomp(double num)
{
    // printfの表示（nan, inf以外は整数部39ケタ、小数部149ケタ表示する）
    if (isnan(num) || isinf(num))
        printf("printf  : %.16f\n", num);
    else
        printf("printf  : %.16f\n", num);
    fflush(stdout);
    // 自作printfloatの表示（nan, inf以外は整数部39ケタ、小数部149ケタ表示する）
    write(1, "putfloat: ", 10);
    printfloat(num);
}

void            printcomp1(float num);

int             aaaaaa64(void)
{

    float	    num;
    uint32_t    mem;

    printcomp(1.2);
    printcomp(2312.31231002312349999);
    printcomp(987345397878232.31231002312349999);
    printcomp(FLT_MAX);
    printcomp(DBL_MAX);
    printcomp(1234567.123534534672247083456);
    printcomp(999.9999999999999999999);
    printcomp(1.7976931348623158e+30);     // 正規数の最大 = 3.40282347e+38F (float.hで定義)
    printcomp(DBL_MIN);     // 正規数の最小= 1.17549435e-38F (float.hで定義)
    mem = 1;
    memcpy(&num, &mem, sizeof(uint32_t));
    printcomp(num);         // 非正規数の最小 (仮数部の最下位bitのみ1)
    printcomp(INFINITY);     // inf
    printcomp(NAN);     // inf
    printcomp(-INFINITY);    // -inf
    return (0);
}

int             aaaaaa64d(void);


int             main(void)
{
    //asdasdsad();
    aaaaaa64();
    //aaaaaa64d();
	//  2312.3123100231237004
	//  2312.31231002312370038

    f16toa(2312.31231002312349999);
    printf("$%.16f\n", 2312.31231002312349999);
    f16toa(-2312.31231002312349999);
    printf("$%.16f\n", -2312.31231002312349999);
    f16toa(-0.31231002312349999);
    printf("$%.16f\n", -0.31231002312349999);
    f16toa(-1.2);
    printf("$%.16f\n", -1.2);
    f16toa(1.5);
    printf("$%.16f\n", 1.5);
    f16toa(0.000001);
    printf("$%.16f\n", 0.000001);
    f16toa(DBL_MAX);
    printf("$%.16f\n", DBL_MAX);
    f16toa(DBL_MIN);
    printf("$%.16f\n", DBL_MIN);
    xprintf("$%.16f\n", DBL_MAX);
}