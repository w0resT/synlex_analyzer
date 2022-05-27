#pragma once

#define Assert(_exp) misc::_Assert(#_exp, _exp, __FILE__, __FUNCTION__, __LINE__)

namespace misc {
	static void _Assert( const char* expr_str, bool expr, const char* file, const char* func, int line/*, const char* msg*/ ) {
		if ( !expr ) 	{
			std::stringstream ss;
			ss << "[error] assert failed:\n"
				<< "[error] expected:\t" << expr_str << "\n"
				<< "[error] source:\t\t" << file << ", func: " << func << ", line: " << line;

			throw std::runtime_error( ss.str( ).c_str( ) );
		}
	}
};