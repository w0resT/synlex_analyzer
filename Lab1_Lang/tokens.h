#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <filesystem>

namespace tokens {
	enum token_type {
		TT_NONE = 0,
		TT_ID,
		TT_KEYWORD,			// 'for', 'do'
		TT_OPERATION_SIGN,	// '<', '>', '==', '='
		TT_NUMBER,
		TT_COMMENT,			// '//'
		TT_OTHER,			// ';', '(', ')'
		TT_ERROR
	};

	enum comparison_op {
		COMP_NONE = 0,
		COMP_EQUAL,			// '=='
		COMP_GREATER,		// '>'
		COMP_LESS			// '<'
	};

	enum assignment_op {
		ASSIGN_NONE = 0,
		ASSIGN_DIRECT		// '='
	};

	struct token_t {
		token_t( ) { token = "";  basic_type = TT_NONE; }

		std::string token;
		token_type basic_type;
	};

	bool is_dict( token_type type );

	std::vector<token_t> get_tokens( std::string str );
	std::string type_to_str( token_type tt );
}
