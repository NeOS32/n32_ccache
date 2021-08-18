// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

#ifndef STORED_OBJ_C_H
#define STORED_OBJ_C_H

#include <n32_base.h>
#include <string>
#include <Windows.h>

#include "file_buffer_c.hpp"

using namespace std;

#define FIRST_LEVEL_DEPTH	( 2 )
#define SECOND_LEVEL_DEPTH	( 2 )

class stored_obj_c
{

private:
	file_buffer_c<char> 		m_FileBuffer;

	//	stored_obj_c& operator=(const stored_obj_c& rhs); singleton?

public:
	stored_obj_c(uAL32Bits i_uBufferSize = 0x0);
	char* getObjectAddress(void) const;
	uAL32Bits 			getObjectLength(void) const;
	result_t			Init(uAL32Bits i_uBufferSize = 0x0);
	result_t			addBufferSize(uAL32Bits i_uElementsToAdd);
	result_t			AppendStringFromOffset(uAL32Bits i_uElementsToSkip, const string& i_DataToBeAppended);
	result_t			tryToReadObjFromCache(const string& i_rObjectToBeLoaded);
	result_t 			ReadGivenFile(const string& i_rFileName);
	result_t 			WriteGivenFile(const string& i_rFileName);
	virtual 			~stored_obj_c();

};

result_t getEnvVarValue(const string& i_VariableName, string& o_VariableValue);
result_t dirCreateIfDoesntExist(LPCTSTR i_szPath);
N32_BOOL dirExists(LPCTSTR i_szPath);

#endif // STORED_OBJ_C_H
