// SPDX-FileCopyrightText: 2011 Sebastian Serewa <neos32.project@gmail.com>
//
// SPDX-License-Identifier: Apache-2.0

#include "cmndline_c.h"
#include "string_tokenizer_c.h"

cmndline_c::cmndline_c(const string& i_FullCommandLine) : string_tokenizer_c(i_FullCommandLine)
{
}

cmndline_c::cmndline_c()
{
}

cmndline_c::~cmndline_c()
{
}
