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

#define FLT_FRACSIZE 20	// 10進数での小数部の最大桁数149 + 計算用余裕1
#define FLT_INTSIZE 308 		// 10進数での最大桁数 39

struct          s_ifloat
{
    //uint8_t     sign;       // 符号1bitを収納
    //uint8_t     exp;        // 指数部8bitを収納
    //uint32_t    frac;       // 仮数部23bitを収納
    unsigned long long    frac;       // 仮数部52bitを収納
    uint16_t              exp;        // 指数部11bitを収納
    uint8_t               sign;       // 符号1bitを収納
    int8_t      intpart[FLT_INTSIZE];   // 10進数での整数部を保存
    int8_t      fracpart[FLT_FRACSIZE]; // 10進数での小数部を保存
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

struct s_ifloat	store_ifloat(double num)
{
    struct s_ifloat	    ifloat;
    unsigned long long  mem;

    memcpy(&mem, &num, sizeof(mem));
    //ifloat.sign = mem >> 31;
    //ifloat.exp = mem >> 23;
    //ifloat.frac = mem << 9;
    ifloat.sign = mem >> 63;
    ifloat.exp = (mem >> 52) & 0x07FF;
    ifloat.frac = mem << 12;

    return (ifloat);
}

void			print_ifloat(struct s_ifloat ifloat)
{
    int     i;
    int flg = 0;
    char    c;

    if (ifloat.exp == 2047 && ifloat.frac != 0)  // nanの場合
    {
        write(1, "nan", 3);
        return;
    }
    if (ifloat.sign == 1)                       // 符号ビットが1の場合'-'を出力
        write(1, "-", 1);
    if (ifloat.exp == 2047)                      // infの場合
    {
        write(1, "inf", 3);
        return;
    }
    for (i = 0; i < FLT_INTSIZE; i++)           // 整数部の出力
    {
        if (flg == 0 && ifloat.intpart[i] == 0)
        {
            continue;
        }
        flg = 1;
        c = ifloat.intpart[i] + '0';
        write(1, &c, 1);
    }
    if (flg == 0)
    {
        write(1, "0", 1);
    }

    write(1, ".", 1);
    for (i = 0; i < FLT_FRACSIZE - 1; i++)      // 小数部の出力
    {
        c = ifloat.fracpart[i] + '0';
        write(1, &c, 1);
    }
}

void			convert_fracpart(struct s_ifloat* ifloat)
{
    int16_t      i;                  // カウンタ
    unsigned long long    fracpart_bin;       // fracの整数部分を格納
    int8_t      n[FLT_FRACSIZE];    // 2^(i-1)を格納

    ZeroMemory(ifloat->fracpart, sizeof(ifloat->fracpart));
    if (ifloat->exp >= 52 + 1023 || (ifloat->exp == 0 && ifloat->frac == 0)) // 小数部0の場合
        return;
    else if (ifloat->exp >= 1023)    // 一部のみ小数部の場合
        fracpart_bin = ifloat->frac << (ifloat->exp - 1023);
    else if (ifloat->exp == 0)      // 非正規数（ケチ表現分bitなし）
        fracpart_bin = ifloat->frac;
    else                            // 全て小数部かつ正規数（ケチ表現分bitを入れる）
        fracpart_bin = ifloat->frac >> 1 | (static_cast<unsigned long long>(1) << 63);
    ZeroMemory(n, sizeof(n));
    n[0] = 5;                       // 2^(-1)(=0.5)からスタート
    for (i = 0; i < (1022 - ifloat->exp); i++)
        array_divbytwo(n, FLT_FRACSIZE);    // あらかじめ指数に合わせて割っておく
    for (i = 0; i < 53; i++)
    {
        if (fracpart_bin & (static_cast<unsigned long long>(1) << (63 - i))) // bitが立っていればfracpartに足していく
            array_add(ifloat->fracpart, n, FLT_FRACSIZE);
        array_divbytwo(n, FLT_FRACSIZE);
    }
}
// 整数部の出力
void            convert_intpart(struct s_ifloat* ifloat)
{
    uint32_t     i;                  // カウンタ
    int         offset;             // fracの整数部分までのoffset
    unsigned long long    intpart_bin;        // fracの整数部分を格納
    int8_t      n[FLT_INTSIZE];     // 2^i乗を格納

    ZeroMemory(ifloat->intpart, sizeof(ifloat->intpart));
    if (ifloat->exp < 1023 || ifloat->exp == 2047) // 整数部は0
        return;
    else if (ifloat->exp < 1023 + 52) // ifloat->fracの一部が整数部
        offset = ifloat->exp - 1023;
    else // ifloat->exp >= 1023 + 52 => ifloat->fracの全てが整数部
        offset = 52;

    if (offset == 0)
        intpart_bin = static_cast<unsigned long long>(1) << offset;
    else
        intpart_bin = (ifloat->frac >> (64 - offset)) | (static_cast<unsigned long long>(1) << offset);
    ZeroMemory(n, sizeof(n));
    n[FLT_INTSIZE - 1] = 1;     // n=1からスタート(最も右を1の位とする)
    for (i = 0; i < 53; i++)    // 0~53ビットを確認する
    {
        if (intpart_bin & (static_cast<unsigned long long>(1) << i))     // bitが立っていれば2^n乗を足す
            array_add(ifloat->intpart, n, FLT_INTSIZE);
        array_double(n, FLT_INTSIZE);   // nを2倍して次へ
    }
    while (i++ <= ifloat->exp - 1023)    // iがexpより小さければその分2倍していく
        array_double(ifloat->intpart, FLT_INTSIZE);
}

void            convert_ifloat(struct s_ifloat* ifloat)
{
    convert_intpart(ifloat);        // 整数部の出力
    convert_fracpart(ifloat);       // 小数部の出力
}

void            printfloat(double num)
{
    struct s_ifloat ifloat;

    ifloat = store_ifloat(num);     // 符号部、指数部、仮数部を別々の変数に取り出す
    convert_ifloat(&ifloat);        // 2進数->10進数へ変換
    print_ifloat(ifloat);           // 標準出力へ出力
}

void            printcomp(double num)
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
    write(1, "\n", 1);
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


int             asdasdsad(void)
{
    float	    num;
    uint32_t    mem;

    printcomp1(1.2);
    printcomp1(4.2);
    printcomp1(0.0);
    printcomp1(2312.31231002312349999);
    printcomp1(1234567.123534534672247083456);
    printcomp1(FLT_MAX);     // 正規数の最大 = 3.40282347e+38F (float.hで定義)
    printcomp1(FLT_MIN);     // 正規数の最小= 1.17549435e-38F (float.hで定義)
    mem = 1;
    memcpy(&num, &mem, sizeof(uint32_t));
    printcomp1(num);         // 非正規数の最小 (仮数部の最下位bitのみ1)
    printcomp1(NAN);     // nan
    printcomp1(INFINITY);     // inf
    printcomp1(-INFINITY);    // -inf
    return (0);
}

void a(int d)
{
    printf("%c", d);
}



int             main(void)
{
    xdev_out(a);
    //asdasdsad();
    //aaaaaa64();
	//  2312.3123100231237004
	//  2312.31231002312370038
    xprintf("%.25f\n", 2312.31231002312349999);
    printf("%.25f\n", 2312.31231002312349999);
    printf("%.24f\n", 2312.31231002312349999);
    printf("%.23f\n", 2312.31231002312349999);
    printf("%.22f\n", 2312.31231002312349999);
    printf("%.21f\n", 2312.31231002312349999);
    printf("%.20f\n", 2312.31231002312349999);
    printf("%.19f\n", 2312.31231002312349999);
    printf("%.18f\n", 2312.31231002312349999);
    printf("%.17f\n", 2312.31231002312349999);
    printf("%.2f\n", 0.155000000000);
}