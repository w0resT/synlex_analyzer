#pragma once
#include <map>
#include "tokens.h"
#include "fnvhash.h"

enum syntax_type {
	NONE = 0,
	S,
	F,
	T,
	E
};

class c_analyzer {
public:
	struct syntax_tree {
		syntax_tree( ) { m_level = 0; m_name = ""; };
		syntax_tree( unsigned __int8 _lvl, std::string _name ) { m_level = _lvl; m_name = _name; };

		unsigned __int8 m_level;
		std::string m_name;
	};

	c_analyzer( ) : file_exist( false ), path( "C:\\study" ), in_scope( false ), m_level( 0 ) { }
	~c_analyzer( ) { }

	void check_file( );
	void analyze_file( );
	void syntax_analyze( );

	void insert( std::string name, tokens::token_type type = tokens::TT_NONE );
	tokens::token_t look_up( const char* name, unsigned __int8 _level = 0 );

	std::vector< tokens::token_t > get_all_tokens( ) { return m_tokens; }
	std::vector< syntax_tree > get_syntax_tree( ) { return m_syntax_tree; }

private:
	void create_file( );

	void initialize_scope( );
	void finalize_scope( );
	bool collision( fnv_t hash );

	unsigned int do_F( std::vector< tokens::token_t >& t, unsigned int _offset );
	unsigned int do_T( std::vector< tokens::token_t >& t, unsigned int _offset );
	unsigned int do_E( std::vector< tokens::token_t >& t, unsigned int _offset );

	bool file_exist;
	std::string path;

	std::vector< tokens::token_t > m_tokens;
	std::map< unsigned int, std::vector< tokens::token_t >> m_tokens_by_line;

	bool in_scope;
	unsigned __int8 m_level;
	unsigned int m_line_counter;
	std::map<fnv_t, unsigned __int8> m_add_counter;
	std::map<fnv_t, std::vector<tokens::token_t>> m_collision_props;
	std::map<fnv_t, tokens::token_t> m_props;
	std::map<unsigned __int8, std::map<fnv_t, tokens::token_t>> m_level_props;

	// syntax analyze stuff
	std::vector<syntax_tree> m_syntax_tree;
	unsigned __int8 m_syn_level;
};