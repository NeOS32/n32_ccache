#ifndef N32_CCACHE_APP_H
#define N32_CCACHE_APP_H

#include <Windows.h>
#include <shellapi.h>
#include <n32_base.h>

#include <tools/wrappers/md5_obj_c.hpp>

#include "cmndline_c.h"
#include "toolset_cmnds_c.h"
#include "toolset_cmndline_c.h"
#include "file_buffer_c.hpp"
#include "stored_obj_c.h"

#define MAX_PATH_LENGTH ( 256 )
#define MAX_NUMBER_OF_CONNECTION_TRIES ( 3 )
#define STRING_CMND_SERVER	"--server"

typedef enum app_run_mode_t
{
	APP_GENERATION_TRANSPARENT,
	APP_GENERATION_NORMAL_FROM_DISC,
	APP_GENERATION_NORMAL_FROM_RAM,
	APP_GENERATION_SERVER_MODE,
	APP_GENERATION_UNKNOWN
} app_run_mode_t;

class n32_ccache_app_c
{

private:
	LPTSTR			m_pCommandLine;
	toolset_cmnds_c		m_Toolset;
	toolset_cmndline_c	m_ToolsetCmndLine;
	char			m_CurrentPath[ MAX_PATH_LENGTH ];
	MD5_hex_value_t		m_CalculatedHexValue;
	app_run_mode_t		m_eRunMode;
	//file_buffer_c<char>	m_FileBuffer;
	stored_obj_c		m_ObjectFile;

	n32_ccache_app_c(const n32_ccache_app_c& rhs);
	n32_ccache_app_c& operator=(const n32_ccache_app_c& rhs);
	result_t		getPreprocessedFileName( string & o_FileName );
	//result_t		getEnvVarValue( const string & i_VariableName, string & o_VariableValue ) const;
	result_t		DoCountCheckSum(void) const;
	result_t		ReadGivenFile( const string & i_rFileName, file_buffer_c<char> & o_FileBuffer );
	result_t		WriteGivenFile( const string & i_rFileName, file_buffer_c<char> & o_FileBuffer );
	result_t		CopyFile( const string & i_rSourceFileeName, const string & i_rDestinationFileeName );
	
	result_t		Serve_MODE_Ram( void );
	result_t		Serve_MODE_Disc( void );
	result_t		Serve_MODE_Transparent( void );
	result_t		StartProcessingInServerMode(void);
	result_t		client_WriteRequestToServer( HANDLE & i_rhPipe, const MD5_hex_value_t & i_MD5Value );
	result_t		client_ReceiveDataFromServer( HANDLE & i_rhPipe, void * i_ptrData, uAL32Bits i_uDataLength );
	result_t		client_ReceiveObjectFromServer( HANDLE & i_rhPipe, stored_obj_c & i_StoredObject );
	result_t		client_SaveOBject( stored_obj_c & i_LoadedObject );

public:
	n32_ccache_app_c( int argc, char * argv[] );
	app_run_mode_t		getRunMode( void ) const;
	app_run_mode_t		setRunMode( app_run_mode_t i_NewRunMode );
	result_t		ParseCommandLine( void );
	result_t		PrepareCommandLine( void );
	result_t		ExecutePreProcessor( void );
	result_t		ExecuteOriginalCommand( void );
	result_t		StoreInCacheIfNeeded( void );
	result_t		CopyAppropriateObject( void );
	result_t		CountCheckSum( void );
	
	result_t		StartProcessing( void );
	//result_t		StartProcessingInClientMode( void );

	virtual ~n32_ccache_app_c();

};

#endif // N32_CCACHE_APP_H
