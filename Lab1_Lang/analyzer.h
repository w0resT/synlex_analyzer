#pragma once
#include <map>
#include "tokens.h"
#include "fnvhash.h"

class c_analyzer {
public:
	c_analyzer( ) : file_exist( false ), path( "C:\\study" ), in_scope( false ), m_level( 0 ) { }
	~c_analyzer( ) { }

	void check_file( );
	void analyze_file( );

	void insert( std::string name, tokens::token_type type = tokens::TT_NONE );
	tokens::token_t look_up( const char *name, unsigned __int8 _level = 0 );
	
	std::vector< tokens::token_t > get_all_tokens( ) { return m_tokens; }

private:
	void create_file( );

	void initialize_scope( );
	void finalize_scope( );
	bool collision( fnv_t hash );

	bool file_exist;
	std::string path;

	std::vector< tokens::token_t > m_tokens;

	bool in_scope;
	unsigned __int8 m_level;
	std::map<fnv_t, unsigned __int8> m_add_counter;
	std::map<fnv_t, std::vector<tokens::token_t>> m_collision_props;
	std::map<fnv_t, tokens::token_t> m_props;
	std::map<unsigned __int8, std::map<fnv_t, tokens::token_t>> m_level_props;

};