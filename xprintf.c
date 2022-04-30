#include "xprintf.h"
#include <math.h>
#include <stdarg.h>

void round_away(char* plast)
{
	while (*plast == '9' || *plast == '.')
	{
		if (*plast == '.')
		{
			plast--;
			continue;
		}
		*plast = '0';
		plast--;
	}
	if (*plast == 0)
	{
		*plast = '1';
	}
	else
	{
		*plast += 1;
	}
}

int ftoa(
	char* obuf,
	double val
)
{
	int d = 0, m = 0;
	double w = 0, fd = 0;
	char rnd = 0, last = 0;
	char stb[SZB_OUTPUT];
	char* buf = stb + 1;
	int sign = 0;

	memset(stb, 0, sizeof(stb));

	if (isnan(val) || isinf(val))
	{
		return 1;
	}
	if (val < 0)
	{
		val = -val;
		sign = 1;
	}
	m = log10(val);
	if (m + 17 + 3 + 2 >= SZB_OUTPUT)
	{
		return 2;
	}
	if (m < 0)
	{
		m = 0;
	}
	// 整数出力
	while ( 0 <= m)
	{
		w = pow(10, m);
		d = val / w;
		fd = d * w;
		val -= fd;
		*buf++ = '0' + d;
		m--;
	}

	// 小数出力
	*buf++ = '.';
	while (m >= -17)
	{
		val *= 10;
		d = val;
		val -= d;
		*buf++ = '0' + d;
		m--;
	}

	rnd = *(--buf);
	*buf = 0;
	last = *(--buf);

	// 小数16桁目奇数、下位桁が5以上又は、6以上の場合切り上げ
	if ((0 != (last - '0') % 2 && rnd == '5') ||
		'6' <= rnd)
	{
		round_away(buf);
	}

	if (0 != sign)
	{
		*obuf++ = '-';
	}
	buf = stb[0] == 0 ? &stb[1] : stb;
	int len = strlen(buf);
	memcpy(obuf, buf, len);

	return 0;
}


int f16toa(
	double val
)
{
	char b[SZB_OUTPUT];

	memset(b, 0, sizeof(b));

	int ret = ftoa(b, val);
	printf("@%s\n", b);

	return ret;
}


static int ilog10(double n)	/* Calculate log10(n) in integer output */
{
	int rv = 0;

	while (n >= 10) {	/* Decimate digit in right shift */
		if (n >= 100000) {
			n /= 100000; rv += 5;
		}
		else {
			n /= 10; rv++;
		}
	}
	while (n < 1) {		/* Decimate digit in left shift */
		if (n < 0.00001) {
			n *= 100000; rv -= 5;
		}
		else {
			n *= 10; rv--;
		}
	}
	return rv;
}


static double i10x(int n)	/* Calculate 10^n */
{
	double rv = 1;

	while (n > 0) {		/* Left shift */
		if (n >= 5) {
			rv *= 100000; n -= 5;
		}
		else {
			rv *= 10; n--;
		}
	}
	while (n < 0) {		/* Right shift */
		if (n <= -5) {
			rv /= 100000; n += 5;
		}
		else {
			rv /= 10; n++;
		}
	}
	return rv;
}


static void ftoa1(
	char* buf,	/* Buffer to output the generated string */
	double val,	/* Real number to output */
	int prec,	/* Number of fractinal digits */
	char fmt	/* Notation */
)
{
	int d;
	int e = 0, m = 0;
	char sign = 0;
	double w;
	const char* er = 0;


	if (isnan(val)) {			/* Not a number? */
		er = "NaN";
	}
	else {
		if (prec < 0) prec = 6;	/* Default precision (6 fractional digits) */
		if (val < 0) {			/* Nagative value? */
			val = -val; sign = '-';
		}
		else {
			sign = '+';
		}
		if (isinf(val)) {		/* Infinite? */
			er = "INF";
		}
		else {
			if (fmt == 'f') {	/* Decimal notation? */
				val += i10x(-prec) / 2;	/* Round (nearest) */
				m = ilog10(val);
				if (m < 0) m = 0;
				if (m + prec + 3 >= SZB_OUTPUT) er = "OV";	/* Buffer overflow? */
			}
			else {			/* E notation */
				if (val != 0) {		/* Not a true zero? */
					val += i10x(ilog10(val) - prec) / 2;	/* Round (nearest) */
					e = ilog10(val);
					if (e > 99 || prec + 6 >= SZB_OUTPUT) {	/* Buffer overflow or E > +99? */
						er = "OV";
					}
					else {
						if (e < -99) e = -99;
						val /= i10x(e);	/* Normalize */
					}
				}
			}
		}
		if (!er) {	/* Not error condition */
			if (sign == '-') *buf++ = sign;	/* Add a - if negative value */
			do {				/* Put decimal number */
				w = i10x(m);				/* Snip the highest digit d */
				d = val / w; val -= d * w;
				if (m == -1) *buf++ = '.';	/* Insert a decimal separarot if get into fractional part */
				*buf++ = '0' + d;			/* Put the digit */
			} while (--m >= -prec);			/* Output all digits specified by prec */
			if (fmt != 'f') {	/* Put exponent if needed */
				*buf++ = fmt;
				if (e < 0) {
					e = -e; *buf++ = '-';
				}
				else {
					*buf++ = '+';
				}
				*buf++ = '0' + e / 10;
				*buf++ = '0' + e % 10;
			}
		}
	}
	if (er) {	/* Error condition? */
		if (sign) *buf++ = sign;		/* Add sign if needed */
		do *buf++ = *er++; while (*er);	/* Put error symbol */
	}
	*buf = 0;	/* Term */
}

static char* strptr;		/* Pointer to the output memory (used by xsprintf) */

/*----------------------------------------------*/
/* Put a character                              */
/*----------------------------------------------*/

void xfputc(			/* Put a character to the specified device */
	void(*func)(int),	/* Pointer to the output function (null:strptr) */
	int chr				/* Character to be output */
)
{
	if (strptr) {
		*strptr++ = chr;	/* Write a character to the memory */
	}
}


/*----------------------------------------------*/
/* Formatted string output                      */
/*----------------------------------------------*/
/*  xprintf("%d", 1234);			"1234"
	xprintf("%6d,%3d%%", -200, 5);	"  -200,  5%"
	xprintf("%-6u", 100);			"100   "
	xprintf("%ld", 12345678);		"12345678"
	xprintf("%llu", 0x100000000);	"4294967296"	<XF_USE_LLI>
	xprintf("%lld", -1LL);			"-1"			<XF_USE_LLI>
	xprintf("%04x", 0xA3);			"00a3"
	xprintf("%08lX", 0x123ABC);		"00123ABC"
	xprintf("%016b", 0x550F);		"0101010100001111"
	xprintf("%*d", 6, 100);			"   100"
	xprintf("%s", "String");		"String"
	xprintf("%5s", "abc");			"  abc"
	xprintf("%-5s", "abc");			"abc  "
	xprintf("%-5s", "abcdefg");		"abcdefg"
	xprintf("%-5.5s", "abcdefg");	"abcde"
	xprintf("%-.5s", "abcdefg");	"abcde"
	xprintf("%-5.5s", "abc");		"abc  "
	xprintf("%c", 'a');				"a"
	xprintf("%12f", 10.0);			"   10.000000"	<XF_USE_FP>
	xprintf("%.4E", 123.45678);		"1.2346E+02"	<XF_USE_FP>
*/

static void xvfprintf(
	const char* fmt,	/* Pointer to the format string */
	va_list arp			/* Pointer to arguments */
)
{
	unsigned int r, i, j, w, f;
	int n, prec;
	char str[SZB_OUTPUT], c, d, * p, pad;
	long v;
	unsigned long uv;

	for (;;) {
		c = *fmt++;					/* Get a format character */
		if (!c) break;				/* End of format? */
		if (c != '%') {				/* Pass it through if not a % sequense */
			xfputc(0, c); continue;
		}
		f = w = 0;			 		/* Clear parms */
		pad = ' '; prec = -1;
		c = *fmt++;					/* Get first char of the sequense */
		if (c == '0') {				/* Flag: left '0' padded */
			pad = '0'; c = *fmt++;
		}
		else {
			if (c == '-') {			/* Flag: left justified */
				f = 2; c = *fmt++;
			}
		}
		if (c == '*') {				/* Minimum width from an argument */
			n = va_arg(arp, int);
			if (n < 0) {			/* Flag: left justified */
				n = 0 - n; f = 2;
			}
			w = n; c = *fmt++;
		}
		else {
			while (c >= '0' && c <= '9') {	/* Minimum width */
				w = w * 10 + c - '0';
				c = *fmt++;
			}
		}
		if (c == '.') {				/* Precision */
			c = *fmt++;
			if (c == '*') {				/* Precision from an argument */
				prec = va_arg(arp, int);
				c = *fmt++;
			}
			else {
				prec = 0;
				while (c >= '0' && c <= '9') {
					prec = prec * 10 + c - '0';
					c = *fmt++;
				}
			}
		}
		if (c == 'l') {		/* Prefix: Size is long */
			f |= 4; c = *fmt++;
		}
		if (!c) break;				/* End of format? */
		switch (c) {				/* Type is... */
		case 'b':					/* Unsigned binary */
			r = 2; break;
		case 'o':					/* Unsigned octal */
			r = 8; break;
		case 'd':					/* Signed decimal */
		case 'u':					/* Unsigned decimal */
			r = 10; break;
		case 'x':					/* Hexdecimal (lower case) */
		case 'X':					/* Hexdecimal (upper case) */
			r = 16; break;
		case 'c':					/* A character */
			xfputc(0, (char)va_arg(arp, int)); continue;
		case 's':					/* String */
			p = va_arg(arp, char*);		/* Get a pointer argument */
			if (!p) p = "";				/* Null ptr generates a null string */
			j = strlen(p);
			if (prec >= 0 && j > (unsigned int)prec) j = prec;	/* Limited length of string body */
			for (; !(f & 2) && j < w; j++) xfputc(0, pad);	/* Left pads */
			while (*p && prec--) xfputc(0, *p++);/* String body */
			while (j++ < w) xfputc(0, ' ');		/* Right pads */
			continue;
		case 'f':					/* Float (decimal) */
			ftoa1(p = str, va_arg(arp, double), prec, c);	/* Make fp string */
			for (j = strlen(p); !(f & 2) && j < w; j++) xfputc(0, pad);	/* Left pads */
			while (*p) xfputc(0, *p++);		/* Value */
			while (j++ < w) xfputc(0, ' ');	/* Right pads */
			continue;
		default:					/* Unknown type (passthrough) */
			xfputc(0, c); continue;
		}
	}
}

void xprintf(
	const char* fmt,
	...
)
{
	va_list arp;
	char buff[360];

	va_start(arp, fmt);
	strptr = buff;		/* Enable destination for memory */
	xvfprintf(fmt, arp);
	va_end(arp);
	*strptr = 0;		/* Terminate output string */
	strptr = 0;			/* Disable destination for memory */
	printf("%s\n", buff);
}
