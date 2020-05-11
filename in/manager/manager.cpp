/**
 */
////////////////////////////////////////////////////////////////////////////////
// Includes
////////////////////////////////////////////////////////////////////////////////
#include "../../cweb.hpp"

////////////////////////////////////////////////////////////////////////////////
// Private Structs
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Private Functions Head
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Private Functions Inline
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Private Functions
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Construct Functions
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Public Functions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Private Variables
////////////////////////////////////////////////////////////////////////////////
static const char* (*_read) (const char*);

////////////////////////////////////////////////////////////////////////////////
// Interface
////////////////////////////////////////////////////////////////////////////////
bool
cweb::in::init()
{
	char *rm = getenv("REQUEST_METHOD");
	if(rm == NULL) {
		MError("CWEB::IN - getenv(\"REQUEST_METHOD\") is NULL");
		return false;
	}
	
	// descobre qual o REQUEST_METHOD passado
	if(strcmp(rm, "GET") == 0) {
		_read = cweb::in::get;
		return cweb::in::init_get();;
	}
	else if(strcmp(rm, "POST") == 0) {
		_read = cweb::in::post;
		return cweb::in::init_post();
	}
	else {
		Error("CWEB::IN - REQUEST_METHOD do not recognize.\n"
			"REQUEST_METHOD = \"%s\"", rm);
	}
	
	return false;
}

const char*
cweb::in::read(const char *key)
{
	return _read(key);
}










