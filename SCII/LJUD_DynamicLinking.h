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
typedef LJ_ERROR (CALLBACK *tListAll) (long, long, long *, long *, long *, double *);
typedef LJ_ERROR (CALLBACK *tOpenLabJack) (long, long, const char *, long, LJ_HANDLE *);
typedef LJ_ERROR (CALLBACK *tAddRequest) (LJ_HANDLE, long, long, double, long, double);
typedef LJ_ERROR (CALLBACK *tGo) (void);
typedef LJ_ERROR (CALLBACK *tGoOne) (LJ_HANDLE);
typedef LJ_ERROR (CALLBACK *teGet) (LJ_HANDLE, long, long, double *, long);
typedef LJ_ERROR (CALLBACK *tePut) (LJ_HANDLE, long, long, double, long);
typedef LJ_ERROR (CALLBACK *tGetResult) (LJ_HANDLE, long, long, double *);
typedef LJ_ERROR (CALLBACK *tGetFirstResult) (LJ_HANDLE, long *, long *, double *, long *, double *);
typedef LJ_ERROR (CALLBACK *tGetNextResult) (LJ_HANDLE, long *, long *, double *, long *, double *);
typedef LJ_ERROR (CALLBACK *teAIN) (LJ_HANDLE, long, long, double *, long, long, long, long,  long, long);
typedef LJ_ERROR (CALLBACK *teDAC) (LJ_HANDLE, long, double, long, long, long);
typedef LJ_ERROR (CALLBACK *teDI) (LJ_HANDLE, long, long *);
typedef LJ_ERROR (CALLBACK *teDO) (LJ_HANDLE, long, long);
typedef LJ_ERROR (CALLBACK *teAddGoGet) (LJ_HANDLE, long, long *, long *, double *, long *, long *, long *, long *);
typedef LJ_ERROR (CALLBACK *teTCConfig) (LJ_HANDLE, long *, long *, long, long, long, long *, double *, long, long);
typedef LJ_ERROR (CALLBACK *teTCValues) (LJ_HANDLE, long *, long *, long *, long *, double *, double *, long, long);
typedef LJ_ERROR (CALLBACK *tResetLabJack) (LJ_HANDLE); 
typedef LJ_ERROR (CALLBACK *tDoubleToStringAddress) (double, char *, long);
typedef LJ_ERROR (CALLBACK *tStringToDoubleAddress) (const char *, double *, long);
typedef long (CALLBACK *tStringToConstant) (const char *);
typedef void (CALLBACK *tErrorToString) (LJ_ERROR, char *);
typedef double (CALLBACK *tGetDriverVersion) (void);
typedef LJ_ERROR (CALLBACK *tTCVoltsToTemp) (long, double, double, double *);



////Define a variable to hold a handle to the loaded DLL.
//HINSTANCE hDLLInstance;
 
////Define variables for the UD functions.
//tListAll m_pListAll;
//tOpenLabJack m_pOpenLabJack;
//tAddRequest m_pAddRequest;
//tGo m_pGo;
//tGoOne m_pGoOne;
//teGet m_peGet;
//tePut m_pePut;
//tGetResult m_pGetResult;
//tGetFirstResult m_pGetFirstResult;
//tGetNextResult m_pGetNextResult;
//teAIN m_peAIN;
//teDAC m_peDAC;
//teDI m_peDI;
//teDO m_peDO;
//teAddGoGet m_peAddGoGet;
//teTCConfig m_peTCConfig;
//teTCValues m_peTCValues;
//tResetLabJack m_pResetLabJack;
//tDoubleToStringAddress m_pDoubleToStringAddress;
//tStringToDoubleAddress m_pStringToDoubleAddress;
//tStringToConstant m_pStringToConstant;
//tErrorToString m_pErrorToString;
//tGetDriverVersion m_pGetDriverVersion;
//tTCVoltsToTemp m_pTCVoltsToTemp;


#ifdef  __cplusplus
} // extern C
#endif


