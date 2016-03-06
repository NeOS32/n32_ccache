#ifndef CMND_LINE_C_H
#define CMND_LINE_C_H

#include <Windows.h>
#include <fstream>
#include <shellapi.h>
#include "string_tokenizer_c.h"


class cmndline_c {

	string_tokenizer_c StringTokens;
	
	cmndline_c& operator=(const cmndline_c& rhs);

public:
	cmndline_c();
	cmndline_c( const LPTSTR i_FullCommandLine );
	const string & getFirstCommand( void ) const;
	virtual ~cmndline_c();

};

#endif // CMND_LINE_C_H
