#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "base_event_emitter.h"
#include "base_enoding.h"

namespace daw {
	namespace nodepp {
		namespace base {
			class Data {
				using storage_t = std::vector < uint8_t > ;
				using iterator_t = storage_t::iterator;
				std::vector<uint8_t> m_data;
				iterator_t first;				
			public:


				template<typename T>
				static T convert_to( std::vector<uint8_t> const & ) {
					return T{ };	// TODO
				}
				enum class DataType { binary, string };
				bool is_string( bool throw_if_not = false ) const;
				bool is_binary( bool throw_if_not = false ) const;
				
				explicit operator std::string( ) const {
					is_string( );
				//	return convert_to < std::string( m_data );
				}

				explicit operator std::wstring( ) const {
					is_string( );
					return convert_to<std::wstring>( m_data );
				}


			};	// class Data

		}	// namespace base
	}	// namespace nodepp
}	// namespace daw
