/*
 * Copyright (C) 2011-2016 Sebastian Serewa
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * C++ win32_pipe_server_actions_c
 *
 * This class represent piped server on Windows.
 *
 */

#include "win32_pipe_server_actions_c.h"

#include <windows.h>

/* From MSDN:
 *
 * To free resources used by a named pipe, the application should always close handles
 * when they are no longer needed, which is accomplished either by calling the CloseHandle
 * function or when the process associated with the instance handles ends. Note that an instance
 * of a named pipe may have more than one handle associated with it. An instance of a named pipe
 * is always deleted when the last handle to the instance of the named pipe is closed.
 */

win32_pipe_server_actions_c::win32_pipe_server_actions_c( char * i_pPipeName, uAL32Bits i_uInputBufSize, uAL32Bits i_uOutputBufSize )
	:
	m_pPipeName( i_pPipeName ),
	m_uInputBufSize( i_uInputBufSize ),
	m_uOutputBufSize( i_uOutputBufSize )
//	m_SuccessfullyCreated(N32_FALSE)

{
	m_pInputBuffer= new char [ m_uInputBufSize ];
	m_pOutputBuffer= new u8 [ m_uOutputBufSize ];
}

result_t win32_pipe_server_actions_c::SERVER_Init( void )
{
	m_hHandle = CreateNamedPipe(
	                m_pPipeName,              // pipe name
	                PIPE_ACCESS_DUPLEX,       // read/write access
	                PIPE_TYPE_MESSAGE |       // message type pipe
	                PIPE_READMODE_MESSAGE |   // message-read mode
	                PIPE_WAIT,                 // blocking mode
	                1, //PIPE_UNLIMITED_INSTANCES, // max. instances
	                m_uOutputBufSize,         // output buffer size
	                m_uInputBufSize,          // input buffer size
	                0,                        // client time-out
	                NULL );                   // default security attribute

	if ( INVALID_HANDLE_VALUE == m_hHandle )
	{
		DEB( DEB_WARN, "Err: 0x%x GLE=0x%x\n", N32_ERR_EXTERNAL, GetLastError() );

		return( N32_ERR_EXTERNAL );
	}
	else
	{
		return( N32_NO_ERROR );
	}
}

win32_pipe_server_actions_c::~win32_pipe_server_actions_c()
{
}

result_t win32_pipe_server_actions_c::SERVER_WaitAndBlockForAConnection( void ) const
{
	BOOL fConnected= ConnectNamedPipe( m_hHandle, NULL )
	                 ?
	                 TRUE
	                 :
	                 ( GetLastError() == ERROR_PIPE_CONNECTED );

	if ( TRUE == fConnected )
	{
		/*		HANDLE hThread= CreateThread(
				                  NULL,              // no security attribute
				                  0,                 // default stack size
				                  InstanceThread,    // thread proc
				                  (LPVOID) hPipe,    // thread parameter
				                  0,                 // not suspended
				                  &dwThreadId);      // returns thread ID

				if (hThread == NULL)
				{
					_tprintf(TEXT("CreateThread failed, GLE=%d.\n"), GetLastError());
					return -1;
				}
				else CloseHandle(hThread);*/

		return( N32_NO_ERROR );
	}
	else
	{
		DEB( DEB_WARN, "Err: 0x%x GLE=0x%x\n", N32_ERR_EXTERNAL, GetLastError() );

		return( N32_ERR_EXTERNAL );
	}
}

result_t win32_pipe_server_actions_c::SERVER_ReadCommandFromAClient( void )
{
	DWORD cbBytesRead = 0;

	// Read client requests from the pipe. This simplistic code only allows messages
	// up to BUFSIZE characters in length.
	BOOL fSuccess = ReadFile(
	                    m_hHandle,		// handle to pipe
	                    m_pInputBuffer,		// buffer to receive data
	                    m_uInputBufSize,	// size of buffer
	                    &cbBytesRead,		// number of bytes read
	                    NULL );			// not overlapped I/O

	m_SearchKey.assign( m_pInputBuffer );

	if ( FALSE == fSuccess || 0x0 == cbBytesRead )
	{
		if ( ERROR_BROKEN_PIPE == GetLastError() )
		{
			DEB( DEB_WARN, "Client disconnected: 0x%x\n", N32_ERR_UNKNOWN );
			return( N32_ERR_UNKNOWN );
		}
		else
		{
			DEB( DEB_WARN, "ReadFile failed: 0x%x GLE=0x%x\n", N32_ERR_UNKNOWN, GetLastError() );
			return( N32_ERR_UNKNOWN );
		}
	}

	return( N32_NO_ERROR );
}

N32_BOOL win32_pipe_server_actions_c::server_isObjectPresentInTheRam( void ) const
{
	mapStoredObject_t::const_iterator it = m_mapStrObj.find( m_SearchKey );

	if ( likely( it != m_mapStrObj.end() ) )
	{
		return( N32_TRUE );
	}
	else
	{
		return( N32_FALSE );
	}
}

result_t win32_pipe_server_actions_c::server_SendDataToTheClient( void * i_ptrData, uAL32Bits i_uDataLength )
{
	DWORD cbWritten = 0;

	RET_ERR_IF_ZERO2( i_ptrData, i_uDataLength );

	DEB( DEB_LOG, "Writing %d bytes to the pipe\n", i_uDataLength );

	// Write the reply to the pipe.
	BOOL fSuccess = WriteFile(
	                    m_hHandle,			// handle to pipe
	                    i_ptrData,			// buffer to write from
	                    i_uDataLength,			// number of bytes to write
	                    &cbWritten,			// number of bytes written
	                    NULL );				// not overlapped I/O

	if ( FALSE == fSuccess || i_uDataLength != cbWritten )
	{
		DEB( DEB_WARN, "WriteFile failed: 0x%x GLE=0x%x\n", N32_ERR_UNKNOWN, GetLastError() );
		return( N32_ERR_UNKNOWN );
	}

	return( N32_NO_ERROR );
}

result_t win32_pipe_server_actions_c::server_SendObjToTheClient( void )
{
	stored_obj_c * ptrData= m_mapStrObj[ m_SearchKey ];
	uAL32Bits uDataLength= ptrData->getObjectLength();

	DEB( DEB_LOG, "Writing %d+4 bytes to the pipe\n", ptrData->getObjectLength() );

	result_t rRet;
	if ( likely( N32_NO_ERROR != ( rRet= server_SendDataToTheClient( &uDataLength, sizeof( uDataLength ) ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	if ( likely( N32_NO_ERROR != ( rRet= server_SendDataToTheClient( ptrData->getObjectAddress(), ptrData->getObjectLength() ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	return( N32_NO_ERROR );
}

result_t win32_pipe_server_actions_c::server_ReadObjFromDisc( void )
{
	stored_obj_c * pObj= new stored_obj_c();//= m_mapStrObj[ m_SearchKey ];

	if ( unlikely( N32_NULL == pObj ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", N32_ERR_MEM );
		return( N32_ERR_MEM );
	}

	result_t rRet;

	if ( unlikely( N32_NO_ERROR != ( rRet= pObj->tryToReadObjFromCache( m_SearchKey ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	m_mapStrObj[ m_SearchKey ]= pObj; // here a copy happens

	return( N32_NO_ERROR );
}

result_t win32_pipe_server_actions_c::server_SendNegativeReply( void )
{
	const uAL32Bits uDataToBeSent= 0x0;
	DWORD cbWritten = 0;
	uAL32Bits uDataLength= sizeof( uDataToBeSent );

	// Write the reply to the pipe.
	BOOL fSuccess = WriteFile(
	                    m_hHandle,		// handle to pipe
	                    &uDataToBeSent,		// buffer to write from
	                    uDataLength,		// number of bytes to write
	                    &cbWritten,		// number of bytes written
	                    NULL );			// not overlapped I/O

	if ( FALSE == fSuccess || uDataLength != cbWritten )
	{
		DEB( DEB_WARN, "WriteFile failed: 0x%x uDataLength=%d cbWritten=%d GLE=0x%x\n", N32_ERR_UNKNOWN, uDataLength, cbWritten, GetLastError() );
		return( N32_ERR_UNKNOWN );
	}

	return( N32_NO_ERROR );
}

result_t win32_pipe_server_actions_c::SERVER_ServeCommandFromAClient( void )
{
	result_t rRet;


	DEB( DEB_INFORM, "Key: %s\n", m_SearchKey.c_str() );

	if ( likely( N32_TRUE == server_isObjectPresentInTheRam() ) )
	{
		DEB( DEB_INFORM, "  ->cache hit\n" );
		// fetching from RAM
		if ( unlikely( N32_NO_ERROR != ( rRet= server_SendObjToTheClient() ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}
	}
	else
	{
		// reading from disc
		if ( unlikely( N32_NO_ERROR != ( rRet= server_ReadObjFromDisc() ) ) )
		{
			// some error encounterd
			DEB( DEB_INFORM, "  -> Cache missed (err=0x%x) - not found on disc - giving up\n", rRet );

			rRet= server_SendNegativeReply();

			DEB( DEB_LOG, "Err: 0x%x\n", rRet );
		}
		else
		{
			DEB( DEB_INFORM, "  ->Cache missed - object read from disc\n" );
			if ( unlikely( N32_NO_ERROR != ( rRet= server_SendObjToTheClient() ) ) )
			{
				DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			}
		}
	}

	return( rRet );
}

result_t win32_pipe_server_actions_c::SERVER_SendResultsToAClient( void ) const
{
	return( N32_ERR_EXTERNAL );
}

win32_pipe_server_actions_c::win32_pipe_server_actions_c( const win32_pipe_server_actions_c& rhs )
{

}

result_t win32_pipe_server_actions_c::SERVER_setCacheSizeLimit( uAL32Bits i_uNewCacheSizeLimit )
{
	if ( i_uNewCacheSizeLimit > 0x0 )
	{
		m_uCacheSizeLimit= i_uNewCacheSizeLimit;

		return( N32_NO_ERROR );
	}

	return( N32_ERR_BAD_PARAMETER );
}

result_t win32_pipe_server_actions_c::SERVER_Done( void )
{
	if ( INVALID_HANDLE_VALUE != m_hHandle )
	{
		FlushFileBuffers( m_hHandle );

		DisconnectNamedPipe( m_hHandle );

		CloseHandle( m_hHandle );
	}

	return( N32_NO_ERROR );
}
