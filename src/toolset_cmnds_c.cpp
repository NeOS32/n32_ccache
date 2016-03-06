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
 * C++ toolset_cmnds_c
 *
 * Supported command line options (gcc).
 */

#include "toolset_cmnds_c.h"

toolset_cmnds_c::toolset_cmnds_c()
{
	CommandsName[ TOOLSET_CMND_OUTPUT ]= "-o";
	CommandsName[ TOOLSET_CMND_COMPILATION ]= "-c";
	CommandsName[ TOOLSET_CMND_PREPROCESSING ]= "-E";
	//CommandsName[ TOOLSET_CMND_LINKING ]= "-E";
	CommandsName[ TOOLSET_CMND_DEPENDENCY ]= "-M";
	CommandsName[ TOOLSET_CMND_UNKNOWN ]= "-M";
}

toolset_cmnds_c::~toolset_cmnds_c()
{
}

toolset_cmnds_c::toolset_cmnds_c( const toolset_cmnds_c& rhs )
{
}

const string toolset_cmnds_c::getSwitchFor( const toolset_cmnd_output_t i_eCommandName )
{
	return( CommandsName[ i_eCommandName ] );
}
