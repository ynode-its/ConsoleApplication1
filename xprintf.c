#include "xprintf.h"
#include <math.h>

void round_away(char* plast, char last, char rnd)
{
	if ((0 == (last - '0') % 2 && rnd == '5') ||
		 '6' > rnd)
	{
		return;
	}

	while (*plast == '9')
	{
		*plast = '0';
		plast--;
	}
	if (*plast == '.')
	{
		plast--;
		while (*plast == '9')
		{
			*plast = '0';
			plast--;
		}
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
	// ®”o—Í
	while ( m > -1)
	{
		w = pow(10, m);
		d = val / w;
		fd = d * w;
		val -= fd;
		*buf++ = '0' + d;
		--m;
	}

	// ­”o—Í
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

	round_away(buf, last, rnd);


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
