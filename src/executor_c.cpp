// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

/*
 * C++ executor_c
 *
 * This class is in charge of execution of a processes within the context
 * of a new process.
 */

#include "executor_c.h"

#include <Windows.h>
#include <string>
#include <arch/windows/specific.h>


executor_c::executor_c()
{
}

executor_c::~executor_c()
{
}

result_t executor_c::ExecuteCommand(const string& i_Command, N32_BOOL i_bInheritHandles, N32_BOOL i_bWaitForReturn) const
{
	STARTUPINFO		si = { sizeof(si) };
	PROCESS_INFORMATION	pi;
	DWORD			dwExitCode = -1;
	result_t		rRet = N32_ERR_UNKNOWN;
	DWORD			dwCreationFlags = 0x0;


	memset(&si, 0x0, sizeof si);
	si.cb = sizeof(si);

	if (N32_TRUE == i_bInheritHandles)
	{
		si.dwFlags |= STARTF_USESTDHANDLES;

		// standard output
		HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
		if (unlikely((0x0 == handle) || (INVALID_HANDLE_VALUE == handle)))
		{
			DEB(DEB_WARN, "Err: 0x%x\n", rRet);
			return(rRet);
		}
		si.hStdOutput = handle;

		// standard error
		handle = GetStdHandle(STD_ERROR_HANDLE);
		if (unlikely((0x0 == handle) || (INVALID_HANDLE_VALUE == handle)))
		{
			DEB(DEB_WARN, "Err: 0x%x\n", rRet);
			return(rRet);
		}
		si.hStdError = handle;

		// standard input
		handle = GetStdHandle(STD_INPUT_HANDLE);
		if (unlikely((0x0 == handle) || (INVALID_HANDLE_VALUE == handle)))
		{
			DEB(DEB_WARN, "Err: 0x%x\n", rRet);
			return(rRet);
		}
		si.hStdInput = handle;
	}

	memset(&pi, 0x0, sizeof pi);

	if (N32_FALSE == i_bWaitForReturn)
	{
		//dwCreationFlags |= CREATE_NEW_PROCESS_GROUP;
		dwCreationFlags |= CREATE_NEW_PROCESS_GROUP | CREATE_DEFAULT_ERROR_MODE | DETACHED_PROCESS;
	}

	DEB(DEB_LOG, "Running: '%s'\n", i_Command.c_str());

	if (likely(CreateProcess(
		0,
		(char*)i_Command.c_str(),
		0,
		0,
		(N32_TRUE == i_bInheritHandles) ? TRUE : FALSE,
		dwCreationFlags,
		0,
		0,
		&si,
		&pi
	)))
	{
		if (N32_TRUE == i_bWaitForReturn)
		{
			// optionally wait for process to finish
			dwExitCode = WaitForSingleObject(pi.hProcess, INFINITE);
			if (unlikely(WAIT_OBJECT_0 != dwExitCode))
			{
				// error ...
				DEB(DEB_WARN, "Err: 0x%x (dwExitCode=%d)\n", rRet, dwExitCode);
			}
			else
			{
				// seems to be ok, but let's check system return code
				GetExitCodeProcess(pi.hProcess, &dwExitCode);
				if (likely(0x0 == dwExitCode))
				{
					rRet = N32_NO_ERROR;
				}
			}
		}
		else
		{
			rRet = N32_NO_ERROR;
		}

		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}
	else
	{
		Win32_ExtendedErrorCodeShow(TEXT("GetProcessId"));
	}

	return(rRet);
}

executor_c::executor_c(const executor_c& rhs)
{
}
