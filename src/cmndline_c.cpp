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

#include "cmndline_c.h"
#include "string_tokenizer_c.h"

cmndline_c::cmndline_c( const string & i_FullCommandLine ) : string_tokenizer_c( i_FullCommandLine )
{
}

cmndline_c::cmndline_c()
{
}

cmndline_c::~cmndline_c()
{
}
