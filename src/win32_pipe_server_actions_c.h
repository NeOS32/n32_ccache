#ifndef WIN32_PIPE_SERVER_ACTIONS_C_H
#define WIN32_PIPE_SERVER_ACTIONS_C_H

#include "win32_server_c.h" // Base class: win32_server_actions_c
#include "stored_obj_c.h" // Base class: win32_server_actions_c
#include <map>
#include <n32_base.h>
#include <tools/wrappers/md5_obj_c.hpp>

// maximum memory to be used by n32_ccache
#define MAX_CACHE_SIZE		( 50 * 1024 * 1024 )
#define MAX_OBJ_FILE_SIZE	( 5 * 1024 * 1024 )

typedef map<string,stored_obj_c*>	mapStoredObject_t;

class win32_pipe_server_actions_c : public win32_server_actions_c
{

private:
	char *				m_pPipeName;
	uAL32Bits			m_uInputBufSize;
	uAL32Bits			m_uOutputBufSize;
	uAL32Bits			m_uCacheSizeLimit;
	char *				m_pInputBuffer;
	u8 *				m_pOutputBuffer;
	string				m_SearchKey;
	mapStoredObject_t		m_mapStrObj;

	win32_pipe_server_actions_c(const win32_pipe_server_actions_c& rhs);
	win32_pipe_server_actions_c& operator=(const win32_pipe_server_actions_c& rhs);
	
	N32_BOOL	 		server_isObjectPresentInTheRam( void ) const;
	result_t			server_SendObjToTheClient(void);
	result_t			server_SendDataToTheClient( void * i_ptrData, uAL32Bits i_uDataLength );
	result_t			server_ReadObjFromDisc(void);
	result_t			server_SendNegativeReply(void);

public:
	win32_pipe_server_actions_c( char * i_pPipeName, uAL32Bits i_uInputBufSize, uAL32Bits i_uOutputBufSize );
	result_t			SERVER_setCacheSizeLimit( uAL32Bits i_uNewCacheSizeLimit= MAX_CACHE_SIZE );
	
	virtual result_t		SERVER_Init( void );
	virtual result_t		SERVER_WaitAndBlockForAConnection( void ) const;
	virtual result_t		SERVER_ReadCommandFromAClient( void );
	virtual result_t		SERVER_ServeCommandFromAClient( void );
	virtual result_t		SERVER_SendResultsToAClient( void ) const;
	virtual result_t		SERVER_Done( void );

	virtual ~win32_pipe_server_actions_c();

};

#endif // WIN32_PIPE_SERVER_ACTIONS_C_H
