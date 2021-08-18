// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

#ifndef __N32_STRING_TOKENIZER_C_H
#define __N32_STRING_TOKENIZER_C_H

#include <vector>
#include <string>
#include <sstream>
#include <map>
#include <n32_base.h>

using namespace std;

typedef vector<string>::iterator	TokenItterator;
typedef int				TokenIndex;
typedef int				TokenOffset;
typedef vector<string>::const_iterator	cTokenIndex;

const TokenIndex			NoneExistingToken = -1;
const TokenOffset			NoneExistingOffset = -1;

class string_tokenizer_c
{

private:
	vector<string>			m_vctrTokens;
	map<string, TokenIndex>		m_mapString2Index;
	map<TokenIndex, TokenOffset>	m_mapIndex2Offset;
	string				m_strOrgCommandLine;
	N32_BOOL			m_bAreDuplicatesAllowed;

	string_tokenizer_c(const string_tokenizer_c& rhs);
	string_tokenizer_c& operator=(const string_tokenizer_c& rhs);

public:
	string_tokenizer_c(const string& i_StringToBeTokenized, N32_BOOL i_bAreDuplicatesAllowed = N32_FALSE);
	string_tokenizer_c();
	result_t		DoProcessGivenString(const string& i_StringToBeTokenized, const string& i_rDelimiters = " ");
	const string& getTokenByNumber(TokenIndex& i_uTokenNumber) const;
	TokenIndex		getIndexOfToken(const string& i_Token, result_t& o_rRet);
	TokenOffset		getOffsetOfIndex(const TokenIndex& i_Index, result_t& o_rRet);
	result_t		getCurrentString(string& o_ResultString);
	const string& getOrignalString(void) const;
	result_t		getSubStringFromGivenIndex(string& o_SubString, TokenIndex& i_uTokenNumber);
	u32			getNumberOfTokens(void) const;
	result_t		setTokenAtIndex(const string& i_NewToken, TokenIndex io_Token);
	N32_BOOL		IsThisProperIndex(const TokenIndex& i_Token) const;
	virtual ~string_tokenizer_c();

};

#endif // __N32_STRING_TOKENIZER_C_H
