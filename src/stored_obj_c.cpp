// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

/*
 * C/C++ stored_obj_c
 *
 * The class representing an object that is stored somewhere (can be disc, ram etc).
 * Also, some helping functions.
 *
 */

#include "stored_obj_c.h"

#include <string>
#include <vector>
#include <sstream>

#include <Windows.h>
 //#include <shellapi.h>
 //#include <direct.h>
#include <sys/stat.h>

N32_BOOL dirExists(LPCTSTR i_szPath)
{
	DWORD dwAttrib = GetFileAttributes(i_szPath);

	if (likely((dwAttrib != INVALID_FILE_ATTRIBUTES) && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
	{
		return(N32_TRUE);
	}
	else
	{
		return(N32_FALSE);
	}
}

result_t dirCreateIfDoesntExist(LPCTSTR i_szPath)
{
	if (N32_FALSE == dirExists(i_szPath))
	{
		if (0x0 == CreateDirectory(i_szPath, NULL))
		{
			DEB(DEB_WARN, "Err: 0x%x\n", N32_ERR_EXTERNAL);
			return(N32_ERR_EXTERNAL);
		}
	}

	return(N32_NO_ERROR);
}

result_t getEnvVarValue(const string& i_VariableName, string& o_VariableValue)
{
#define MAX_VAR_LENGTH ( 32768 )
	o_VariableValue.clear();

	vector<char> vctrVarBuffer;

	vctrVarBuffer.reserve(MAX_VAR_LENGTH);

	if (unlikely(0x0 == GetEnvironmentVariable(
		i_VariableName.c_str(),
		&*vctrVarBuffer.begin(),
		MAX_VAR_LENGTH)))
	{
		DEB(DEB_WARN, "Err: 0x%x\n", N32_ERR_EXTERNAL);
		return(N32_ERR_EXTERNAL);
	}

	o_VariableValue += &*vctrVarBuffer.begin();

	return(N32_NO_ERROR);
}

stored_obj_c::stored_obj_c(uAL32Bits i_uBufferSize) :m_FileBuffer(i_uBufferSize)
{
}

stored_obj_c::~stored_obj_c()
{
}

result_t stored_obj_c::Init(uAL32Bits i_uBufferSize)
{
	result_t rRet;

	if (unlikely(N32_NO_ERROR != (rRet = m_FileBuffer.Init(i_uBufferSize))))
	{
		DEB(DEB_WARN, "Err: 0x%x\n", rRet);
		return(rRet);
	}

	return(rRet);
}

char* stored_obj_c::getObjectAddress(void) const
{
	return(m_FileBuffer.getBufferAddr());
}

uAL32Bits stored_obj_c::getObjectLength(void) const
{
	return(m_FileBuffer.getBufferSize());
}

result_t stored_obj_c::ReadGivenFile(const string& i_rFileName)
{
	ifstream file(i_rFileName.c_str(), ios::in | ios::binary | ios::ate);

	if (likely(file.is_open()))
	{
		result_t rRet;
		ifstream::pos_type uFileSize = file.tellg();

		if (unlikely(N32_NO_ERROR != (rRet = m_FileBuffer.Init(uFileSize))))
		{
			file.close();
			DEB(DEB_WARN, "Err: 0x%x\n", rRet);
			return(rRet);
		}

		file.seekg(0, ios::beg);
		file.read(m_FileBuffer.getBufferAddr(), uFileSize);
		file.close();

		if (unlikely(N32_NO_ERROR != (rRet = m_FileBuffer.addBufferSize(uFileSize))))
		{
			file.close();
			DEB(DEB_WARN, "Err: 0x%x\n", rRet);
			return(rRet);
		}

		return(N32_NO_ERROR);
	}

	return(N32_ERR_UNKNOWN);
}

result_t stored_obj_c::WriteGivenFile(const string& i_rFileName)
{
	ofstream file(i_rFileName.c_str(), ios::out | ios::binary);

	if (likely(file.is_open()))
	{
		file.write(m_FileBuffer.getBufferAddr(), m_FileBuffer.getBufferSize());
		file.close();

		return(N32_NO_ERROR);
	}

	return(N32_ERR_UNKNOWN);
}

result_t stored_obj_c::tryToReadObjFromCache(const string& i_rObjectToBeLoaded)
{
	result_t rRet;
	string strCCacheDir;

	if (unlikely(N32_NO_ERROR != (rRet = getEnvVarValue("N32_CCACHE_DIR", strCCacheDir))))
	{
		if (likely(N32_NO_ERROR != (rRet = getEnvVarValue("TEMP", strCCacheDir))))
		{
			DEB(DEB_WARN, "Err: 0x%x\n", rRet);
			return(rRet);
		}
	}

	strCCacheDir += '\\';

	if (unlikely(N32_NO_ERROR != (rRet = dirCreateIfDoesntExist(strCCacheDir.c_str()))))
	{
		DEB(DEB_WARN, "Err: 0x%x\n", rRet);
		return(rRet);
	}

	char DirPrefix[FIRST_LEVEL_DEPTH + SECOND_LEVEL_DEPTH + 2 + 1]; // 1 for \0, 2 for \\ (backslashes)
	memcpy(&DirPrefix[0], &i_rObjectToBeLoaded[0], FIRST_LEVEL_DEPTH);
	DirPrefix[FIRST_LEVEL_DEPTH] = '\\';
	DirPrefix[FIRST_LEVEL_DEPTH + 1] = 0x0;

	string strDirectory = strCCacheDir + DirPrefix;
	if (unlikely(N32_NO_ERROR != (rRet = dirCreateIfDoesntExist(strDirectory.c_str()))))
	{
		DEB(DEB_WARN, "Err: 0x%x\n", rRet);
		return(rRet);
	}

	memcpy(&DirPrefix[FIRST_LEVEL_DEPTH + 1], &i_rObjectToBeLoaded[FIRST_LEVEL_DEPTH], FIRST_LEVEL_DEPTH);
	DirPrefix[FIRST_LEVEL_DEPTH + SECOND_LEVEL_DEPTH + 1] = '\\';
	DirPrefix[FIRST_LEVEL_DEPTH + SECOND_LEVEL_DEPTH + 2] = 0x0;

	strDirectory = strCCacheDir + DirPrefix;
	if (unlikely(N32_NO_ERROR != (rRet = dirCreateIfDoesntExist(strDirectory.c_str()))))
	{
		DEB(DEB_WARN, "Err: 0x%x\n", rRet);
		return(rRet);
	}

	strCCacheDir = strDirectory + i_rObjectToBeLoaded;

	struct stat buf;

	/* Get data associated with "stat.c": */
	int result = stat(strCCacheDir.c_str(), &buf);

	/* Check if statistics are valid: */
	if (unlikely((0x0 != result) || (0x0 == buf.st_size)))
	{
		// cached file doesn't exist
		DEB(DEB_LOG, "Err: 0x%x (Cache file not found - leaving)\n", N32_ERR_NOT_FOUND);
		return(N32_ERR_NOT_FOUND);
	}
	else
	{
		// cached file exists!
		if (unlikely(N32_NO_ERROR != (rRet = ReadGivenFile(strCCacheDir))))
		{
			DEB(DEB_WARN, "Err: 0x%x\n", rRet);
			return(rRet);
		}

		/* Output some of the statistics: */
		/*printf( "File size : %ld\n", buf.st_size );
		printf( "Drive : %c:\n", buf.st_dev + 'A' );
		printf( "Time modified : %s", ctime( &buf.st_mtime ) );*/
	}

	return(N32_NO_ERROR);
}

result_t stored_obj_c::AppendStringFromOffset(uAL32Bits i_uElementsToSkip, const string& i_DataToBeAppended)
{
	return(m_FileBuffer.AppendStringFromOffset(i_uElementsToSkip, i_DataToBeAppended));
}

result_t stored_obj_c::addBufferSize(uAL32Bits i_uElementsToAdd)
{
	return(m_FileBuffer.addBufferSize(i_uElementsToAdd));
}
