// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

/*
 * C/C++ checksum_counter_c
 *
 * The idea behind this class was to have separate class in charge of
 * checksum calculation.
 */

#include "checksum_counter_c.h"

#include <tools/wrappers/md5_obj_c.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <n32_base.h>

using namespace std;
using namespace n32_md5_wrapper;

checksum_counter_c::~checksum_counter_c()
{
}

checksum_counter_c::checksum_counter_c( const string & i_FileName ):m_rFileName( i_FileName )
{
}

result_t checksum_counter_c::DoCountCheckSum( const string & i_vctrCommandLine, stored_obj_c & io_FileBuffer, MD5_hex_value_t & o_CalculatedHexValue ) const
{
	ifstream::pos_type uFileSize;
	//char * memblock;
	result_t rRet;
	vector<char> vctrBuffer;

	ifstream file( m_rFileName.c_str(), ios::in|ios::binary|ios::ate );
	if ( likely( file.is_open() ) )
	{
		uFileSize = file.tellg();

		if ( unlikely( N32_NO_ERROR != ( rRet= io_FileBuffer.Init( uFileSize + i_vctrCommandLine.size() ) ) ) )
		{
			file.close();
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}

		file.seekg( 0, ios::beg );
		//file.read( io_FileBuffer.getBufferAddr(), uFileSize );
		file.read( io_FileBuffer.getObjectAddress(), uFileSize );
		file.close();

		if ( unlikely( N32_NO_ERROR != ( rRet= io_FileBuffer.addBufferSize( uFileSize ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}

		if ( unlikely( N32_NO_ERROR != ( rRet= io_FileBuffer.AppendStringFromOffset( uFileSize, i_vctrCommandLine ) ) ) )
		{
			DEB( DEB_WARN, "Err: 0x%x\n", rRet );
			return( rRet );
		}

		//md5_wrapper_c md5( io_FileBuffer );
		md5_wrapper_c md5( io_FileBuffer.getObjectAddress(), io_FileBuffer.getObjectLength() );
		MD5_hash_t Hash;
		md5.DoCalculateMD5();
		md5.getHashValue( Hash );
		md5.getHexValue( o_CalculatedHexValue );

		DEB( DEB_LOG, "MD5: '%s'\n", o_CalculatedHexValue );

		// here code should go TODO
		// plik + linia komend + toolset

		rRet= N32_NO_ERROR;
	}
	else
	{
		rRet= N32_ERR_UNKNOWN;
	}

	return( rRet );
}
