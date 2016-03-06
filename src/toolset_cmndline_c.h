#ifndef TOOLSET_CMNDLINE_C_H
#define TOOLSET_CMNDLINE_C_H

#include "cmndline_c.h" // Base class: cmndline_c
#include "toolset_cmnds_c.h"

class toolset_cmndline_c : public cmndline_c
{

private:
	toolset_cmnd_output_t	m_eToolsetCommandMode;
	toolset_cmndline_c(const toolset_cmndline_c& rhs);
	toolset_cmndline_c& operator=(const toolset_cmndline_c& rhs);

public:
	toolset_cmndline_c( const LPTSTR i_FullCommandLine );
	toolset_cmndline_c();
	TokenIndex		getIndexOfToolsetCommand( toolset_cmnds_c & i_ToolsetInstance, toolset_cmnd_output_t i_Command, result_t & o_rRet );
	result_t		getNextTokenAfterCommand( toolset_cmnds_c & i_ToolsetInstance, toolset_cmnd_output_t i_Command, string & o_rToken );
	toolset_cmnd_output_t	getCommandLineMode( void );
	result_t		setWorkingMode( toolset_cmnds_c & i_ToolsetInstance );
	virtual ~toolset_cmndline_c();
};

#endif // TOOLSET_CMNDLINE_C_H
