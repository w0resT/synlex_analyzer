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

	m_line_counter = 0;
	std::vector< tokens::token_t > current_line_tokens;

	std::string current_line;
	while ( std::getline( file, current_line ) ) {
		auto cur_tokens = tokens::get_tokens( current_line );

		// ѕеред тем, как заносить, определ€ем очередность. ¬ текущей строке ищем все токены с самого начала,
		// начинаем добавл€ть те, у которых позици€ 0, пробелы из строки после удалени убираем
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
					current_line_tokens.push_back( cur_elem );
					current_line.erase( current_line.begin( ), current_line.begin( ) + cur_elem.token.size( ) );
					cur_tokens.erase( cur_tokens.begin() + i );
				}
			}
		}

		if ( !current_line_tokens.empty( ) ) {
			m_tokens_by_line[ m_line_counter++ ] = current_line_tokens;
		}

		current_line_tokens.clear( );
	}
}

/*
* TODO:
* Change thrd_elem etc for "next_elem". Rn its just for the better debugging.
*/
unsigned int c_analyzer::do_F( std::vector<tokens::token_t> &t, unsigned int _offset ) {
	unsigned int j = _offset;
	unsigned int return_j = j;

	auto& cur_elem = t.at( j );
	auto cur_type = cur_elem.basic_type;
	auto& cur_token = cur_elem.token;

	// ¬ конце фукнции текущий уровень должен быть равен уровню до начала, 
	// т.е. мы прошли весь уровень и вернулись туда же
	auto old_syn_level = m_syn_level;

	if ( cur_type == tokens::TT_KEYWORD) {
		m_syntax_tree.push_back( syntax_tree( m_syn_level, "F" ) );
		m_syn_level++;

		if ( cur_token == "for" ) {
			m_syntax_tree.push_back( syntax_tree( m_syn_level, cur_token ) ); // push_back ( "for" )

			// Need to look for the next token. 
			// Must be "(" - TT_OTHER
			Assert( j + 1 < t.size( ) );
			auto &next_elem = t.at( j + 1 );
			if ( next_elem.basic_type == tokens::TT_OTHER && next_elem.token == "(" ) {
				m_syntax_tree.push_back( syntax_tree( m_syn_level, next_elem.token ) ); // push_back ( "(" )

				auto cur_j = do_T( t, j + 1 );

				// Checking for ")" symbol
				Assert( cur_j + 1 < t.size( ) );
				auto &another_elem = t.at( cur_j + 1 );
				if ( another_elem.basic_type == tokens::TT_OTHER && another_elem.token == ")" ) {
					m_syntax_tree.push_back( syntax_tree( m_syn_level, another_elem.token ) ); // push_back ( ")" )

					// Now we need to check if we have "do" 
					Assert( cur_j + 2 < t.size( ) );
					if ( t.at( cur_j + 2 ).basic_type == tokens::TT_KEYWORD && t.at( cur_j + 2 ).token == "do" ) {
						m_syntax_tree.push_back( syntax_tree( m_syn_level, t.at(cur_j + 2).token ) ); // push_back ( "do" )

						// Need to do +1 to get next elem
						return_j = do_F( t, cur_j + 3 );

						// Ќет выражени€ => ошибка
						/*if ( return_j == cur_j + 3 ) {
							Assert( !"Expected F" );
						}*/
					}
					else {
#ifdef _DEBUG
						std::cout << "[debug] unexpected token!" << std::endl;
						std::cout << "[debug] token: " << t.at( cur_j + 2 ).token << ", type: " << type_to_str( t.at( cur_j + 2 ).basic_type ) << ", level: " << static_cast< int >( m_syn_level ) << std::endl;
#endif // _DEBUG
						Assert( !"Expected 'do'" );
					}
				}
				else {
#ifdef _DEBUG
					std::cout << "[debug] unexpected token!" << std::endl;
					std::cout << "[debug] token: " << another_elem.token << ", type: " << type_to_str( another_elem.basic_type ) << ", level: " << static_cast< int >( m_syn_level ) << std::endl;

#endif // _DEBUG
					Assert( false );
				}
			}
			else {
#ifdef _DEBUG
				std::cout << "[debug] unexpected token!" << std::endl;
				std::cout << "[debug] token: " << next_elem.token << ", type: " << type_to_str( next_elem.basic_type ) << ", level: " << static_cast< int >( m_syn_level ) << std::endl;

#endif // _DEBUG
				Assert( false );
			}
		}
		else {
#ifdef _DEBUG
			std::cout << "[debug] unexpected token!" << std::endl;
			std::cout << "[debug] token: " << cur_token << ", type: " << type_to_str( cur_type ) << ", level: " << static_cast< int >( m_syn_level ) << std::endl;

#endif // _DEBUG
			Assert( false );
		}

		// ”ходим из текущего уровн€
		m_syn_level--;
	}
	else if ( cur_type == tokens::TT_ID ) {
		m_syntax_tree.push_back( syntax_tree( m_syn_level, "F" ) );
		m_syn_level++;

		m_syntax_tree.push_back( syntax_tree( m_syn_level, cur_token ) ); // push_back ( a )

		// Looking for the next token
		Assert( j + 1 < t.size( ) );
		if ( t.at( j + 1 ).basic_type == tokens::TT_OPERATION_SIGN) {
			m_syntax_tree.push_back( syntax_tree( m_syn_level, t.at( j + 1 ).token ) ); // push_back ( "=" )

			Assert( j + 2 < t.size( ) );
			if ( t.at( j + 2 ).basic_type == tokens::TT_ID || t.at( j + 2 ).basic_type == tokens::TT_NUMBER ) {
				m_syntax_tree.push_back( syntax_tree( m_syn_level, t.at( j + 2 ).token ) ); // push_back ( a )

				return_j = j + 2;
			}
			else {
#ifdef _DEBUG
				std::cout << "[debug] unexpected token!" << std::endl;
				std::cout << "[debug] token: " << t.at( j + 2 ).token << ", type: " << type_to_str( t.at( j + 2 ).basic_type ) << ", level: " << static_cast< int >( m_syn_level ) << std::endl;

#endif // _DEBUG
				Assert( false );
			}
		}
		else {
#ifdef _DEBUG
			std::cout << "[debug] unexpected token!" << std::endl;
			std::cout << "[debug] token: " << t.at( j + 1 ).token << ", type: " << type_to_str( t.at( j + 1 ).basic_type ) << ", level: " << static_cast< int >( m_syn_level ) << std::endl;

#endif // _DEBUG
			Assert( false );
		}

		// ”ходим из текущего уровн€
		m_syn_level--;
	}
	else {
#ifdef _DEBUG
		std::cout << "[debug] unexpected token!" << std::endl;
		std::cout << "[debug] token: " << cur_token << ", type: " << type_to_str( cur_type ) << ", level: " << static_cast< int >( m_syn_level ) << std::endl;
		
#endif // _DEBUG
		Assert( false );
	}

	// ћы не вернулись на прежний уровень => где-то ошибка
	Assert( old_syn_level == m_syn_level );

	return return_j;
}

unsigned int c_analyzer::do_T( std::vector<tokens::token_t>& t, unsigned int _offset ) {
	unsigned int return_i = _offset;

	unsigned int i = _offset + 1;
	Assert( i < t.size( ) );

	m_syntax_tree.push_back( syntax_tree( m_syn_level, "T" ) );
	m_syn_level++;

	// ¬ конце фукнции текущий уровень должен быть равен уровню до начала, 
	// т.е. мы прошли весь уровень и вернулись туда же
	auto old_syn_level = m_syn_level;

	// —начала запускаем F, тогда останутс€ только последние 2 случа€
	if ( t.at( i ).basic_type == tokens::TT_KEYWORD || t.at( i ).basic_type == tokens::TT_ID ) {
		auto ret_i = do_F( t, i );
		if ( ret_i != i ) {
			i = ret_i;
			i++;
		}
	}

	if ( t.at( i ).basic_type == tokens::TT_OTHER && t.at( i ).token == ";" ) {
		m_syntax_tree.push_back( syntax_tree( m_syn_level, t.at( i ).token ) ); // push_back ( ";" )

		i = do_E( t, i );

		// Next symbol must be ";'
		Assert( i + 1 < t.size( ) );
		if ( t.at( i + 1 ).basic_type == tokens::TT_OTHER && t.at( i + 1 ).token == ";" ) {
			m_syntax_tree.push_back( syntax_tree( m_syn_level, t.at( i + 1 ).token ) ); // push_back ( ";" )
			if ( t.at( i + 2 ).basic_type == tokens::TT_KEYWORD || t.at( i + 2 ).basic_type == tokens::TT_ID ) {
				return_i = do_F( t, i + 2 );
			}
			else {
				return_i = i + 1;
			}

			//// ≈сли вернули то же значение, значит ничего не изменилось.
			//// ¬озвращаем каретку обратно.
			//if ( return_i == i + 2 ) {
			//	return_i--;
			//}
		}
		else {
#ifdef _DEBUG
			std::cout << "[debug] unexpected token!" << std::endl;
			std::cout << "[debug] token: " << t.at( i + 1 ).token << ", type: " << type_to_str( t.at( i + 1 ).basic_type ) << ", level: " << static_cast< int >( m_syn_level ) << std::endl;
#endif // _DEBUG
			Assert( false );
		}

	}
	else {
#ifdef _DEBUG
		std::cout << "[debug] unexpected token!" << std::endl;
		std::cout << "[debug] token: " << t.at( i ).token << ", type: " << type_to_str( t.at( i ).basic_type ) << ", level: " << static_cast< int >( m_syn_level ) << std::endl;
#endif // _DEBUG
		Assert( false );
	}

	// ћы не вернулись на прежний уровень => где-то ошибка
	Assert( old_syn_level == m_syn_level );

	// ¬ыходим из области “, а значит уменьшаем уровень
	m_syn_level--;

	// ¬озвращаем текущее положение каретки, чтобы при выходе попали на символ конца F
	return return_i;
}

unsigned int c_analyzer::do_E( std::vector<tokens::token_t>& t, unsigned int _offset ) {
	unsigned int return_i = _offset;

	unsigned int i = _offset + 1;
	Assert( i < t.size( ) );

	auto& cur_elem = t.at( i );
	auto cur_type = cur_elem.basic_type;
	auto& cur_token = cur_elem.token;

	m_syntax_tree.push_back( syntax_tree( m_syn_level, "E" ) );
	m_syn_level++;

	// ¬ конце фукнции текущий уровень должен быть равен уровню до начала, 
	// т.е. мы прошли весь уровень и вернулись туда же
	auto old_syn_level = m_syn_level;

	// Main code
	if ( cur_type == tokens::TT_ID || cur_type == tokens::TT_NUMBER ) {
		m_syntax_tree.push_back( syntax_tree( m_syn_level, cur_token ) ); // push_back ( a )

		i++;
		Assert( i < t.size( ) );
		if ( t.at( i ).basic_type == tokens::TT_OPERATION_SIGN ) {
			m_syntax_tree.push_back( syntax_tree( m_syn_level, t.at( i ).token ) ); // push_back ( <, >, ==, = )

			i++;
			Assert( i < t.size( ) );
			if ( t.at( i ).basic_type == tokens::TT_ID || t.at( i ).basic_type == tokens::TT_NUMBER ) {
				m_syntax_tree.push_back( syntax_tree( m_syn_level, t.at( i ).token ) ); // push_back ( a )
				return_i = i;
			}
			else {
#ifdef _DEBUG
				std::cout << "[debug] unexpected token!" << std::endl;
				std::cout << "[debug] token: " << t.at( i ).token << ", type: " << type_to_str( t.at( i ).basic_type ) << ", level: " << static_cast< int >( m_syn_level ) << std::endl;
#endif // _DEBUG
				Assert( false );
			}
		}
		else {
#ifdef _DEBUG
			std::cout << "[debug] unexpected token!" << std::endl;
			std::cout << "[debug] token: " << t.at( i ).token << ", type: " << type_to_str( t.at( i ).basic_type ) << ", level: " << static_cast< int >( m_syn_level ) << std::endl;
#endif // _DEBUG
			Assert( false );
		}
	}
	else {
#ifdef _DEBUG
		std::cout << "[debug] unexpected token!" << std::endl;
		std::cout << "[debug] token: " << cur_token << ", type: " << type_to_str( cur_type ) << ", level: " << static_cast< int >( m_syn_level ) << std::endl;
#endif // _DEBUG
		Assert( false );
	}

	// ћы не вернулись на прежний уровень => где-то ошибка
	Assert( old_syn_level == m_syn_level );

	// ¬ыходим из области “, а значит уменьшаем уровень
	m_syn_level--;

	// ¬озвращаем текущее положение каретки, чтобы при выходе попали на символ конца F
	return return_i;
}

void c_analyzer::syntax_analyze( ) {
	if ( m_tokens.empty( ) ) {
		return;
	}
	
	for ( unsigned int i = 0; i < m_line_counter; ++i ) {
		auto &cur_line_tokens = m_tokens_by_line[ i ];
		m_syn_level = 0;

		m_syntax_tree.push_back( syntax_tree( m_syn_level, "S" ) );
		m_syn_level++;
		auto ret_i = do_F( cur_line_tokens, 0 );
		Assert( ret_i + 1 < cur_line_tokens.size( ) );
		if ( cur_line_tokens.at( ret_i + 1 ).token == ";" ) {
			m_syntax_tree.push_back( syntax_tree( m_syn_level, ";" ) );
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
