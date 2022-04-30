#ifndef XPRINTF_DEF
#define XPRINTF_DEF

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define SZB_OUTPUT	350

	int f16toa(double d);
	void xprintf(const char* fmt, ...);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // XPRINTF_DEF
