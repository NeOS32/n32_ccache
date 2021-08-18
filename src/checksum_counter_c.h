// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

#ifndef CHECKSUM_COUNTER_C_H
#define CHECKSUM_COUNTER_C_H

#include <iostream>
#include <fstream>

#include <string>
#include <map>
#include <vector>

#include <tools/wrappers/md5_obj_c.hpp>
#include <n32_base.h>

#include "stored_obj_c.h"

using namespace std;

class checksum_counter_c
{

private:
	const string& m_rFileName;
	//checksum_counter_c(const checksum_counter_c& rhs);
	checksum_counter_c& operator=(const checksum_counter_c& rhs);

public:
	checksum_counter_c(const string& i_FileName);
	result_t DoCountCheckSum(const string& i_vctrCommandLine, stored_obj_c& io_FileBuffer, MD5_hex_value_t& o_CalculatedHexValue) const;
	virtual ~checksum_counter_c();

};

#endif // CHECKSUM_COUNTER_C_H
