

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.00.0603 */
/* at Sat Dec 28 15:50:55 2019
 */
/* Compiler settings for test.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 8.00.0603 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __test_h__
#define __test_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __Iswobj_FWD_DEFINED__
#define __Iswobj_FWD_DEFINED__
typedef interface Iswobj Iswobj;

#endif 	/* __Iswobj_FWD_DEFINED__ */


#ifndef __IPart_FWD_DEFINED__
#define __IPart_FWD_DEFINED__
typedef interface IPart IPart;

#endif 	/* __IPart_FWD_DEFINED__ */


#ifndef __swobj_FWD_DEFINED__
#define __swobj_FWD_DEFINED__

#ifdef __cplusplus
typedef class swobj swobj;
#else
typedef struct swobj swobj;
#endif /* __cplusplus */

#endif 	/* __swobj_FWD_DEFINED__ */


#ifndef __Part_FWD_DEFINED__
#define __Part_FWD_DEFINED__

#ifdef __cplusplus
typedef class Part Part;
#else
typedef struct Part Part;
#endif /* __cplusplus */

#endif 	/* __Part_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __Iswobj_INTERFACE_DEFINED__
#define __Iswobj_INTERFACE_DEFINED__

/* interface Iswobj */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_Iswobj;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AD4A3352-79C5-495B-82AA-CABC7A971099")
    Iswobj : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE StartNotepad( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ToolbarUpdate( 
            /* [retval][out] */ long *status) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Traverse( void) = 0;
        
    };
    
    
#else 	/* C style interface */

    typedef struct IswobjVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            Iswobj * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            Iswobj * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            Iswobj * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            Iswobj * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            Iswobj * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            Iswobj * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            Iswobj * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *StartNotepad )( 
            Iswobj * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ToolbarUpdate )( 
            Iswobj * This,
            /* [retval][out] */ long *status);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Traverse )( 
            Iswobj * This);
        
        END_INTERFACE
    } IswobjVtbl;

    interface Iswobj
    {
        CONST_VTBL struct IswobjVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Iswobj_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define Iswobj_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define Iswobj_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define Iswobj_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define Iswobj_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define Iswobj_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define Iswobj_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define Iswobj_StartNotepad(This)	\
    ( (This)->lpVtbl -> StartNotepad(This) ) 

#define Iswobj_ToolbarUpdate(This,status)	\
    ( (This)->lpVtbl -> ToolbarUpdate(This,status) ) 

#define Iswobj_Traverse(This)	\
    ( (This)->lpVtbl -> Traverse(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __Iswobj_INTERFACE_DEFINED__ */


#ifndef __IPart_INTERFACE_DEFINED__
#define __IPart_INTERFACE_DEFINED__

/* interface IPart */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IPart;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8608542E-9790-4ACD-AA25-F42A445B77B4")
    IPart : public IDispatch
    {
    public:
    };
    
    
#else 	/* C style interface */

    typedef struct IPartVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPart * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPart * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPart * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IPart * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IPart * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IPart * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IPart * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        END_INTERFACE
    } IPartVtbl;

    interface IPart
    {
        CONST_VTBL struct IPartVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPart_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IPart_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IPart_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IPart_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IPart_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IPart_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IPart_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IPart_INTERFACE_DEFINED__ */



#ifndef __TESTLib_LIBRARY_DEFINED__
#define __TESTLib_LIBRARY_DEFINED__

/* library TESTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_TESTLib;

EXTERN_C const CLSID CLSID_swobj;

#ifdef __cplusplus

class DECLSPEC_UUID("0B35D8BA-EC78-4C5F-B047-36AFEF144E0D")
swobj;
#endif

EXTERN_C const CLSID CLSID_Part;

#ifdef __cplusplus

class DECLSPEC_UUID("F44B312D-CE40-452A-B418-103255478D7D")
Part;
#endif
#endif /* __TESTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


