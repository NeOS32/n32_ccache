// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

/*
 * C++ toolset_cmndline_c
 *
 * This class is in charge of parsing toolset command line. In the future if needed, could be
 * used as an inerface for another toolsets.
 */

#include "toolset_cmndline_c.h"

toolset_cmndline_c::toolset_cmndline_c(const LPTSTR i_FullCommandLine) :cmndline_c(i_FullCommandLine), m_eToolsetCommandMode(TOOLSET_CMND_UNKNOWN)
{
}

toolset_cmndline_c::~toolset_cmndline_c()
{
}

toolset_cmndline_c::toolset_cmndline_c() :m_eToolsetCommandMode(TOOLSET_CMND_UNKNOWN)
{
}

toolset_cmndline_c::toolset_cmndline_c(const toolset_cmndline_c& rhs) : m_eToolsetCommandMode(TOOLSET_CMND_UNKNOWN)
{
}

TokenIndex toolset_cmndline_c::getIndexOfToolsetCommand(toolset_cmnds_c& i_ToolsetInstance, toolset_cmnd_output_t i_Command, result_t& o_rRet)
{
	string strSwitch = i_ToolsetInstance.getSwitchFor(i_Command);

	if (unlikely(0x0 == strSwitch.size()))
	{
		return(NoneExistingToken);
	}

	return(getIndexOfToken(strSwitch, o_rRet));
}

result_t toolset_cmndline_c::getNextTokenAfterCommand(toolset_cmnds_c& i_ToolsetInstance, toolset_cmnd_output_t i_Command, string& o_rToken)
{
	string strSwitch = i_ToolsetInstance.getSwitchFor(i_Command);
	result_t rRet;
	TokenIndex iIndex;

	if (unlikely(0x0 == strSwitch.size()))
	{
		return(NoneExistingToken);
	}

	if (unlikely(NoneExistingToken == (iIndex = getIndexOfToken(strSwitch, rRet))))
	{
		DEB(DEB_WARN, "Err: 0x%x\n", rRet);
		return(rRet);
	}

	iIndex++; // now, iIndex should point to the next token

	if (unlikely(N32_FALSE == IsThisProperIndex(iIndex)))
	{
		DEB(DEB_WARN, "Err: 0x%x ('%s' not found)\n", rRet, i_ToolsetInstance.getSwitchFor(i_Command).c_str());
		return(N32_ERR_NOT_FOUND);
	}

	o_rToken = getTokenByNumber(iIndex);

	return(N32_NO_ERROR);
}

result_t toolset_cmndline_c::setWorkingMode(toolset_cmnds_c& i_ToolsetInstance)
{
	result_t rRet;
	TokenIndex iIndex;

	for (int IndexOfCommands = 0; IndexOfCommands < TOOLSET_MAX_NUMBER; IndexOfCommands++)
	{
		string strSwitch = i_ToolsetInstance.getSwitchFor(aCmndLine_AllCommands[IndexOfCommands]);
		if (likely(NoneExistingToken != (iIndex = getIndexOfToken(strSwitch, rRet))))
		{
			m_eToolsetCommandMode = aCmndLine_AllCommands[IndexOfCommands];
			break;
		}
	}

	return(N32_NO_ERROR);
}

toolset_cmnd_output_t toolset_cmndline_c::getCommandLineMode(void)
{
	return(m_eToolsetCommandMode);
}
