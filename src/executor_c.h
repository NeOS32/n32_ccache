#ifndef EXECUTOR_C_H
#define EXECUTOR_C_H

#include <Windows.h>
#include <string>
#include <n32_base.h>

using namespace std;

class executor_c
{

private:
	executor_c(const executor_c& rhs);
	executor_c& operator=(const executor_c& rhs);

public:
	executor_c();
	result_t	ExecuteCommand( const string & i_Command, N32_BOOL i_bInheritHandles = N32_TRUE, N32_BOOL i_bWaitForReturn = N32_TRUE ) const;
	virtual 	~executor_c();

};

#endif // EXECUTOR_C_H
