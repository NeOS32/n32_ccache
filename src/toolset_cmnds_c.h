#ifndef TOOLSET_CMNDS_C_H
#define TOOLSET_CMNDS_C_H

#include <string>
#include <map>

using namespace std;

/*!
 * \brief Severity levels in NeOS32
 */
typedef enum toolset_cmnd_output_e
{
        TOOLSET_CMND_OUTPUT,
        TOOLSET_CMND_COMPILATION,
	TOOLSET_CMND_PREPROCESSING,
	TOOLSET_CMND_LINKING,
        TOOLSET_CMND_DEPENDENCY,
	TOOLSET_CMND_UNKNOWN,
	TOOLSET_MAX_NUMBER
} toolset_cmnd_output_t;

const toolset_cmnd_output_t aCmndLine_AllCommands[ 3 ]=
{
        TOOLSET_CMND_COMPILATION,
	TOOLSET_CMND_PREPROCESSING,
        TOOLSET_CMND_DEPENDENCY,
};

class toolset_cmnds_c
{
	std::map<toolset_cmnd_output_t,string> CommandsName;

	toolset_cmnds_c(const toolset_cmnds_c& rhs);
	toolset_cmnds_c& operator=(const toolset_cmnds_c& rhs);

public:
	toolset_cmnds_c();
	const string getSwitchFor( const toolset_cmnd_output_t i_eCommandName );
	virtual ~toolset_cmnds_c();
};

#endif // TOOLSET_CMNDS_C_H
