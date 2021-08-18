// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

/*
 * C++ toolset_cmnds_c
 *
 * Supported command line options (gcc).
 */

#include "toolset_cmnds_c.h"

toolset_cmnds_c::toolset_cmnds_c()
{
	CommandsName[TOOLSET_CMND_OUTPUT] = "-o";
	CommandsName[TOOLSET_CMND_COMPILATION] = "-c";
	CommandsName[TOOLSET_CMND_PREPROCESSING] = "-E";
	//CommandsName[ TOOLSET_CMND_LINKING ]= "-E";
	CommandsName[TOOLSET_CMND_DEPENDENCY] = "-M";
	CommandsName[TOOLSET_CMND_UNKNOWN] = "-M";
}

toolset_cmnds_c::~toolset_cmnds_c()
{
}

toolset_cmnds_c::toolset_cmnds_c(const toolset_cmnds_c& rhs)
{
}

const string toolset_cmnds_c::getSwitchFor(const toolset_cmnd_output_t i_eCommandName)
{
	return(CommandsName[i_eCommandName]);
}
