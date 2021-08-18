// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

/*
 * C++ string_tokenizer_c
 *
 * String tokenizer. Not efficient, can be done more efficient.
 */

#include "string_tokenizer_c.h"

#include <vector>
#include <string>
#include <sstream>


string_tokenizer_c::string_tokenizer_c(const string& i_StringToBeTokenized, N32_BOOL i_bAreDuplicatesAllowed)
	:m_bAreDuplicatesAllowed(i_bAreDuplicatesAllowed)
{
	DoProcessGivenString(i_StringToBeTokenized);
}

string_tokenizer_c::string_tokenizer_c() : m_bAreDuplicatesAllowed(N32_FALSE)
{
}

string_tokenizer_c::string_tokenizer_c(const string_tokenizer_c& rhs)
	: m_bAreDuplicatesAllowed(N32_FALSE)
{
}

string_tokenizer_c::~string_tokenizer_c()
{
}

const string& string_tokenizer_c::getTokenByNumber(TokenIndex& i_uTokenNumber) const
{
	return(m_vctrTokens[i_uTokenNumber]);
}

//void Tokenize(const string& str,             vector<string>& tokens,              const string& delimiters = " "){}
result_t string_tokenizer_c::DoProcessGivenString(const string& i_StringToBeTokenized, const string& i_rDelimiters)
{
	string strToken; // temporrary token
	stringstream strStream(i_StringToBeTokenized); // Insert the string into a stream
	TokenIndex iIndex = 0x0;
	pair<map<string, TokenIndex>::iterator, bool> ret;

	// first of all we're creating a copy of orginal command line
	m_strOrgCommandLine = i_StringToBeTokenized;
	//strcpy( &*m_vctrOrgCommandLine.begin(), i_StringToBeTokenized.c_str() );

	// Skip i_rDelimiters at beginning.
	string::size_type lastPos = i_StringToBeTokenized.find_first_not_of(i_rDelimiters, 0);

	// Find first "non-delimiter".
	string::size_type pos = i_StringToBeTokenized.find_first_of(i_rDelimiters, lastPos);

	while (string::npos != pos || string::npos != lastPos)
	{
		strToken = i_StringToBeTokenized.substr(lastPos, pos - lastPos);
		// Found a token, add it to the vector.
		m_vctrTokens.push_back(strToken);

		ret = m_mapString2Index.insert(pair<string, TokenIndex>(strToken, iIndex));
		if (unlikely((false == ret.second) && (N32_FALSE == m_bAreDuplicatesAllowed)))
		{
			DEB(DEB_WARN, "Element: '%s:%d' is not unique\n", strToken.c_str(), iIndex);
			return(N32_ERR_ALREADY_REGISTERED);
		}

		m_mapIndex2Offset[iIndex] = lastPos;

		// Skip i_rDelimiters.  Note the "not_of"
		lastPos = i_StringToBeTokenized.find_first_not_of(i_rDelimiters, pos);

		// Find next "non-delimiter"
		pos = i_StringToBeTokenized.find_first_of(i_rDelimiters, lastPos);

		iIndex++;
	}

	return(N32_NO_ERROR);
}

TokenIndex string_tokenizer_c::getIndexOfToken(const string& i_Token, result_t& o_rRet)
{
	map<string, TokenIndex>::const_iterator it = m_mapString2Index.find(i_Token);

	if (likely(it != m_mapString2Index.end()))
	{
		o_rRet = N32_NO_ERROR;
		return(m_mapString2Index[i_Token]);
	}
	else
	{
		o_rRet = N32_ERR_NOT_FOUND;
		return(NoneExistingToken);
	}
}

TokenOffset string_tokenizer_c::getOffsetOfIndex(const TokenIndex& i_Index, result_t& o_rRet)
{
	map<TokenIndex, TokenOffset>::const_iterator it = m_mapIndex2Offset.find(i_Index);

	if (likely(it != m_mapIndex2Offset.end()))
	{
		o_rRet = N32_NO_ERROR;
		return(m_mapIndex2Offset[i_Index]);
	}
	else
	{
		DEB(DEB_WARN, "Err: 0x%x (token number %d not found)\n", N32_ERR_NOT_FOUND, i_Index);

		o_rRet = N32_ERR_NOT_FOUND;
		return(NoneExistingOffset);
	}
}

result_t string_tokenizer_c::setTokenAtIndex(const string& i_NewToken, TokenIndex io_Token)
{
	// erasing by key
	if (unlikely(0x1 != m_mapString2Index.erase(m_vctrTokens[io_Token])))
	{
		DEB(DEB_WARN, "Err: 0x%x\n", N32_ERR_NOT_FOUND);

		return(N32_ERR_NOT_FOUND);
	}

	m_vctrTokens[io_Token] = i_NewToken;

	m_mapString2Index[i_NewToken] = io_Token;

	return(N32_NO_ERROR);
}

result_t string_tokenizer_c::getCurrentString(string& o_ResultString)
{
	TokenItterator iItterator = m_vctrTokens.begin();

	o_ResultString.clear();

	iItterator++; // we're leaving first token

	if (unlikely(m_vctrTokens.end() == iItterator))
	{
		return(N32_ERR_NOT_FOUND);
	}

	for (; iItterator != m_vctrTokens.end(); ++iItterator)
	{
		o_ResultString += *iItterator;
		o_ResultString += " ";
	}

	return(N32_NO_ERROR);
}

N32_BOOL string_tokenizer_c::IsThisProperIndex(const TokenIndex& i_Token) const
{
	return(
		(i_Token < getNumberOfTokens())
		?
		N32_TRUE
		:
		N32_FALSE
		);
}

u32 string_tokenizer_c::getNumberOfTokens(void) const
{
	return(m_vctrTokens.size());
}

const string& string_tokenizer_c::getOrignalString(void) const
{
	return(m_strOrgCommandLine);
}


result_t string_tokenizer_c::getSubStringFromGivenIndex(string& o_SubString, TokenIndex& i_uTokenNumber)
{
	result_t rRet;

	TokenOffset Offset = getOffsetOfIndex(i_uTokenNumber, rRet);
	if (unlikely(N32_NO_ERROR != rRet))
	{
		DEB(DEB_WARN, "Err: 0x%x\n", rRet);
		return(rRet);
	}

	o_SubString.clear();
	o_SubString = m_strOrgCommandLine.substr(Offset);

	return(N32_NO_ERROR);
}

