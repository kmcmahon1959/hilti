
// This is a wrapper around the reporter's functionality needed by HILTI so
// that we don't need to include Bro's main reporter.h, which conflicts with
// some of the HILTI include files.

#ifndef  BRO_PLUGIN_HILTI_REPORTER_H
#define  BRO_PLUGIN_HILTI_REPORTER_H

#include <string>

class Connection;

namespace bro {
namespace hilti {
namespace reporter {

extern void         __error(const char* msg);
extern void         __internal_error(const char* msg);
extern void         __weird(Connection* conn, const char* msg);
extern void         __push_location(const char* file, int line);
extern void         __pop_location();
extern  char*       __current_location();

inline void   push_location(const std::string& file, int line) { __push_location(file.c_str(), line); }
inline void   pop_location()                                   { __pop_location(); }
inline void   error(const std::string& msg)                    { __error(msg.c_str()); }
inline void   internal_error(const std::string& msg)           { __internal_error(msg.c_str()); }
inline void   weird(Connection* conn, const std::string& msg)  { __weird(conn, msg.c_str()); }

inline std::string current_location()
	{
	char* s = __current_location();
	std::string r = s;
	free(s);
	return r;
	}

}
}
}

#endif
