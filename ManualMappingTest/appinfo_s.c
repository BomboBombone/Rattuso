

/* this ALWAYS GENERATED file contains the RPC server stubs */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Tue Jan 19 04:14:07 2038
 */
/* Compiler settings for appinfo\appinfo.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0628 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#if !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_)


#if _MSC_VER >= 1200
#pragma warning(push)
#endif

#pragma warning( disable: 4211 )  /* redefine extern to static */
#pragma warning( disable: 4232 )  /* dllimport identity*/
#pragma warning( disable: 4024 )  /* array to pointer mapping*/
#pragma warning( disable: 4100 ) /* unreferenced arguments in x86 call */

#pragma optimize("", off ) 

#include <string.h>
#include "appinfo_h.h"

#define TYPE_FORMAT_STRING_SIZE   69                                
#define PROC_FORMAT_STRING_SIZE   101                               
#define EXPR_FORMAT_STRING_SIZE   1                                 
#define TRANSMIT_AS_TABLE_SIZE    0            
#define WIRE_MARSHAL_TABLE_SIZE   0            

typedef struct _appinfo_MIDL_TYPE_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ TYPE_FORMAT_STRING_SIZE ];
    } appinfo_MIDL_TYPE_FORMAT_STRING;

typedef struct _appinfo_MIDL_PROC_FORMAT_STRING
    {
    short          Pad;
    unsigned char  Format[ PROC_FORMAT_STRING_SIZE ];
    } appinfo_MIDL_PROC_FORMAT_STRING;

typedef struct _appinfo_MIDL_EXPR_FORMAT_STRING
    {
    long          Pad;
    unsigned char  Format[ EXPR_FORMAT_STRING_SIZE ];
    } appinfo_MIDL_EXPR_FORMAT_STRING;


static const RPC_SYNTAX_IDENTIFIER  _RpcTransferSyntax_2_0 = 
{{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}};

#if defined(_CONTROL_FLOW_GUARD_XFG)
#define XFG_TRAMPOLINES(ObjectType)\
NDR_SHAREABLE unsigned long ObjectType ## _UserSize_XFG(unsigned long * pFlags, unsigned long Offset, void * pObject)\
{\
return  ObjectType ## _UserSize(pFlags, Offset, (ObjectType *)pObject);\
}\
NDR_SHAREABLE unsigned char * ObjectType ## _UserMarshal_XFG(unsigned long * pFlags, unsigned char * pBuffer, void * pObject)\
{\
return ObjectType ## _UserMarshal(pFlags, pBuffer, (ObjectType *)pObject);\
}\
NDR_SHAREABLE unsigned char * ObjectType ## _UserUnmarshal_XFG(unsigned long * pFlags, unsigned char * pBuffer, void * pObject)\
{\
return ObjectType ## _UserUnmarshal(pFlags, pBuffer, (ObjectType *)pObject);\
}\
NDR_SHAREABLE void ObjectType ## _UserFree_XFG(unsigned long * pFlags, void * pObject)\
{\
ObjectType ## _UserFree(pFlags, (ObjectType *)pObject);\
}
#define XFG_TRAMPOLINES64(ObjectType)\
NDR_SHAREABLE unsigned long ObjectType ## _UserSize64_XFG(unsigned long * pFlags, unsigned long Offset, void * pObject)\
{\
return  ObjectType ## _UserSize64(pFlags, Offset, (ObjectType *)pObject);\
}\
NDR_SHAREABLE unsigned char * ObjectType ## _UserMarshal64_XFG(unsigned long * pFlags, unsigned char * pBuffer, void * pObject)\
{\
return ObjectType ## _UserMarshal64(pFlags, pBuffer, (ObjectType *)pObject);\
}\
NDR_SHAREABLE unsigned char * ObjectType ## _UserUnmarshal64_XFG(unsigned long * pFlags, unsigned char * pBuffer, void * pObject)\
{\
return ObjectType ## _UserUnmarshal64(pFlags, pBuffer, (ObjectType *)pObject);\
}\
NDR_SHAREABLE void ObjectType ## _UserFree64_XFG(unsigned long * pFlags, void * pObject)\
{\
ObjectType ## _UserFree64(pFlags, (ObjectType *)pObject);\
}
#define XFG_BIND_TRAMPOLINES(HandleType, ObjectType)\
static void* ObjectType ## _bind_XFG(HandleType pObject)\
{\
return ObjectType ## _bind((ObjectType) pObject);\
}\
static void ObjectType ## _unbind_XFG(HandleType pObject, handle_t ServerHandle)\
{\
ObjectType ## _unbind((ObjectType) pObject, ServerHandle);\
}
#define XFG_TRAMPOLINE_FPTR(Function) Function ## _XFG
#define XFG_TRAMPOLINE_FPTR_DEPENDENT_SYMBOL(Symbol) Symbol ## _XFG
#else
#define XFG_TRAMPOLINES(ObjectType)
#define XFG_TRAMPOLINES64(ObjectType)
#define XFG_BIND_TRAMPOLINES(HandleType, ObjectType)
#define XFG_TRAMPOLINE_FPTR(Function) Function
#define XFG_TRAMPOLINE_FPTR_DEPENDENT_SYMBOL(Symbol) Symbol
#endif

extern const appinfo_MIDL_TYPE_FORMAT_STRING appinfo__MIDL_TypeFormatString;
extern const appinfo_MIDL_PROC_FORMAT_STRING appinfo__MIDL_ProcFormatString;
extern const appinfo_MIDL_EXPR_FORMAT_STRING appinfo__MIDL_ExprFormatString;

/* Standard interface: LaunchAdminProcess, ver. 1.0,
   GUID={0x201ef99a,0x7fa0,0x444c,{0x93,0x99,0x19,0xba,0x84,0xf1,0x2a,0x1a}} */


extern const MIDL_SERVER_INFO LaunchAdminProcess_ServerInfo;

extern const RPC_DISPATCH_TABLE LaunchAdminProcess_v1_0_DispatchTable;

static const RPC_SERVER_INTERFACE LaunchAdminProcess___RpcServerInterface =
    {
    sizeof(RPC_SERVER_INTERFACE),
    {{0x201ef99a,0x7fa0,0x444c,{0x93,0x99,0x19,0xba,0x84,0xf1,0x2a,0x1a}},{1,0}},
    {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}},
    (RPC_DISPATCH_TABLE*)&LaunchAdminProcess_v1_0_DispatchTable,
    0,
    0,
    0,
    &LaunchAdminProcess_ServerInfo,
    0x04000000
    };
RPC_IF_HANDLE LaunchAdminProcess_v1_0_s_ifspec = (RPC_IF_HANDLE)& LaunchAdminProcess___RpcServerInterface;
#ifdef __cplusplus
namespace {
#endif

extern const MIDL_STUB_DESC LaunchAdminProcess_StubDesc;
#ifdef __cplusplus
}
#endif


#if !defined(__RPC_WIN32__)
#error  Invalid build platform for this stub.
#endif
#if !(TARGET_IS_NT60_OR_LATER)
#error You need Windows Vista or later to run this stub because it uses these features:
#error   forced complex structure or array, compiled for Windows Vista.
#error However, your C/C++ compilation flags indicate you intend to run this app on earlier systems.
#error This app will fail with the RPC_X_WRONG_STUB_VERSION error.
#endif


static const appinfo_MIDL_PROC_FORMAT_STRING appinfo__MIDL_ProcFormatString =
    {
        0,
        {

	/* Procedure RAiLaunchAdminProcess */

			0x0,		/* 0 */
			0x48,		/* Old Flags:  */
/*  2 */	NdrFcLong( 0x0 ),	/* 0 */
/*  6 */	NdrFcShort( 0x0 ),	/* 0 */
/*  8 */	NdrFcShort( 0x34 ),	/* x86 Stack size/offset = 52 */
/* 10 */	0x32,		/* FC_BIND_PRIMITIVE */
			0x0,		/* 0 */
/* 12 */	NdrFcShort( 0x0 ),	/* x86 Stack size/offset = 0 */
/* 14 */	NdrFcShort( 0x20 ),	/* 32 */
/* 16 */	NdrFcShort( 0x58 ),	/* 88 */
/* 18 */	0x46,		/* Oi2 Flags:  clt must size, has return, has ext, */
			0xc,		/* 12 */
/* 20 */	0x8,		/* 8 */
			0x1,		/* Ext Flags:  new corr desc, */
/* 22 */	NdrFcShort( 0x0 ),	/* 0 */
/* 24 */	NdrFcShort( 0x0 ),	/* 0 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */

	/* Parameter ExecutablePath */

/* 28 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 30 */	NdrFcShort( 0x4 ),	/* x86 Stack size/offset = 4 */
/* 32 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter CommandLine */

/* 34 */	NdrFcShort( 0xb ),	/* Flags:  must size, must free, in, */
/* 36 */	NdrFcShort( 0x8 ),	/* x86 Stack size/offset = 8 */
/* 38 */	NdrFcShort( 0x2 ),	/* Type Offset=2 */

	/* Parameter StartFlags */

/* 40 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 42 */	NdrFcShort( 0xc ),	/* x86 Stack size/offset = 12 */
/* 44 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter CreationFlags */

/* 46 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 48 */	NdrFcShort( 0x10 ),	/* x86 Stack size/offset = 16 */
/* 50 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter CurrentDirectory */

/* 52 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 54 */	NdrFcShort( 0x14 ),	/* x86 Stack size/offset = 20 */
/* 56 */	NdrFcShort( 0x8 ),	/* Type Offset=8 */

	/* Parameter WindowStation */

/* 58 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 60 */	NdrFcShort( 0x18 ),	/* x86 Stack size/offset = 24 */
/* 62 */	NdrFcShort( 0x8 ),	/* Type Offset=8 */

	/* Parameter StartupInfo */

/* 64 */	NdrFcShort( 0x10b ),	/* Flags:  must size, must free, in, simple ref, */
/* 66 */	NdrFcShort( 0x1c ),	/* x86 Stack size/offset = 28 */
/* 68 */	NdrFcShort( 0x16 ),	/* Type Offset=22 */

	/* Parameter hWnd */

/* 70 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 72 */	NdrFcShort( 0x20 ),	/* x86 Stack size/offset = 32 */
/* 74 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter Timeout */

/* 76 */	NdrFcShort( 0x48 ),	/* Flags:  in, base type, */
/* 78 */	NdrFcShort( 0x24 ),	/* x86 Stack size/offset = 36 */
/* 80 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Parameter ProcessInformation */

/* 82 */	NdrFcShort( 0x4112 ),	/* Flags:  must free, out, simple ref, srv alloc size=16 */
/* 84 */	NdrFcShort( 0x28 ),	/* x86 Stack size/offset = 40 */
/* 86 */	NdrFcShort( 0x36 ),	/* Type Offset=54 */

	/* Parameter ElevationType */

/* 88 */	NdrFcShort( 0x2150 ),	/* Flags:  out, base type, simple ref, srv alloc size=8 */
/* 90 */	NdrFcShort( 0x2c ),	/* x86 Stack size/offset = 44 */
/* 92 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

	/* Return value */

/* 94 */	NdrFcShort( 0x70 ),	/* Flags:  out, return, base type, */
/* 96 */	NdrFcShort( 0x30 ),	/* x86 Stack size/offset = 48 */
/* 98 */	0x8,		/* FC_LONG */
			0x0,		/* 0 */

			0x0
        }
    };

static const appinfo_MIDL_TYPE_FORMAT_STRING appinfo__MIDL_TypeFormatString =
    {
        0,
        {
			NdrFcShort( 0x0 ),	/* 0 */
/*  2 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/*  4 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/*  6 */	
			0x11, 0x8,	/* FC_RP [simple_pointer] */
/*  8 */	
			0x25,		/* FC_C_WSTRING */
			0x5c,		/* FC_PAD */
/* 10 */	
			0x11, 0x0,	/* FC_RP */
/* 12 */	NdrFcShort( 0xa ),	/* Offset= 10 (22) */
/* 14 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 16 */	NdrFcShort( 0x8 ),	/* 8 */
/* 18 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 20 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 22 */	0xb1,		/* FC_FORCED_BOGUS_STRUCT */
			0x3,		/* 3 */
/* 24 */	NdrFcShort( 0x30 ),	/* 48 */
/* 26 */	NdrFcShort( 0x0 ),	/* 0 */
/* 28 */	NdrFcShort( 0x12 ),	/* Offset= 18 (46) */
/* 30 */	0x36,		/* FC_POINTER */
			0x8,		/* FC_LONG */
/* 32 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 34 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 36 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 38 */	0x8,		/* FC_LONG */
			0x6,		/* FC_SHORT */
/* 40 */	0x3e,		/* FC_STRUCTPAD2 */
			0x4c,		/* FC_EMBEDDED_COMPLEX */
/* 42 */	0x0,		/* 0 */
			NdrFcShort( 0xffe3 ),	/* Offset= -29 (14) */
			0x5b,		/* FC_END */
/* 46 */	
			0x12, 0x8,	/* FC_UP [simple_pointer] */
/* 48 */	0x5,		/* FC_WCHAR */
			0x5c,		/* FC_PAD */
/* 50 */	
			0x11, 0x4,	/* FC_RP [alloced_on_stack] */
/* 52 */	NdrFcShort( 0x2 ),	/* Offset= 2 (54) */
/* 54 */	
			0x15,		/* FC_STRUCT */
			0x3,		/* 3 */
/* 56 */	NdrFcShort( 0x10 ),	/* 16 */
/* 58 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 60 */	0x8,		/* FC_LONG */
			0x8,		/* FC_LONG */
/* 62 */	0x5c,		/* FC_PAD */
			0x5b,		/* FC_END */
/* 64 */	
			0x11, 0xc,	/* FC_RP [alloced_on_stack] [simple_pointer] */
/* 66 */	0x8,		/* FC_LONG */
			0x5c,		/* FC_PAD */

			0x0
        }
    };

static const unsigned short LaunchAdminProcess_FormatStringOffsetTable[] =
    {
    0
    };


#ifdef __cplusplus
namespace {
#endif
static const MIDL_STUB_DESC LaunchAdminProcess_StubDesc = 
    {
    (void *)& LaunchAdminProcess___RpcServerInterface,
    MIDL_user_allocate,
    MIDL_user_free,
    0,
    0,
    0,
    0,
    0,
    appinfo__MIDL_TypeFormatString.Format,
    1, /* -error bounds_check flag */
    0x60001, /* Ndr library version */
    0,
    0x8010274, /* MIDL Version 8.1.628 */
    0,
    0,
    0,  /* notify & notify_flag routine table */
    0x1, /* MIDL flag */
    0, /* cs routines */
    0,   /* proxy/server info */
    0
    };
#ifdef __cplusplus
}
#endif

static const RPC_DISPATCH_FUNCTION LaunchAdminProcess_table[] =
    {
    NdrServerCall2,
    0
    };
static const RPC_DISPATCH_TABLE LaunchAdminProcess_v1_0_DispatchTable = 
    {
    1,
    (RPC_DISPATCH_FUNCTION*)LaunchAdminProcess_table
    };

static const SERVER_ROUTINE LaunchAdminProcess_ServerRoutineTable[] = 
    {
    (SERVER_ROUTINE)RAiLaunchAdminProcess
    };

static const MIDL_SERVER_INFO LaunchAdminProcess_ServerInfo = 
    {
    &LaunchAdminProcess_StubDesc,
    LaunchAdminProcess_ServerRoutineTable,
    appinfo__MIDL_ProcFormatString.Format,
    LaunchAdminProcess_FormatStringOffsetTable,
    0,
    0,
    0,
    0};
#pragma optimize("", on )
#if _MSC_VER >= 1200
#pragma warning(pop)
#endif


#endif /* !defined(_M_IA64) && !defined(_M_AMD64) && !defined(_ARM_) */

