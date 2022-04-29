
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
#include <float.h>
#include <stdint.h>
#include <io.h>
#include<Windows.h>

#define FLT_FRACSIZE 150	// 10進数での小数部の最大桁数149 + 計算用余裕1
#define FLT_INTSIZE 39		// 10進数での最大桁数 39

struct          s_ifloat
{
    uint8_t     sign;       // 符号1bitを収納
    uint8_t     exp;        // 指数部8bitを収納
    uint32_t    frac;       // 仮数部23bitを収納
    int8_t      intpart[FLT_INTSIZE];   // 10進数での整数部を保存
    int8_t      fracpart[FLT_FRACSIZE]; // 10進数での小数部を保存
};

static void            array_add(int8_t *a, int8_t *b, int size)
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

static void            array_divbytwo(int8_t *n, int size)
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

static void            array_double(int8_t *n, int size)
{
    int		i;

    i = size - 1;
    while(i >= 0)
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

static struct s_ifloat	store_ifloat(float num)
{
    struct s_ifloat	ifloat;
    uint32_t        mem;

    memcpy(&mem, &num, sizeof(uint32_t));
    ifloat.sign = mem >> 31;
    ifloat.exp = mem >> 23;
    ifloat.frac = mem << 9;
    return (ifloat);
}

static void			print_ifloat(struct s_ifloat ifloat)
{
    int     i;
    char    c;

    if (ifloat.exp == 255 && ifloat.frac != 0)  // nanの場合
    {
        write(1, "nan", 3);
        return ;
    }
    if (ifloat.sign == 1)                       // 符号ビットが1の場合'-'を出力
        write(1, "-", 1);
    if (ifloat.exp == 255)                      // infの場合
    {
        write(1, "inf", 3);
        return ;
    }
    for (i = 0; i < FLT_INTSIZE; i++)           // 整数部の出力
    {
        c = ifloat.intpart[i] + '0';
        write(1, &c, 1);
    }
    write(1, ".", 1);
    for (i = 0; i < FLT_FRACSIZE - 1; i++)      // 小数部の出力
    {
        c = ifloat.fracpart[i] + '0';
        write(1, &c, 1);
    }
}

static void			convert_fracpart(struct s_ifloat *ifloat)
{
    int8_t      i;                  // カウンタ
    uint32_t    fracpart_bin;       // fracの整数部分を格納
    int8_t      n[FLT_FRACSIZE];    // 2^(i-1)を格納

    ZeroMemory(ifloat->fracpart, sizeof(ifloat->fracpart));
    if (ifloat->exp >= 23 + 127 || (ifloat->exp == 0 && ifloat->frac == 0)) // 小数部0の場合
        return ;
    else if (ifloat->exp >= 127)    // 一部のみ小数部の場合
        fracpart_bin = ifloat->frac << (ifloat->exp - 127);
    else if (ifloat->exp == 0)      // 非正規数（ケチ表現分bitなし）
        fracpart_bin = ifloat->frac;
    else                            // 全て小数部かつ正規数（ケチ表現分bitを入れる）
        fracpart_bin = ifloat->frac >> 1 | (1 << 31);
    ZeroMemory(n, sizeof(n));
    n[0] = 5;                       // 2^(-1)(=0.5)からスタート
    for (i = 0; i < (126 - ifloat->exp); i++)
        array_divbytwo(n, FLT_FRACSIZE);    // あらかじめ指数に合わせて割っておく
    for (i = 0; i < 24; i++)
    {
        if (fracpart_bin & (1 << (31 - i))) // bitが立っていればfracpartに足していく
            array_add(ifloat->fracpart, n, FLT_FRACSIZE);
        array_divbytwo(n, FLT_FRACSIZE);
    }
}

static void            convert_intpart(struct s_ifloat *ifloat)
{
    uint8_t     i;                  // カウンタ
    int         offset;             // fracの整数部分までのoffset
    uint32_t    intpart_bin;        // fracの整数部分を格納
    int8_t      n[FLT_INTSIZE];     // 2^i乗を格納

    ZeroMemory(ifloat->intpart, sizeof(ifloat->intpart));
    if (ifloat->exp < 127 || ifloat->exp == 255) // 整数部は0
        return ;
    else if (ifloat->exp < 127 + 23) // ifloat->fracの一部が整数部
        offset = ifloat->exp - 127;
    else // ifloat->exp >= 127 + 23 => ifloat->fracの全てが整数部
        offset = 23;
    if (offset == 0)
        intpart_bin = 1 << offset;
    else
        intpart_bin = (ifloat->frac >> (32 - offset)) | (1 << offset);
    ZeroMemory(n, sizeof(n));
    n[FLT_INTSIZE - 1] = 1;     // n=1からスタート(最も右を1の位とする)
    for (i = 0; i < 24; i++)    // 0~24ビットを確認する
    {
        if (intpart_bin & (1 << i))     // bitが立っていれば2^n乗を足す
            array_add(ifloat->intpart, n, FLT_INTSIZE);
        array_double(n, FLT_INTSIZE);   // nを2倍して次へ
    }
    while (i++ <= ifloat->exp - 127)    // iがexpより小さければその分2倍していく
        array_double(ifloat->intpart, FLT_INTSIZE);
}

static void            convert_ifloat(struct s_ifloat *ifloat)
{
    convert_intpart(ifloat);        // 整数部の出力
    convert_fracpart(ifloat);       // 小数部の出力
}

static void            printfloat(float num)
{
    struct s_ifloat ifloat;

    ifloat = store_ifloat(num);     // 符号部、指数部、仮数部を別々の変数に取り出す
    convert_ifloat(&ifloat);        // 2進数->10進数へ変換
    print_ifloat(ifloat);           // 標準出力へ出力
}

void            printcomp1(float num)
{
    // printfの表示（nan, inf以外は整数部39ケタ、小数部149ケタ表示する）
    if (isnan(num) || isinf(num))
        printf("printf  : %f\n", num);
    else
        printf("printf  : %0*.*f\n", FLT_INTSIZE + FLT_FRACSIZE, FLT_FRACSIZE - 1, num);
    fflush(stdout);
    // 自作printfloatの表示（nan, inf以外は整数部39ケタ、小数部149ケタ表示する）
    write(1, "putfloat: ", 10);
    printfloat(num);
    write(1, "\n", 1);
}
