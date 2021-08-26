#ifndef FREERDP_DRDYNVC_DEF_H
#define FREERDP_DRDYNVC_DEF_H

extern UINT dvcman_write_channel(IWTSVirtualChannel* pChannel, ULONG cbSize, const BYTE* pBuffer,
                                 void* pReserved);

extern void dvcman_test();

#endif /* FREERDP_DRDYNVC_DEF_H */