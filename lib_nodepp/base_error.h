// The MIT License (MIT)
//
// Copyright (c) 2014-2016 Darrell Wright
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files( the "Software" ), to
// deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once

#include <boost/utility/string_ref.hpp>
#include <boost/system/error_code.hpp>
#include <map>
#include <memory>

namespace daw {
	namespace nodepp {
		namespace base {
			using ErrorCode = boost::system::error_code;

			//////////////////////////////////////////////////////////////////////////
			// Summary:		Contains key/value pairs describing an error condition.
			//				Description is mandatory.
			// Requires:
			class Error final: public std::exception {
				std::map<std::string, std::string> m_keyvalues;
				bool m_frozen;
				std::shared_ptr<Error> m_child;
				std::exception_ptr m_exception;
			public:
				Error( ) = delete;
				~Error( );
				explicit Error( boost::string_ref description );
				explicit Error( ErrorCode const & err );
				Error( Error const & ) = default;
				Error( Error && ) = default;
				Error& operator=( Error const & ) = default;
				Error& operator=( Error && ) = default;
				Error( boost::string_ref description, std::exception_ptr ex_ptr );

				Error& add( boost::string_ref name, boost::string_ref value );
				boost::string_ref get( boost::string_ref name ) const;
				std::string & get( boost::string_ref name );
				Error & child( ) const;
				bool has_child( ) const;
				Error& clear_child( );
				Error& child( Error child );
				void freeze( );
				bool has_exception( ) const;
				void throw_exception( );
				std::string to_string( boost::string_ref prefix = "" ) const;
			};	// class Error

			std::ostream& operator<<( std::ostream& os, Error const & error );

			using OptionalError = std::shared_ptr <Error>;

			//////////////////////////////////////////////////////////////////////////
			/// Summary:	Create a null error (e.g. no error)
			OptionalError create_optional_error( );

			//////////////////////////////////////////////////////////////////////////
			/// Summary:	Create an error item
			template<typename... Args>
			OptionalError create_optional_error( Args&&... args ) {
				auto err = new Error( std::forward<Args>( args )... );
				return OptionalError( err );
			}
		}	// namespace base
	}	// namespace nodepp
}	// namespace daw

