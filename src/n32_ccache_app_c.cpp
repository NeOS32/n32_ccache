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
 * C/C++ n32_ccache_app_c
 *
 * Application class.
 *
 */

#include "n32_ccache_app_c.h"
#include "executor_c.h"
#include "checksum_counter_c.h"
#include "win32_server_c.h"
#include "win32_pipe_server_actions_c.h"
#include "stored_obj_c.h"

#include <tools/wrappers/md5_obj_c.hpp>

#include <Windows.h>
#include <shellapi.h>
#include <direct.h>

#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>

n32_ccache_app_c::n32_ccache_app_c( int argc, char * argv[] ):m_eRunMode( APP_GENERATION_UNKNOWN )
{
	if ( 0x2 == argc )
	{
		if ( 0x0 == strcmp( argv[ 1 ], STRING_CMND_SERVER ) )
		{
			setRunMode( APP_GENERATION_SERVER_MODE );
			return;
		}
	}

	m_pCommandLine= GetCommandLine();
	if ( N32_NULL == m_pCommandLine )
	{
		return;
	}

	result_t rRet;
	string strWorkingMode;

	setRunMode( APP_GENERATION_NORMAL_FROM_DISC );

	if ( likely( N32_NO_ERROR == ( rRet= getEnvVarValue( "N32_CCACHE_MODE", strWorkingMode ) ) ) )
	{
		if  ( 0x0 == strWorkingMode.compare( "MODE_RAM" ) )
		{
			// caching on in RAM
			setRunMode( APP_GENERATION_NORMAL_FROM_RAM );

		}
		else
			if  ( 0x0 == strWorkingMode.compare( "MODE_DISC" ) )
			{
				// caching on disc - default
				setRunMode( APP_GENERATION_NORMAL_FROM_DISC );
			}
			else
				if ( 0x0 == strWorkingMode.compare( "MODE_NONE" ) )
				{
					// no caching - transparent
					setRunMode( APP_GENERATION_TRANSPARENT );
				}
				else
				{
					DEB( DEB_WARN, "Unknown value for N32_CCACHE_MODE - defaulting to APP_GENERATION_NORMAL_FROM_DISC\n" );
					// caching on disc - default
				}
	}

	// use the function to get the path
	// getcwd( m_CurrentPath, MAX_PATH_LENGTH);
}

n32_ccache_app_c::~n32_ccache_app_c()
{
}

n32_ccache_app_c::n32_ccache_app_c( const n32_ccache_app_c& rhs )
{
}


result_t n32_ccache_app_c::ParseCommandLine( void )
{
	result_t rRet;

	if ( unlikely( N32_NO_ERROR != ( rRet= m_ToolsetCmndLine.DoProcessGivenString( m_pCommandLine ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x ('%s' not found)\n", rRet, m_Toolset.getSwitchFor( TOOLSET_CMND_COMPILATION ).c_str() );
		return( rRet ); //we don't want to leave if e.g. -DDEBUG is duplicated
	}

	if ( unlikely( N32_NO_ERROR != ( rRet= m_ToolsetCmndLine.setWorkingMode( m_Toolset ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	if ( TOOLSET_CMND_COMPILATION != m_ToolsetCmndLine.getCommandLineMode() )
	{
		setRunMode( APP_GENERATION_TRANSPARENT );

		return( N32_ERR_BAD_PARAMETER );
	}

	return( N32_NO_ERROR );
}

result_t n32_ccache_app_c::PrepareCommandLine( void )
{
	string strPreProcess= m_Toolset.getSwitchFor( TOOLSET_CMND_PREPROCESSING );

	result_t rRet= N32_ERR_UNKNOWN;

	TokenIndex piCompileCommandToken= m_ToolsetCmndLine.getIndexOfToolsetCommand( m_Toolset, TOOLSET_CMND_COMPILATION, rRet );
	if ( unlikely( N32_NO_ERROR != rRet ) )
	{
		DEB( DEB_WARN, "Err: 0x%x ('%s' not found)\n", rRet, m_Toolset.getSwitchFor( TOOLSET_CMND_COMPILATION ).c_str() );
		return( rRet );
	}

	if ( unlikely( N32_NO_ERROR != ( rRet= m_ToolsetCmndLine.setTokenAtIndex( strPreProcess, piCompileCommandToken ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	return( N32_NO_ERROR );
}

result_t n32_ccache_app_c::ExecutePreProcessor( void )
{
	result_t rRet;
	string Full;
	executor_c Exec;

	if ( unlikely( N32_NO_ERROR != ( rRet= m_ToolsetCmndLine.getCurrentString( Full ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	if ( unlikely( N32_NO_ERROR != ( rRet= Exec.ExecuteCommand( Full ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	DEB( DEB_LOG, "New command: %s\n", Full.c_str() );

	return( N32_NO_ERROR );
}

result_t n32_ccache_app_c::ExecuteOriginalCommand( void )
{
	result_t rRet;
	string strOrginalCommand;
	TokenIndex IndexOfFirstCommand= 1;

	if ( unlikely( N32_NO_ERROR != ( rRet= m_ToolsetCmndLine.getSubStringFromGivenIndex( strOrginalCommand, IndexOfFirstCommand ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	executor_c Exec;
	if ( unlikely( N32_NO_ERROR != ( rRet= Exec.ExecuteCommand( strOrginalCommand ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	return( N32_NO_ERROR );
}

result_t n32_ccache_app_c::CountCheckSum( void )
{
	string FileName;
	result_t rRet;

	if ( unlikely( N32_NO_ERROR != ( rRet= m_ToolsetCmndLine.getNextTokenAfterCommand( m_Toolset, TOOLSET_CMND_OUTPUT, FileName ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	checksum_counter_c CheckSumCounter( FileName );
	if ( unlikely( N32_NO_ERROR != ( rRet= CheckSumCounter.DoCountCheckSum( m_ToolsetCmndLine.getOrignalString(), m_ObjectFile, m_CalculatedHexValue ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	return( N32_NO_ERROR );
}

result_t n32_ccache_app_c::StoreInCacheIfNeeded( void )
{
	return( N32_NO_ERROR );
}

result_t n32_ccache_app_c::ReadGivenFile( const string & i_rFileName, file_buffer_c<char> & o_FileBuffer )
{
	ifstream file( i_rFileName.c_str(), ios::in|ios::binary|ios::ate );

	if ( likely( file.is_open() ) )
	{
		result_t rRet;
		ifstream::pos_type uFileSize= file.tellg();

		if ( unlikely( N32_NO_ERROR != ( rRet= o_FileBuffer.Init( uFileSize ) ) ) )
		{
			file.close();
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}

		file.seekg( 0, ios::beg );
		file.read( o_FileBuffer.getBufferAddr(), uFileSize );
		file.close();

		if ( unlikely( N32_NO_ERROR != ( rRet= o_FileBuffer.addBufferSize( uFileSize ) ) ) )
		{
			file.close();
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}

		return( N32_NO_ERROR );
	}

	return( N32_ERR_UNKNOWN );
}

result_t n32_ccache_app_c::WriteGivenFile( const string & i_rFileName, file_buffer_c<char> & o_FileBuffer )
{
	ofstream file( i_rFileName.c_str(), ios::out|ios::binary );

	if ( likely( file.is_open() ) )
	{
		file.write( o_FileBuffer.getBufferAddr(), o_FileBuffer.getBufferSize() );
		file.close();

		return( N32_NO_ERROR );
	}

	return( N32_ERR_UNKNOWN );
}

result_t n32_ccache_app_c::CopyFile( const string & i_rSourceFileeName, const string & i_rDestinationFileeName )
{
	file_buffer_c<char> FileBuffer;
	result_t rRet;

	if ( unlikely( N32_NO_ERROR != ( rRet= ReadGivenFile( i_rSourceFileeName, FileBuffer ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	if ( unlikely( N32_NO_ERROR != ( rRet= WriteGivenFile( i_rDestinationFileeName, FileBuffer ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	return( N32_NO_ERROR );
}

result_t n32_ccache_app_c::CopyAppropriateObject( void )
{
	result_t rRet;
	string strCCacheDir;

	if ( unlikely( N32_NO_ERROR != ( rRet= getEnvVarValue( "N32_CCACHE_DIR", strCCacheDir ) ) ) )
	{
		if ( likely( N32_NO_ERROR != ( rRet= getEnvVarValue( "TEMP", strCCacheDir ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}
	}

	strCCacheDir += '\\';

	if ( unlikely( N32_NO_ERROR != ( rRet= dirCreateIfDoesntExist( strCCacheDir.c_str() ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	char DirPrefix[ FIRST_LEVEL_DEPTH + SECOND_LEVEL_DEPTH + 2 + 1 ]; // 1 for \0, 2 for \\ (backslashes)
	memcpy( &DirPrefix[ 0 ], &m_CalculatedHexValue[ 0 ], FIRST_LEVEL_DEPTH );
	DirPrefix[ FIRST_LEVEL_DEPTH ]= '\\';
	DirPrefix[ FIRST_LEVEL_DEPTH + 1 ]= 0x0;

	string strDirectory= strCCacheDir + DirPrefix;
	if ( unlikely( N32_NO_ERROR != ( rRet= dirCreateIfDoesntExist( strDirectory.c_str() ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	memcpy( &DirPrefix[ FIRST_LEVEL_DEPTH + 1 ], &m_CalculatedHexValue[ FIRST_LEVEL_DEPTH ], FIRST_LEVEL_DEPTH );
	DirPrefix[ FIRST_LEVEL_DEPTH + SECOND_LEVEL_DEPTH + 1 ]= '\\';
	DirPrefix[ FIRST_LEVEL_DEPTH + SECOND_LEVEL_DEPTH + 2 ]= 0x0;

	strDirectory= strCCacheDir + DirPrefix;
	if ( unlikely( N32_NO_ERROR != ( rRet= dirCreateIfDoesntExist( strDirectory.c_str() ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	strCCacheDir= strDirectory + m_CalculatedHexValue;

	struct stat buf;
	int result;

	/* Get data associated with "stat.c": */
	result = stat( strCCacheDir.c_str(), &buf );

	/* Check if statistics are valid: */
	if( unlikely( ( 0x0 != result )||( 0x0 == buf.st_size ) ) )
	{
		// cached file doesn't exist
		string strOrginalCommand;
		TokenIndex IndexOfFirstCommand= 1;

		if ( unlikely( N32_NO_ERROR != ( rRet= m_ToolsetCmndLine.getSubStringFromGivenIndex( strOrginalCommand, IndexOfFirstCommand ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}

		executor_c Exec;
		if ( unlikely( N32_NO_ERROR != ( rRet= Exec.ExecuteCommand( strOrginalCommand ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}

		string FileName;
		if ( unlikely( N32_NO_ERROR != ( rRet= m_ToolsetCmndLine.getNextTokenAfterCommand( m_Toolset, TOOLSET_CMND_OUTPUT, FileName ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}

		if ( unlikely( N32_NO_ERROR != ( rRet= CopyFile( FileName, strCCacheDir.c_str() ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}
	}
	else
	{
		// cached file exists!
		string FileName;
		if ( unlikely( N32_NO_ERROR != ( rRet= m_ToolsetCmndLine.getNextTokenAfterCommand( m_Toolset, TOOLSET_CMND_OUTPUT, FileName ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}

		if ( unlikely( N32_NO_ERROR != ( rRet= CopyFile( strCCacheDir.c_str(), FileName ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}
		/* Output some of the statistics: */
		/*printf( "File size : %ld\n", buf.st_size );
		printf( "Drive : %c:\n", buf.st_dev + 'A' );
		printf( "Time modified : %s", ctime( &buf.st_mtime ) );*/
	}

	return( N32_NO_ERROR );
}

result_t n32_ccache_app_c::DoCountCheckSum( void ) const
{
	return( N32_ERR_UNKNOWN );
}

result_t n32_ccache_app_c::Serve_MODE_Disc( void )
{
	result_t rRet;

	if ( likely( N32_NO_ERROR == ( rRet= ParseCommandLine() ) ) )
	{
		if ( likely( N32_NO_ERROR == ( rRet= PrepareCommandLine() ) ) )
		{
			if ( likely( N32_NO_ERROR == ( rRet= ExecutePreProcessor() ) ) )
			{
				if ( likely( N32_NO_ERROR == ( rRet= CountCheckSum() ) ) )
				{
					if ( likely( N32_NO_ERROR == ( rRet= CopyAppropriateObject() ) ) )
					{
					}
				}
			}
		}
	}

	return( rRet );
}

result_t n32_ccache_app_c::StartProcessingInServerMode( void )
{
	result_t rRet;

	// sprawdzic czy juz nie dziala ...
	win32_pipe_server_actions_c pipeServerActions( TEXT( "\\\\.\\pipe\\n32_ccache" ), 1024*4, 1024*4 );
	win32_server_c pipeServer( pipeServerActions );

	if ( unlikely( N32_NO_ERROR != ( rRet= pipeServer.rInit() ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}
	else
	{
		rRet= pipeServer.rStart();
		DEB( DEB_LOG, "Closing server with value: 0x%x\n", rRet );
	}

	return( rRet );
}

result_t n32_ccache_app_c::client_WriteRequestToServer( HANDLE & i_rhPipe, const MD5_hex_value_t & i_MD5Value )
{
	BOOL fSuccess;

	// The pipe connected; change to message-read mode.
	DWORD dwMode = PIPE_READMODE_BYTE;
	fSuccess = SetNamedPipeHandleState(
	               i_rhPipe,    // pipe handle
	               &dwMode,  // new pipe mode
	               NULL,     // don't set maximum bytes
	               NULL );   // don't set maximum time

	if ( unlikely( FALSE == fSuccess ) )
	{
		DEB( DEB_WARN, "Err: 0x%x, GLE: 0x%x\n", N32_ERR_UNKNOWN, GetLastError() );
		return( N32_ERR_UNKNOWN );
	}

	// Send a message to the pipe server.
	DWORD cbWritten= 0x0, cbToWrite = sizeof( i_MD5Value );//(lstrlen(lpvMessage)+1)*sizeof(TCHAR);

	DEB( DEB_LOG, "Sending: '%s'\n", i_MD5Value );

	fSuccess = WriteFile(
	               i_rhPipe,                  // pipe handle
	               &i_MD5Value[ 0 ],             // message
	               cbToWrite,              // message length
	               &cbWritten,             // bytes written
	               NULL );                 // not overlapped

	if ( unlikely( ( FALSE == fSuccess )||( cbWritten != cbToWrite  ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x, GLE: 0x%x\n", N32_ERR_UNKNOWN, GetLastError() );
		return( N32_ERR_UNKNOWN );
	}

	return( N32_NO_ERROR );
}

result_t n32_ccache_app_c::client_ReceiveDataFromServer( HANDLE & i_rhPipe, void * i_ptrData, uAL32Bits i_uDataLength )
{
	BOOL fSuccess;
	DWORD cbRead = 0;

	do
	{
		// Read from the pipe.
		fSuccess = ReadFile(
		               i_rhPipe,    		// pipe handle
		               i_ptrData,    		// buffer to receive reply
		               i_uDataLength,	// size of buffer
		               &cbRead,  			// number of bytes read
		               NULL );    			// not overlapped

		if ( FALSE == fSuccess && ERROR_MORE_DATA != GetLastError() )
		{
			DEB( DEB_WARN, "WriteFile failed: 0x%x GLE=0x%x\n", N32_ERR_UNKNOWN, GetLastError() );
			return( N32_ERR_UNKNOWN );
		}
	}
	while ( ! fSuccess ); // repeat loop if ERROR_MORE_DATA


	//if ( ! fSuccess && GetLastError() != ERROR_MORE_DATA )
	//  break;

	return( N32_NO_ERROR );
}

result_t n32_ccache_app_c::client_ReceiveObjectFromServer( HANDLE & i_rhPipe, stored_obj_c & i_StoredObject )
{
	result_t rRet;
	uAL32Bits uDataLength;

	if ( likely( N32_NO_ERROR != ( rRet= client_ReceiveDataFromServer( i_rhPipe, &uDataLength, sizeof( uDataLength ) ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	/*	if ( likely( N32_NO_ERROR != ( rRet= m_FileBuffer.Init( uDataLength ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}*/
	if ( uDataLength > 0x0 )
	{
		if ( likely( N32_NO_ERROR != ( rRet= i_StoredObject.Init( uDataLength ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}

		//if ( likely( N32_NO_ERROR != ( rRet= client_ReceiveDataFromServer( i_rhPipe, m_FileBuffer.getBufferAddr(), uDataLength ) ) ) )
		if ( likely( N32_NO_ERROR != ( rRet= client_ReceiveDataFromServer( i_rhPipe, i_StoredObject.getObjectAddress(), uDataLength ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}

		if ( likely( N32_NO_ERROR != ( rRet= i_StoredObject.addBufferSize( uDataLength ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}
	}
	else
	{
		DEB( DEB_WARN, "Err: 0x%x (seems server couldn't find this object)\n", N32_ERR_NOT_FOUND );
		return( N32_ERR_NOT_FOUND );
	}

	return( rRet );
}

result_t n32_ccache_app_c::client_SaveOBject( stored_obj_c & i_LoadedObject )
{
	result_t rRet;

	string FileName;
	if ( unlikely( N32_NO_ERROR != ( rRet= m_ToolsetCmndLine.getNextTokenAfterCommand( m_Toolset, TOOLSET_CMND_OUTPUT, FileName ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	if ( unlikely( N32_NO_ERROR != ( rRet= i_LoadedObject.WriteGivenFile( FileName ) ) ) )
	{
		DEB( DEB_WARN, "Err: 0x%x\n", rRet );
		return( rRet );
	}

	return( rRet );
}

result_t n32_ccache_app_c::Serve_MODE_Ram( void )
{
	result_t rRet;

	if ( likely( N32_NO_ERROR == ( rRet= ParseCommandLine() ) ) )
	{
		if ( likely( N32_NO_ERROR == ( rRet= PrepareCommandLine() ) ) )
		{
			if ( likely( N32_NO_ERROR == ( rRet= ExecutePreProcessor() ) ) )
			{
				if ( likely( N32_NO_ERROR == ( rRet= CountCheckSum() ) ) )
				{
					HANDLE hPipe;
					const char * PipeName= TEXT( "\\\\.\\pipe\\n32_ccache" );

					uAL32Bits uTries= MAX_NUMBER_OF_CONNECTION_TRIES;
					while( uTries-- > 0x0 )
					{
						hPipe = CreateFile(
						            PipeName,   // pipe name
						            GENERIC_READ |  // read and write access
						            GENERIC_WRITE,
						            0,              // no sharing
						            NULL,           // default security attributes
						            OPEN_EXISTING,  // opens existing pipe
						            0,              // default attributes
						            NULL );         // no template file

						// Break if the pipe handle is valid.
						if ( likely( INVALID_HANDLE_VALUE != hPipe ) )
						{
							break;
						}
						else
						{
							int iError;
							// Exit if an error other than ERROR_PIPE_BUSY occurs.
							if ( unlikely( ERROR_PIPE_BUSY != ( iError= GetLastError() ) ) )
							{
								// no handle - let's start a server
								executor_c Exec;
								TokenIndex uTokenNumber= 0x0;
								string strCommand( m_ToolsetCmndLine.getTokenByNumber( uTokenNumber ) );
								strCommand += " ";
								strCommand += STRING_CMND_SERVER;
								//strCommand = "d:\\docs\\cvs\\NeOS32\\out\\_n32_ccache-IA32-i386-RELEASE.exe --server";

								if ( unlikely( N32_NO_ERROR != ( rRet= Exec.ExecuteCommand( strCommand, N32_FALSE, N32_FALSE ) ) ) )
								{
									DEB( DEB_WARN, "Err: 0x%x Pipe=%d\n", rRet, iError );

									CloseHandle( hPipe );

									return( rRet );
								}
							}
						}

						// All pipe instances are busy, so wait for 100 ms
						Sleep( 100 );
					}

					if ( 0 == uTries )
					{
						// pipe is still not available - giving up
						return( N32_ERR_ALREADY_TAKEN );
					}

					if ( likely( N32_NO_ERROR == ( rRet= client_WriteRequestToServer( hPipe, m_CalculatedHexValue ) ) ) )
					{
						stored_obj_c StoredObject;

						if ( likely( N32_NO_ERROR == ( rRet= client_ReceiveObjectFromServer( hPipe, StoredObject ) ) ) )
						{
							if ( likely( N32_NO_ERROR == ( rRet= client_SaveOBject( StoredObject ) ) ) )
							{
							}
						}
					}

					if ( unlikely( N32_NO_ERROR != rRet ) )
					{
						DEB( DEB_WARN, "Err: 0x%x\n", rRet );
						//return( rRet );

						if ( unlikely( N32_NO_ERROR != rRet ) )
						{
							if ( likely( TOOLSET_CMND_COMPILATION == m_ToolsetCmndLine.getCommandLineMode() ) )
							{
								if ( likely( N32_NO_ERROR == ( rRet= CopyAppropriateObject() ) ) )
								{
								}
							}
							else
							{
								if ( likely( N32_NO_ERROR == ( rRet= Serve_MODE_Transparent() ) ) )
								{
								}
							}
						}
					}
				}

				//TODO - zwalnianie handlera
			}
		}
	}


	return( rRet );
}

result_t n32_ccache_app_c::Serve_MODE_Transparent( void )
{
	return( ExecuteOriginalCommand() );
}


result_t n32_ccache_app_c::StartProcessing( void )
{
	result_t rRet;

	switch ( getRunMode() )
	{
	case APP_GENERATION_NORMAL_FROM_RAM:
		rRet= Serve_MODE_Ram();
		break;

	case APP_GENERATION_NORMAL_FROM_DISC:
		rRet= Serve_MODE_Disc();
		break;

	case APP_GENERATION_SERVER_MODE:

		extern result_t Win32_RedirectStdOutToFile( const char * i_FileName );
		Win32_RedirectStdOutToFile( "c:\logs.txt" );
		rRet= StartProcessingInServerMode();
		break;

	case APP_GENERATION_UNKNOWN:
	case APP_GENERATION_TRANSPARENT:
	default
			:
		rRet= Serve_MODE_Transparent();
		break;
	}

	if ( unlikely( N32_NO_ERROR != rRet ) )
	{
		if ( likely( N32_NO_ERROR == ( rRet= Serve_MODE_Transparent() ) ) )
		{
		}
	}

	return( rRet );
}

result_t n32_ccache_app_c::getPreprocessedFileName( string& o_FileName )
{
	return( N32_ERR_UNKNOWN );
}

app_run_mode_t n32_ccache_app_c::getRunMode( void ) const
{
	return( m_eRunMode );
}

app_run_mode_t n32_ccache_app_c::setRunMode( app_run_mode_t i_NewRunMode )
{
	app_run_mode_t ePreviourRunMode= getRunMode();

	m_eRunMode= i_NewRunMode;

	return( ePreviourRunMode );
}
