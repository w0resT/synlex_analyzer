#include "analyzer.h"
#include "fnvhash.h"
#include "misc.h"
#include <iostream>

//#define ANOTHER_METHOD

void c_analyzer::check_file( ) {
	std::string cur_path( path );
	std::fstream file( cur_path.append( "\\lab1\\code.txt" ) );

	if ( !file.good( ) ) {
		create_file( );
	}
	else {
		file_exist = true;
	}
}

void c_analyzer::analyze_file( ) {
	// File doesn't exist
	if ( !file_exist ) {
		return;
	}

	std::string cur_path( path );
	std::ifstream file( cur_path.append( "\\lab1\\code.txt" ).c_str( ) );
	if ( !file.is_open( ) ) {
		return;
	}

	std::string current_line;
	while ( std::getline( file, current_line ) ) {
		auto cur_tokens = tokens::get_tokens( current_line );

		// Перед тем, как заносить, определяем очередность. В текущей строке ищем все токены с самого начала,
		// начинаем добавлять те, у которых позиция 0, пробелы из строки после удалени убираем

		while ( !cur_tokens.empty() ) 	{
			for ( size_t i = 0; i < cur_tokens.size( ); ++i ) {
				auto sc = current_line.find( ' ' );
				while ( sc != std::string::npos && sc == 0 ) {
					current_line.erase( current_line.begin( ) );
					sc = current_line.find( ' ' );
				}

				auto &cur_elem = cur_tokens[ i ];

				auto pos = current_line.find( cur_elem.token );
				if ( pos != std::string::npos && pos == 0 ) {
					m_tokens.push_back( cur_elem );
					current_line.erase( current_line.begin( ), current_line.begin( ) + cur_elem.token.size( ) );
					cur_tokens.erase( cur_tokens.begin() + i );
				}
			}
		}
	}
}

void c_analyzer::initialize_scope( ) {
	m_level++;

	if ( !in_scope ) {
		in_scope = true;
	}
}

// TODO: this func must delete used ids
// m_level_props[level] del and after dec the counter
void c_analyzer::finalize_scope( ) {
	if ( in_scope ) {
		Assert( m_level );

		// Clearing current scope level data
		//m_level_props[ m_level ].clear( );

#ifdef ANOTHER_METHOD
		// Need to clear this scope level collision data
		//m_collision_props[ hash ].pop_back()
#endif

		m_level--;

		if ( m_level == 0 ) {
			in_scope = false;
		}
	}
}

tokens::token_t c_analyzer::look_up( const char *name, unsigned __int8 _level ) {
	tokens::token_t ret;

	static fnv_t hash;
	hash = FnvHash( name, _level );

	auto& _props = m_level_props[ _level ];
	Assert( !_props.empty( ) );

#ifdef ANOTHER_METHOD
	ret = m_collision_props[ hash ].back( );
	//m_collision_props[ hash ].pop_back()
#else
	if ( m_add_counter[ hash ] ) {
		hash = FnvHash( name, _level + m_add_counter[ hash ] );
	}

	ret = _props[ hash ];
#endif // ANOTHER_METHOD

	
	Assert( !ret.token.empty() );

	return ret;
}

void c_analyzer::insert( std::string name, tokens::token_type type ) {
	// Detect "{ }" and use initialize_scope
	// if ( type == tokens::TT_OTHER ||  name == '{' ) {
	initialize_scope( );
	// }

	tokens::token_t tt{ };
	tt.basic_type = type;
	tt.token = name;

	// Need to cast it to prevent FnvHash using the recursive hasher
	// which would hash all 256 bytes
	auto hash = FnvHash( name.c_str(), m_level );

#ifndef ANOTHER_METHOD
	check_collision:
#endif
	if ( collision( hash ) && !tokens::is_dict(type) && type != tokens::TT_NUMBER) {

#ifdef ANOTHER_METHOD
		m_collision_props[ hash ].push_back( tt );
#ifdef _DEBUG
		std::cout << "[debug] collision: " << name << ", hash: " << hash << ", index: " << m_collision_props[hash].size() - 1 << std::endl;
#endif // _DEBUG
#else
		auto _old_hash = hash;
		m_add_counter[ _old_hash ]++;
		hash = FnvHash( name.c_str( ), m_level + m_add_counter[ _old_hash ] ); // using m_add_counter[ _old_hash ] to get new hash
#ifdef _DEBUG
		std::cout << "[debug] collision: " << name << ", hash: " << _old_hash << ", new hash: " << hash << std::endl;
#endif // _DEBUG

		// While we have collison
		// TODO: do this with a 'while' instead of 'goto'
		goto check_collision;
#endif // ANOTHER_METHOD
	}

	m_props[ hash ] = tt;

	m_level_props[ m_level ] = m_props;
		
	// if ( type == tokens::TT_OTHER ||  name == '}' ) {
	finalize_scope( );
	// }
}

bool c_analyzer::collision( fnv_t hash ) {
	auto prop_it = m_props.find( hash );

	// If not .end() then we already have this hash
	if ( prop_it != m_props.end( ) ) {
		// We can check prop level
		// current_level == founded_prop_level => a1 will be ignored and rewrited so its ok
		// but if current_level > or < founded_prop_level => collision
		// then we need to fix it:
		// 1. inc FNV_PRIME in hash func by every level and dec when leaving scope
		// 2. change the name of this var (in m_tokens as well). For example var1 -> var1S1, where S1 - its the current scope level

		// Right now thats more than enough for us bcs we don't have scope levels except of 1 (0).
		return true;
	}

	return false;
}

void c_analyzer::create_file( ) {
	std::string cur_path( path );
	if ( std::filesystem::create_directory( cur_path.append( "\\lab1" ) ) ) {
		std::fstream file;
		file.open( cur_path.append( "\\code.txt" ), std::ios_base::in | std::ios_base::out | std::ios_base::trunc );
		file_exist = file.good( ) ? true : false;

		file.close( );
	}
	else {
		file_exist = false;
	}
}
