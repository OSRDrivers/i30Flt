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
#include "i30Flt.h"

///////////// 
// GLOBALS //
/////////////

I30FLT_GLOBALS i30FltGlobals;

DECLARE_CONST_UNICODE_STRING(i30Flti30PrefixPath, I30FLT_I30_PREFIX_STR);

#if DBG
ULONG i30FltTraceLevel = INFO;
#endif

//  
// This is the operation registration structure, it is where you
// indicate which I/O operations you're interested in (and whether you want to
// be called for before the file system (pre) and/or after the file system
// (post))
// 
CONST FLT_OPERATION_REGISTRATION i30FltOpRegistration[] = {

    //
    // Open the file gets the attributes, which corrupts
    //
    { IRP_MJ_CREATE,
      0,
      i30FltPre,
      NULL 
    },

    //
    // Query attributes by name gets the attributes, which corrupts
    //
    { IRP_MJ_QUERY_OPEN,
      0,
      i30FltPre,
      NULL 
    },

    //
    // Network query opens get the attributes, which corrupts
    //
    { IRP_MJ_NETWORK_QUERY_OPEN,
      0,
      i30FltPre,
      NULL 
    },

    { IRP_MJ_OPERATION_END }

};

// 
// Registration structure describing which types of context
// we're interested in maintaining
// 
FLT_CONTEXT_REGISTRATION i30FltContextRegistration[] = {

    { FLT_CONTEXT_END }
};

//
//  This defines what we want to filter with FltMgr
//
CONST FLT_REGISTRATION i30FltRegistration = {

    sizeof(FLT_REGISTRATION),   //  Size
    FLT_REGISTRATION_VERSION,   //  Version
    0,                          //  Flags

    i30FltContextRegistration,  //  Context
    i30FltOpRegistration,       //  Operation callbacks

    i30FltUnload,               //  FilterUnload

    i30FltInstanceSetup,        //  InstanceSetup
    i30FltQueryTeardown,        //  InstanceQueryTeardown
    NULL,                       //  InstanceTeardownStart
    NULL,                       //  InstanceTeardownComplete

    NULL,                       //  GenerateFileName
    NULL,                       //  GenerateDestinationFileName
    NULL                        //  NormalizeNameComponent

};

///////////////////////////////////////////////////////////////////////////////
//
//  DriverEntry
//
//    This routine is called by Windows when the driver is first loaded.  We
//    simply do our initialization, and start our filter
//
//  INPUTS:
//
//      DriverObject - Address of the DRIVER_OBJECT created by NT for this
//                     driver.
//  
//      RegistryPath - UNICODE_STRING which represents this drivers KEY in the
//                     Registry.
//
//  OUTPUTS:
//
//      None.
//
//  RETURNS:
//
//      An appropriate NTSTATUS value.
//
//  IRQL:
//
//    This routine is called at IRQL == PASSIVE_LEVEL.
//
//  NOTES:
//
//
///////////////////////////////////////////////////////////////////////////////
extern "C"
_Use_decl_annotations_
NTSTATUS
DriverEntry(
    PDRIVER_OBJECT DriverObject,
    PUNICODE_STRING RegistryPath)
{

    NTSTATUS status;

    UNREFERENCED_PARAMETER(RegistryPath);

    //
    // Initialize NX pool support
    //
    ExInitializeDriverRuntime(DrvRtPoolNxOptIn);

    //
    // Initialize tracing
    //
    EventRegisteri30Flt();

    //
    // Register with FltMgr
    //
    status = FltRegisterFilter(DriverObject,
                               &i30FltRegistration,
                               &i30FltGlobals.FilterHandle);

    if (!NT_SUCCESS(status)) {

        i30FltTracePrint(ERROR, "FltRegisterFilter failed! 0x%x\n",
                         status);

        goto Exit;

    }

    //
    // Start filtering i/o
    //
    status = FltStartFiltering(i30FltGlobals.FilterHandle);

    if (!NT_SUCCESS(status)) {

        i30FltTracePrint(ERROR, "FltStartFiltering failed! 0x%x\n",
                         status);

        goto Exit;

    }

Exit:

    if (!NT_SUCCESS(status)) {

        // 
        // Call our unload entry point, which will handle any cleanup
        // 
        i30FltUnload(0);

    }

    return status;

}

///////////////////////////////////////////////////////////////////////////////
//
//  i30FltInstanceSetup
// 
//      Called by the filter manager for each new instance of
//      the filter created
// 
//  INPUTS:
// 
//      FltObjects           - Filter manager objects for the operation
// 
//      Flags                - Filter manager flags for the operation
// 
//      VolumeDeviceType     - Volume type that we're mounting over
// 
//      VolumeFilesystemType - File system that we're instantiating over
// 
//  OUTPUTS:
// 
//      None.
// 
//  RETURNS:
//
//      STATUS_SUCCESS if successful
// 
//      STATUS_FLT_DO_NOT_ATTACH if it's not NTFS
// 
//      An appropriate error otherwise
//
//  IRQL:
//
//      This routine is called at IRQL == PASSIVE_LEVEL.
//
//  NOTES:
// 
//
///////////////////////////////////////////////////////////////////////////////
_Use_decl_annotations_
NTSTATUS
FLTAPI
i30FltInstanceSetup(
    PCFLT_RELATED_OBJECTS FltObjects,
    FLT_INSTANCE_SETUP_FLAGS Flags,
    DEVICE_TYPE VolumeDeviceType,
    FLT_FILESYSTEM_TYPE VolumeFilesystemType)
{

    NTSTATUS status;

    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);
    UNREFERENCED_PARAMETER(VolumeDeviceType);

    i30FltTraceFunctionEntry();

    //
    // Quickly ignore any file systems that we don't support
    //
    if (VolumeFilesystemType != FLT_FSTYPE_NTFS) {

        //
        // Don't trace attempts to attach to raw, it's just noise and it 
        // happens all the time
        //
        if (VolumeFilesystemType != FLT_FSTYPE_RAW) {

            i30FltTracePrint(INFO, "Unsupported file system 0x%x, ignoring\n",
                             VolumeFilesystemType);

        }

        status = STATUS_FLT_DO_NOT_ATTACH;

        goto Exit;

    }

    // 
    // Done!
    // 
    status = STATUS_SUCCESS;

Exit:

    i30FltTraceFunctionExit();

    return status;

}

///////////////////////////////////////////////////////////////////////////////
//
//  i30FltQueryTeardown
// 
//      Called by the filter manager when someone tries to
//      manually detach our instance
// 
//  INPUTS:
// 
//      FltObjects - Filter manager objects for the operation
// 
//      Flags      - Filter manager flags for the operation
// 
//  OUTPUTS:
// 
//      None.
// 
//  RETURNS:
//
//      STATUS_SUCCESS
//
//  IRQL:
//
//      This routine is called at IRQL == PASSIVE_LEVEL.
//
//  NOTES:
// 
//
///////////////////////////////////////////////////////////////////////////////
_Use_decl_annotations_
NTSTATUS
FLTAPI
i30FltQueryTeardown(
    PCFLT_RELATED_OBJECTS FltObjects,
    FLT_INSTANCE_QUERY_TEARDOWN_FLAGS Flags)
{

    UNREFERENCED_PARAMETER(FltObjects);
    UNREFERENCED_PARAMETER(Flags);

    i30FltTraceFunctionEntry();

    // 
    // We need to return success here if we want the user to be able to
    // dynamically detach from us...
    // 
    i30FltTracePrint(INFO, "Teardown starting for instance 0x%p\n",
                     FltObjects->Instance);

    i30FltTraceFunctionExit();

    return STATUS_SUCCESS;
}

///////////////////////////////////////////////////////////////////////////////
//
//  i30FltUnload
// 
//      Called by the filter manager when our filter is being
//      unloaded
// 
//  INPUTS:
// 
//      Flags - Filter manager flags for the operation
// 
//  OUTPUTS:
// 
//      None.
// 
//  RETURNS:
//
//      STATUS_SUCCESS
//
//  IRQL:
//
//      This routine is called at IRQL == PASSIVE_LEVEL.
//
//  NOTES:
// 
//
///////////////////////////////////////////////////////////////////////////////
_Use_decl_annotations_
NTSTATUS
FLTAPI
i30FltUnload(
    FLT_FILTER_UNLOAD_FLAGS Flags)
{

    UNREFERENCED_PARAMETER(Flags);

    i30FltTraceFunctionEntry();

    if (i30FltGlobals.FilterHandle != NULL) {

        FltUnregisterFilter(i30FltGlobals.FilterHandle);

    }

    EventUnregisteri30Flt();

    i30FltTraceFunctionExit();

    return STATUS_SUCCESS;
}


///////////////////////////////////////////////////////////////////////////////
//
//  i30FltPre
// 
//      Pre processing routine for any operation that might trigger the crash
// 
//  INPUTS:
//
//      Data              - Filter manager callback data for the operation
// 
//      FltObjects        - Filter manager objects for the operation
// 
//      CompletionContext - Unused
// 
//  OUTPUTS:
// 
//      None.
// 
//  RETURNS:
//
//      FLT_PREOP_SUCCESS_WITH_CALLBACK
//
//  IRQL:
//
//      This routine is called at IRQL == PASSIVE_LEVEL.
//
//  NOTES:
// 
//
///////////////////////////////////////////////////////////////////////////////
_Use_decl_annotations_
FLT_PREOP_CALLBACK_STATUS
FLTAPI
i30FltPre(
    PFLT_CALLBACK_DATA Data,
    PCFLT_RELATED_OBJECTS FltObjects,
    PVOID *CompletionContext)
{
    NTSTATUS                   status;
    FLT_PREOP_CALLBACK_STATUS  callbackStatus;
    PFLT_FILE_NAME_INFORMATION fileNameInfo = NULL;
    
    UNREFERENCED_PARAMETER(FltObjects);

    callbackStatus     = FLT_PREOP_SUCCESS_NO_CALLBACK;
    *CompletionContext = NULL;

    status = FltGetFileNameInformation(
                            Data,
                            FLT_FILE_NAME_OPENED |
                                FLT_FILE_NAME_QUERY_ALWAYS_ALLOW_CACHE_LOOKUP,
                            &fileNameInfo);

    if (!NT_SUCCESS(status)) {

        i30FltTracePrint(ERROR, "FltGetFileNameInformation failed! 0x%x\n",
                         status);

        goto Exit;

    }

    status = FltParseFileNameInformation(fileNameInfo);

    if (!NT_SUCCESS(status)) {

        i30FltTracePrint(ERROR, "FltParseFileNameInformation failed! 0x%x\n",
                         status);

        goto Exit;

    }

    //
    // Check to see if our evil prefix is present. Note that this is CASE
    // SENSITIVE (FALSE as Arg3)
    //
    if (!RtlPrefixUnicodeString(&i30Flti30PrefixPath,
                                &fileNameInfo->Stream,
                                FALSE)) {

        //
        // It's not the evil magic path...Don't trace anything (this is every
        // file opened or queried) just leave
        //
        goto Exit;

    }

    //
    // It's our evil path! Fail it...
    //
    i30FltTracePrint(ERROR, "Denying attempt to access %wZ\n",
                     &fileNameInfo->Name);

    Data->IoStatus.Status      = STATUS_ACCESS_DENIED;
    Data->IoStatus.Information = 0;
    callbackStatus             = FLT_PREOP_COMPLETE;

    //
    // Send an entry to the event log so there's a trace of this happening
    //
    EventWriteOperationBlocked(NULL,
                               Data->Iopb->MajorFunction,
                               fileNameInfo->Name.Length / 2,
                               fileNameInfo->Name.Buffer);

Exit:

    if (fileNameInfo != NULL) {

        FltReleaseFileNameInformation(fileNameInfo);

    }

    return callbackStatus;

}