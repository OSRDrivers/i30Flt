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

///////////////////////////
//      W4 Warnings      //
///////////////////////////

//
// The following are C/C++ compiler warnings at W4 that we suppress globally
//

#pragma warning(disable: 4201) // don't use nameless struct/union


//////////////////////////////////////
//      Code Analysis Warnings      //
//////////////////////////////////////

//
// The following are Code Analysis warnings that we suppress globally. This
// allows us to use the "All Rules" ruleset and get everything possible while
// ignoring things that don't make sense for this solution
//

//
// GSL Warnings
//
#pragma warning(disable: 26429)      // Symbol can be marked not_null
#pragma warning(disable: 26438)      // Avoid goto
#pragma warning(disable: 26440)      // Function can be declared noexcept
#pragma warning(disable: 26446)      // Prefer gsl::at()
#pragma warning(disable: 26448)      // Consider using gsl::finally
#pragma warning(disable: 26476)      // Use variant instead of naked union
#pragma warning(disable: 26477)      // Use nullptr rather than NULL
#pragma warning(disable: 26481)      // Don't use pointer arithmetic
#pragma warning(disable: 26482)      // Only index using constant expressions
#pragma warning(disable: 26485)      // No array to pointer decay
#pragma warning(disable: 26486)      // Don't pass a pointer that may be invalid
#pragma warning(disable: 26487)      // Don't return a pointer that may be invalid
#pragma warning(disable: 26489)      // Don't deref a pointer that may be invalid
#pragma warning(disable: 26493)      // Don't use C-style casts
#pragma warning(disable: 26494)      // Always initialize an object

//
// Native Code Warnings 
//
// Note that we'll suppress any, "you're using executable memory" warnings that 
// pop up. This would be bad if we didn't also set POOL_NX_OPTIN_XXX
//
#if (!defined(POOL_NX_OPTIN) && !defined(POOL_NX_OPTIN_AUTO))
#error "Must opt in to NX pool!"
#endif

#pragma warning(disable:  6320)      // Don't use EXCEPTION_EXECUTE_HANDLER
#pragma warning(disable: 28159)      // Use error logging instead of KeBugCheckEx
#pragma warning(disable: 28160)      // Must succeed pool allocations are forbidden
#pragma warning(disable: 30030)      // Must use MdlMappingNoExecute
