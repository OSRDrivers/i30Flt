//
// Copyright 2021 OSR Open Systems Resources, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from this
//    software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE
// 
#pragma once

#include <fltKernel.h>
#include "OsrSuppress.h"
#include "i30FltEvents.h"

/////////////
// DEFINES //
/////////////

//
// The evil stream is:
// 
//     :$i30:$bitmap
//  
// Note that the problem only happens if the $I30 is specified with a lower 
// case 'I'. Thee case of $bitmap does not matter. We'll be conservative and 
// fail any attempt to open a stream that starts with "$i30"
//
#define I30FLT_I30_PREFIX_STR           L":$i30:"

////////////////////////// 
// DEBUG OUTPUT SUPPORT //
//////////////////////////
#define DRIVER_NAME "I30FLT!"

#if DBG
extern ULONG i30FltTraceLevel;

#define ERROR    1
#define INFO     2
#define VERBOSE  3
#define FUNCTION 4

#define i30FltTracePrint(__LEVEL__,...)                         \
{                                                               \
    if (i30FltTraceLevel >= __LEVEL__) {                        \
         DbgPrint(DRIVER_NAME __FUNCTION__ ": " __VA_ARGS__);   \
    }                                                           \
}
#else
#define i30FltTracePrint(__LEVEL__,...)
#endif

#define i30FltTraceFunctionEntry() \
    i30FltTracePrint(FUNCTION, "--> Entered.\n")
            
#define i30FltTraceFunctionExit() \
    i30FltTracePrint(FUNCTION, "<-- Exit.\n")

typedef struct _I30FLT_GLOBALS {

    //
    //  The filter handle that results from a call to
    //  FltRegisterFilter.
    //
    PFLT_FILTER    FilterHandle;

} I30FLT_GLOBALS, *PI30FLT_GLOBALS;

extern I30FLT_GLOBALS i30FltGlobals;

//////////////// 
// PROTOTYPES //
////////////////
 
//
// i30Flt.cpp
// 
extern "C" DRIVER_INITIALIZE DriverEntry;

NTSTATUS
FLTAPI
i30FltInstanceSetup(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_SETUP_FLAGS Flags,
    _In_ DEVICE_TYPE VolumeDeviceType,
    _In_ FLT_FILESYSTEM_TYPE VolumeFilesystemType
    );

NTSTATUS
FLTAPI
i30FltQueryTeardown(
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _In_ FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags
    );

NTSTATUS
FLTAPI
i30FltUnload(
    _In_ FLT_FILTER_UNLOAD_FLAGS Flags
    );

FLT_PREOP_CALLBACK_STATUS
FLTAPI
i30FltPre(
    _Inout_ PFLT_CALLBACK_DATA Data,
    _In_ PCFLT_RELATED_OBJECTS FltObjects,
    _Outptr_result_maybenull_ PVOID *CompletionContext
    );

