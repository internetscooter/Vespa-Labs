//  This is an extra header file for dynamic linking of
//  the LabJackUD driver.
//
//  support@labjack.com
//  Sep 18, 2007

#ifdef  __cplusplus
extern "C"
{
#endif

//For dynamic linking, we first define structures that have the same format
//as the desired function prototype.
typedef LJ_ERROR (__stdcall *tListAll) (long, long, long *, long *, long *, double *);
typedef LJ_ERROR (__stdcall *tOpenLabJack) (long, long, const char *, long, LJ_HANDLE *);
typedef LJ_ERROR (__stdcall *tAddRequest) (LJ_HANDLE, long, long, double, long, double);
typedef LJ_ERROR (__stdcall *tGo) (void);
typedef LJ_ERROR (__stdcall *tGoOne) (LJ_HANDLE);
typedef LJ_ERROR (__stdcall *teGet) (LJ_HANDLE, long, long, double *, long);
typedef LJ_ERROR (__stdcall *tePut) (LJ_HANDLE, long, long, double, long);
typedef LJ_ERROR (__stdcall *tGetResult) (LJ_HANDLE, long, long, double *);
typedef LJ_ERROR (__stdcall *tGetFirstResult) (LJ_HANDLE, long *, long *, double *, long *, double *);
typedef LJ_ERROR (__stdcall *tGetNextResult) (LJ_HANDLE, long *, long *, double *, long *, double *);
typedef LJ_ERROR (__stdcall *teAIN) (LJ_HANDLE, long, long, double *, long, long, long, long,  long, long);
typedef LJ_ERROR (__stdcall *teDAC) (LJ_HANDLE, long, double, long, long, long);
typedef LJ_ERROR (__stdcall *teDI) (LJ_HANDLE, long, long *);
typedef LJ_ERROR (__stdcall *teDO) (LJ_HANDLE, long, long);
typedef LJ_ERROR (__stdcall *teAddGoGet) (LJ_HANDLE, long, long *, long *, double *, long *, long *, long *, long *);
typedef LJ_ERROR (__stdcall *teTCConfig) (LJ_HANDLE, long *, long *, long, long, long, long *, double *, long, long);
typedef LJ_ERROR (__stdcall *teTCValues) (LJ_HANDLE, long *, long *, long *, long *, double *, double *, long, long);
typedef LJ_ERROR (__stdcall *tResetLabJack) (LJ_HANDLE);
typedef LJ_ERROR (__stdcall *tDoubleToStringAddress) (double, char *, long);
typedef LJ_ERROR (__stdcall *tStringToDoubleAddress) (const char *, double *, long);
typedef long (__stdcall *tStringToConstant) (const char *);
typedef void (__stdcall *tErrorToString) (LJ_ERROR, char *);
typedef double (__stdcall *tGetDriverVersion) (void);
typedef LJ_ERROR (__stdcall *tTCVoltsToTemp) (long, double, double, double *);

#ifdef  __cplusplus
} // extern C
#endif


