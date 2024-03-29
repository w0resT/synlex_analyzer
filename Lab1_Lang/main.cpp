/*
* 21.������� ���� �������� ��������� ����� �for (...; ...; ...) do ...�, 
* ���������� �������� �;�. ��������� ����� �������� ��������������, 
* ����� ��������� �<�, �>�, �==�, ������� �����, ���� ������������ �=�. 
* �������� ������� �����, ���������� �������� ������� X, V � I.
*/

#include <iostream>
#include "analyzer.h"

int main( ) {

	setlocale( LC_ALL, "Russia" );

	try {
		c_analyzer analyzer;
		analyzer.check_file( );
		analyzer.analyze_file( );

		auto tokens = analyzer.get_all_tokens( );
		for ( const auto t : tokens ) {
			std::cout << type_to_str( t.basic_type ) << " | ";
			std::cout << t.token << std::endl;

			analyzer.insert( t.token, t.basic_type );
		}

		auto test = analyzer.get_all_tokens( ).at(0); // "test"
		auto test_coll = analyzer.get_all_tokens( ).at( 2 ); // another "test"

		auto got_value = analyzer.look_up( test.token.c_str( ), 1 );
		auto got_value_coll = analyzer.look_up( test_coll.token.c_str( ), 1 );
		std::cout << "Token: " << got_value.token <<
			" | Type: " << type_to_str( got_value.basic_type ) << std::endl;

		std::cout << "Token: " << got_value_coll.token <<
			" | Type: " << type_to_str( got_value_coll.basic_type ) << std::endl;
	}
	catch ( const std::exception& ex ) {
		std::cout << "[exception]: " << std::endl;
		std::cout << ex.what( ) << std::endl;
	}

	return EXIT_SUCCESS;
}