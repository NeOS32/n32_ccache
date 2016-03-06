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
 * C/C++ main()
 *
 * The program starting point.
 */

#include "stdafx.h"
#include <n32_base.h>
#include <string>
#include <iostream>
#include "Windows.h"
#include <fstream>
#include <shellapi.h>
#include <direct.h>
#include "cmndline_c.h"
#include "toolset_cmnds_c.h"
#include "n32_ccache_app_c.h"

using namespace std;

LOCAL result_t GoModeTransparent( n32_ccache_app_c & App )
{
	return( 0 );
}


int main(int argc,char* argv[])
{
#ifdef DEBUG
	DEBUG_SetSeverity( DEB_INFORM );
#endif

	n32_ccache_app_c App( argc, argv );
	
	result_t rRet= App.StartProcessing();

	return( rRet );
}
