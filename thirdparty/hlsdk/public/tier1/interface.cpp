//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//===========================================================================//
#if defined( _WIN32 ) && !defined( _X360 )
#include <windows.h>
#endif

#if !defined( DONT_PROTECT_FILEIO_FUNCTIONS )
#define DONT_PROTECT_FILEIO_FUNCTIONS // for protected_things.h
#endif

#if defined( PROTECTED_THINGS_ENABLE )
#undef PROTECTED_THINGS_ENABLE // from protected_things.h
#endif

#include <stdio.h>
#include "interface.h"
#include <string.h>
#include <stdlib.h>
#ifdef _WIN32
#include <direct.h> // getcwd
#elif POSIX
#include <dlfcn.h>
#include <unistd.h>
#define _getcwd getcwd
#endif

// ------------------------------------------------------------------------------------ //
// InterfaceReg.
// ------------------------------------------------------------------------------------ //
InterfaceReg *InterfaceReg::s_pInterfaceRegs = NULL;

InterfaceReg::InterfaceReg( InstantiateInterfaceFn fn, const char *pName ) :
	m_pName(pName)
{
	m_CreateFn = fn;
	m_pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}

// ------------------------------------------------------------------------------------ //
// CreateInterface.
// This is the primary exported function by a dll, referenced by name via dynamic binding
// that exposes an opqaue function pointer to the interface.
//
// We have the Internal variant so Sys_GetFactoryThis() returns the correct internal 
// symbol under GCC/Linux/Mac as CreateInterface is DLL_EXPORT so its global so the loaders
// on those OS's pick exactly 1 of the CreateInterface symbols to be the one that is process wide and 
// all Sys_GetFactoryThis() calls find that one, which doesn't work. Using the internal walkthrough here
// makes sure Sys_GetFactoryThis() has the dll specific symbol and GetProcAddress() returns the module specific
// function for CreateInterface again getting the dll specific symbol we need.
// ------------------------------------------------------------------------------------ //
void* CreateInterfaceInternal( const char *pName, int *pReturnCode )
{
	InterfaceReg *pCur;
	
	for (pCur=InterfaceReg::s_pInterfaceRegs; pCur; pCur=pCur->m_pNext)
	{
		if (strcmp(pCur->m_pName, pName) == 0)
		{
			if (pReturnCode)
			{
				*pReturnCode = IFACE_OK;
			}
			return pCur->m_CreateFn();
		}
	}
	
	if (pReturnCode)
	{
		*pReturnCode = IFACE_FAILED;
	}
	return NULL;	
}

void* CreateInterface( const char *pName, int *pReturnCode )
{
    return CreateInterfaceInternal( pName, pReturnCode );
}



#ifdef POSIX
// Linux doesn't have this function so this emulates its functionality
void *GetModuleHandle(const char *name)
{
	void *handle;

	if( name == NULL )
	{
		// hmm, how can this be handled under linux....
		// is it even needed?
		return NULL;
	}

    if( (handle=dlopen(name, RTLD_NOW))==NULL)
    {
            printf("DLOPEN Error:%s\n",dlerror());
            // couldn't open this file
            return NULL;
    }

	// read "man dlopen" for details
	// in short dlopen() inc a ref count
	// so dec the ref count by performing the close
	dlclose(handle);
	return handle;
}
#endif

#if defined( _WIN32 ) && !defined( _X360 )
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

//-----------------------------------------------------------------------------
// Purpose: returns a pointer to a function, given a module
// Input  : pModuleName - module name
//			*pName - proc name
//-----------------------------------------------------------------------------
static void *Sys_GetProcAddress( const char *pModuleName, const char *pName )
{
	HMODULE hModule = (HMODULE)GetModuleHandle( pModuleName );
#ifdef WIN32
	return (void *)GetProcAddress( hModule, pName );
#else
	return (void *)dlsym( (void *)hModule, pName );
#endif
}

#if !defined(LINUX)
static void *Sys_GetProcAddress( HMODULE hModule, const char *pName )
{
#ifdef WIN32
	return (void *)GetProcAddress( hModule, pName );
#else
	return (void *)dlsym( (void *)hModule, pName );
#endif
}
#endif

//-----------------------------------------------------------------------------
// Purpose: Unloads a DLL/component from
// Input  : *pModuleName - filename of the component
// Output : opaque handle to the module (hides system dependency)
//-----------------------------------------------------------------------------
void Sys_UnloadModule( CSysModule *pModule )
{
	if ( !pModule )
		return;

	HMODULE	hDLL = reinterpret_cast<HMODULE>(pModule);

#ifdef _WIN32
	FreeLibrary( hDLL );
#elif defined(POSIX)
	dlclose((void *)hDLL);
#endif
}

//-----------------------------------------------------------------------------
// Purpose: returns a pointer to a function, given a module
// Input  : module - windows HMODULE from Sys_LoadModule() 
//			*pName - proc name
// Output : factory for this module
//-----------------------------------------------------------------------------
CreateInterfaceFn Sys_GetFactory( CSysModule *pModule )
{
	if ( !pModule )
		return NULL;

	HMODULE	hDLL = reinterpret_cast<HMODULE>(pModule);
#ifdef _WIN32
	return reinterpret_cast<CreateInterfaceFn>(GetProcAddress( hDLL, CREATEINTERFACE_PROCNAME ));
#elif defined(POSIX)
	// Linux gives this error:
	//../public/interface.cpp: In function `IBaseInterface *(*Sys_GetFactory
	//(CSysModule *)) (const char *, int *)':
	//../public/interface.cpp:154: ISO C++ forbids casting between
	//pointer-to-function and pointer-to-object
	//
	// so lets get around it :)
	return (CreateInterfaceFn)(GetProcAddress( (void *)hDLL, CREATEINTERFACE_PROCNAME ));
#endif
}

//-----------------------------------------------------------------------------
// Purpose: returns the instance of this module
// Output : interface_instance_t
//-----------------------------------------------------------------------------
CreateInterfaceFn Sys_GetFactoryThis( void )
{
	return &CreateInterfaceInternal;
}

//-----------------------------------------------------------------------------
// Purpose: returns the instance of the named module
// Input  : *pModuleName - name of the module
// Output : interface_instance_t - instance of that module
//-----------------------------------------------------------------------------
CreateInterfaceFn Sys_GetFactory( const char *pModuleName )
{
#ifdef _WIN32
	return static_cast<CreateInterfaceFn>( Sys_GetProcAddress( pModuleName, CREATEINTERFACE_PROCNAME ) );
#elif defined(POSIX)
	// see Sys_GetFactory( CSysModule *pModule ) for an explanation
	return (CreateInterfaceFn)( Sys_GetProcAddress( pModuleName, CREATEINTERFACE_PROCNAME ) );
#endif
}

#if defined( STAGING_ONLY ) && defined( _WIN32 )

typedef USHORT( WINAPI RtlCaptureStackBackTrace_FUNC )(
	ULONG frames_to_skip,
	ULONG frames_to_capture,
	PVOID *backtrace,
	PULONG backtrace_hash );

extern "C" int backtrace( void **buffer, int size )
{
	HMODULE hNTDll = GetModuleHandleA( "ntdll.dll" );
	static RtlCaptureStackBackTrace_FUNC * const pfnRtlCaptureStackBackTrace =
		( RtlCaptureStackBackTrace_FUNC * )GetProcAddress( hNTDll, "RtlCaptureStackBackTrace" );

	if ( !pfnRtlCaptureStackBackTrace )
		return 0;

	return (int)pfnRtlCaptureStackBackTrace( 2, size, buffer, 0 );
}

#endif // STAGING_ONLY && _WIN32

