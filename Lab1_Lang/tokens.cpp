#include "tokens.h"
#include "analyzer.h"
#include "misc.h"

std::vector< std::string > dict_comments = {
	"//",
	"/*",
	"*/"
};

std::vector< std::string > dict_keywords = {
	"for",
	"do"
};

std::vector< std::string > dict_operation_sign = {
	"<",
	">",
	"==",
	"=",
	"+"
};

std::vector< std::string > dict_other = {
	";",
	"(",
	")",
	"{",
	"}",
	"++"
};

bool tokens::is_dict( token_type type ) {
	return ( type == TT_COMMENT || type == TT_KEYWORD 
			 || type == TT_OPERATION_SIGN || type == TT_OTHER);
}

std::vector<tokens::token_t> tokens::get_tokens( std::string str ) {
	std::vector<token_t> vec_tokens;

	if ( str.empty( ) ) {
		return vec_tokens;
	}

	auto is_roman_numerals = [ ]( char c ) -> bool {
		return c == 'X' || c == 'V' || c == 'I';
	};

	token_t current_token;

	/*
	* TODO:
		Actually we can use
			current_token.token = dc;
		insted of
			current_token.token = str.substr( pos, str.size( ) );
	*/

	// Keywords?
	for ( const auto& dw : dict_keywords ) {
		size_t pos = str.find( dw );
		if ( pos != std::string::npos && pos == 0 && str[ pos + dw.size( ) ] == ' ' ) {
			current_token.basic_type = tokens::TT_KEYWORD;
			current_token.token = str.substr( pos, dw.size( ) );

			vec_tokens.push_back( current_token );

			str.erase( str.begin( ), str.begin( ) + dw.size( ) );
		}
	}

	// Comments?
	for ( const auto& dc : dict_comments ) {
		while ( str.find( dc ) != std::string::npos ) {
			size_t pos = str.find( dc );
			if ( pos != std::string::npos ) {
				current_token.basic_type = tokens::TT_COMMENT;
				current_token.token = str.substr( pos, str.size( ) );

				vec_tokens.push_back( current_token );

				str.erase( str.begin( ) + pos, str.end( ) );
			}
		}
	}

	// Other symbols? :  '(', ')', ';' 
	for ( const auto& dother : dict_other ) {
		while ( str.find( dother ) != std::string::npos ) {
			size_t pos = str.find( dother );
			if ( pos != std::string::npos ) {
				current_token.basic_type = tokens::TT_OTHER;
				current_token.token = str.substr( pos, dother.size( ) );

				vec_tokens.push_back( current_token );

				str.erase( str.begin( ) + pos, str.begin( ) + pos + dother.size( ) );
			}
		}
	}

	// Operation signs
	for ( const auto& dos : dict_operation_sign ) {
		while ( str.find( dos ) != std::string::npos ) {
			size_t pos = str.find( dos );
			if ( pos != std::string::npos ) {
				current_token.basic_type = tokens::TT_OPERATION_SIGN;
				current_token.token = str.substr( pos, dos.size( ) );

				vec_tokens.push_back( current_token );

				str.replace( str.begin( ) + pos, str.begin( ) + pos + dos.size( ), " " );
				//str.erase( str.begin( ) + pos, str.begin( ) + pos + dos.size( ) );
			}
		}
	}

	// Numbers 0-9
	for ( size_t i = 0; !str.empty( ) && i < str.size( ); ++i ) {
		auto iter = std::find_if( str.cbegin( ), str.cend( ), isdigit );
		if ( iter != std::end( str ) ) {
			auto pos = std::distance( str.cbegin( ), iter );
			size_t pos_dest = 0;

			while ( isdigit( str[ pos + pos_dest ] ) ) {
				if ( pos + pos_dest >= str.size( ) ) {
					break;
				}

				pos_dest++;
			}

			if ( str[ pos + pos_dest ] != ' ' && !isdigit( str[ pos + pos_dest ] ) ) {
				// Error
				break;
			}

			current_token.basic_type = tokens::TT_NUMBER;
			current_token.token = str.substr( pos, pos_dest );

			vec_tokens.push_back( current_token );

			str.erase( str.begin( ) + pos, str.begin( ) + pos + pos_dest );
		}
		else { // No more numbers
			break;
		}
	}

	// Roman numerals X V I
	for ( size_t i = 0; !str.empty( ) && i < str.size( ); ++i ) {
		auto iter = std::find_if( str.cbegin( ), str.cend( ), is_roman_numerals );
		if ( iter != std::end( str ) ) {
			auto pos = std::distance( str.cbegin( ), iter );

			// Wrong number
			if ( pos != 0 && str[ pos - 1 ] != ' ' ) {
				continue;
			}

			size_t pos_dest = 0;
			while ( is_roman_numerals( str[ pos + pos_dest ] ) ) {
				if ( pos + pos_dest >= str.size( ) ) {
					break;
				}

				pos_dest++;
			}

			// Wrong number again
			if ( pos + pos_dest < str.size( ) && str[ pos + pos_dest ] != ' ' ) {
				continue;
			}

			current_token.basic_type = tokens::TT_NUMBER;
			current_token.token = str.substr( pos, pos_dest );

			vec_tokens.push_back( current_token );

			str.erase( str.begin( ) + pos, str.begin( ) + pos + pos_dest );

		}
		else { // No more roman numerals
			break;
		}
	}

	// IDs. a-z, A-Z, '_' with a letter
	for ( size_t i = 0; !str.empty( ) && i < str.size( ); ++i ) {
		bool any_letters = false;
		// First char cannot be a number
		if ( isalpha( str[ i ] ) || str[ i ] == '_' ) {
			size_t pos_dest = 0;
			while ( isalnum( str[ i + pos_dest ] ) || str[ i + pos_dest ] == '_' ) {
				if ( i + pos_dest >= str.size( ) ) {
					break;
				}

				// Need something with '_'
				if ( !any_letters && isalnum( str[ i + pos_dest ] ) ) {
					any_letters = true;
				}

				pos_dest++;
			}

			// Thats can't be only '_'
			if ( !any_letters ) {
				continue;
			}

			current_token.basic_type = tokens::TT_ID;
			current_token.token = str.substr( i, pos_dest );

			vec_tokens.push_back( current_token );

			str.replace( str.begin( ) + i, str.begin( ) + i + pos_dest, " " );
			//str.erase( str.begin( ) + i, str.begin( ) + i + pos_dest );
		}
		else if ( str[i] == ' ' ) {
			continue;
		}
		else {
			// Check if its last word
			auto sub_str = str.substr( i, str.size( ) );
			auto pos = sub_str.find( ' ' );
			if ( pos != std::string::npos ) {
				i += pos;
				continue;
			}

			// Number as a first char and its not a number => error type
			break;
		}
	}

	// No matches, so just throw error
	for ( size_t i = 0; !str.empty( ) && i < str.size( ); ++i ) {
		// Removing spaces
		while ( str.find( ' ' ) == 0 ) {
			str.erase( str.find( ' ' ), 1 );
		}

		if ( str.empty( ) ) {
			break;
		}

		// Pos - str[0], dest - before space
		auto pos_dest = str.find( ' ' );
		if ( pos_dest != std::string::npos ) {
			current_token.basic_type = tokens::TT_ERROR;
			current_token.token = str.substr( 0, pos_dest );

			vec_tokens.push_back( current_token );

			str.erase( str.begin( ) , str.begin( ) + pos_dest );
		}
		else {
			current_token.basic_type = tokens::TT_ERROR;
			current_token.token = str.substr( i, str.size( ) );

			vec_tokens.push_back( current_token );

			str.erase( str.begin( ), str.end( ) );
		}
	}

	return vec_tokens;
}

std::string tokens::type_to_str( token_type tt ) {
	switch ( tt ) {
	case TT_NONE:
		return "None";
		break;
	case TT_ID:
		return "ID";
		break;
	case TT_KEYWORD:
		return "Keyword";
		break;
	case TT_OPERATION_SIGN:
		return "Operation sign";
		break;
	case TT_NUMBER:
		return "Number";
		break;
	case TT_COMMENT:
		return "Comment";
		break;
	case TT_OTHER:
		return "Other";
		break;
	case TT_ERROR:
		return "Error";
		break;
	default:
		return "Unknown type";
		break;
	}
}