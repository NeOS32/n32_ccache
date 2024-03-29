// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

/*
 * C++ win32_server_c
 *
 * This class represents high level commands of in-RAM server.
 *
 */

#include "win32_server_c.h"

#include <Windows.h>
#include <shellapi.h>
#include <n32_base.h>

win32_server_c::win32_server_c(win32_server_actions_c& i_rServerActions)
	:
	m_rServerActions(i_rServerActions),
	m_eState(CLASS_STATE_UKNOWN)
{
}

win32_server_c::~win32_server_c()
{
	m_eState = CLASS_STATE_UKNOWN;
}

result_t win32_server_c::rDone(void)
{
	m_eState = CLASS_STATE_CLOSED;

	return(N32_ERR_BAD_ALGORITHM);
}

result_t win32_server_c::rInit(void)
{
	/*if ( likely( N32_TRUE == m_rServerActions.SERVER_IsSuccessfullyCreated() ) )
	{
		m_eState= CLASS_STATE_INITIALIZED;

		return( N32_NO_ERROR );
	}
	else
	{
		return( N32_ERR_BAD_ALGORITHM );
	}*/
	m_eState = CLASS_STATE_INITIALIZED;
	return(N32_NO_ERROR);
}

result_t win32_server_c::rStart(void)
{
	m_eState = CLASS_STATE_RUNNING;

	result_t rRet;
	N32_BOOL bProcess = N32_TRUE;
	uAL32Bits uTries;

	while (N32_TRUE == bProcess)
	{
		if (unlikely(N32_NO_ERROR != (rRet = m_rServerActions.SERVER_Init())))
		{
			DEB(DEB_WARN, "Err: 0x%x\n", rRet);
			return(rRet);
		}

		if (unlikely(N32_NO_ERROR != (rRet = m_rServerActions.SERVER_WaitAndBlockForAConnection())))
		{
			DEB(DEB_WARN, "Err: 0x%x, ignoring waiting failure\n", rRet);
		}

		if (unlikely(N32_NO_ERROR != (rRet = m_rServerActions.SERVER_ReadCommandFromAClient())))
		{
			DEB(DEB_WARN, "Err: 0x%x, ignoring this request and waiting for another ... \n", rRet);
		}
		else
		{
			if (unlikely(N32_NO_ERROR != (rRet = m_rServerActions.SERVER_ServeCommandFromAClient())))
			{
				DEB(DEB_WARN, "Err: 0x%x\n", rRet);
			}
		}

		if (unlikely(N32_NO_ERROR != (rRet = m_rServerActions.SERVER_Done())))
		{
			DEB(DEB_WARN, "Err: 0x%x\n", rRet);
		}

		DEB(DEB_LOG, "Waiting for another request ... \n");
	}

	return(rRet);
}

result_t win32_server_c::rStop(void)
{
	m_eState = CLASS_STATE_PAUSED;

	return(N32_ERR_BAD_ALGORITHM);
}

win32_server_actions_c::win32_server_actions_c()
	:
	m_hHandle(INVALID_HANDLE_VALUE)
	//	m_SuccessfullyCreated(N32_FALSE)
{
}

win32_server_actions_c::~win32_server_actions_c()
{
	if (INVALID_HANDLE_VALUE != m_hHandle)
	{
		CloseHandle(m_hHandle);
	}
}
