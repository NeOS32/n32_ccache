// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

#ifndef SERVER_C_H
#define SERVER_C_H

#include <Windows.h>
#include <shellapi.h>
#include <n32_base.h>
#include <n32_states.h>

class win32_server_actions_c
{

protected:
	HANDLE 			m_hHandle;
//	N32_BOOL		m_SuccessfullyCreated;

public:
	win32_server_actions_c();
	virtual result_t	SERVER_WaitAndBlockForAConnection( void ) const= 0;
	virtual result_t	SERVER_ReadCommandFromAClient( void ) = 0;
	virtual result_t	SERVER_ServeCommandFromAClient( void ) = 0;
	virtual result_t	SERVER_SendResultsToAClient( void ) const= 0;
	virtual result_t	SERVER_Init( void ) = 0;
	virtual result_t	SERVER_Done( void ) = 0;
//		N32_BOOL	SERVER_IsSuccessfullyCreated( void ) const;
	
	virtual ~win32_server_actions_c();
};

class win32_server_c
{
private:
	win32_server_c& operator=(const win32_server_c& rhs);

protected:
	n32_state_class_t		m_eState;
	win32_server_actions_c &	m_rServerActions;

public:
	win32_server_c( win32_server_actions_c & i_rServerActions );
	virtual result_t	rInit( void );
	virtual result_t	rStart( void );
	virtual result_t	rStop( void );
	virtual result_t	rDone( void );
	virtual ~win32_server_c();
};

#endif // SERVER_C_H
