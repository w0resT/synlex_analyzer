#pragma once
#include <cstring>

using fnv_t = unsigned;
class FnvHash {
	static const fnv_t FNV_PRIME = 16777619u;
	static const fnv_t OFFSET_BASIS = 2166136261u;

	static fnv_t fnvHash( const char* str, unsigned __int8 _level = 0 ) {
		const auto length = strlen( str ) + 1;
		auto hash = OFFSET_BASIS + _level;
		for ( size_t i = 0; i < length; ++i ) {
			hash ^= *str++;
			hash *= FNV_PRIME;
		}
		return hash;
	}

	fnv_t hash_value;
public:
	FnvHash( std::string name, unsigned __int8 _level = 0 ) : hash_value( fnvHash( name.c_str(), _level ) ) { }

	constexpr operator fnv_t( ) const { return this->hash_value; }
};