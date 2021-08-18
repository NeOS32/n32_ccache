// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

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

LOCAL result_t GoModeTransparent(n32_ccache_app_c& App)
{
	return(0);
}


int main(int argc, char* argv[])
{
#ifdef DEBUG
	DEBUG_SetSeverity(DEB_INFORM);
#endif

	n32_ccache_app_c App(argc, argv);

	result_t rRet = App.StartProcessing();

	return(rRet);
}
