#pragma once

#include <mutex>

#include "base_event_emitter.h"
#include "base_stream.h"
#include "base_write_buffer.h"

namespace daw {
	namespace nodepp {
		namespace lib {	
			namespace file {
				namespace impl {
					class FileImpl;
				}	// namespace impl

				using LibFile = std::shared_ptr < impl::FileImpl > ;

				enum class FileOpenMode { ReadOnly, WriteOnly, ReadAndWrite };
				enum class FilePositionOffset { Begin, End };
				LibFile create_file( boost::string_ref path, FileOpenMode mode = FileOpenMode::ReadOnly, base::EventEmitter emitter = base::create_event_emitter( ) );
				

				namespace impl {
					using namespace daw::nodepp;

					class FileImpl: public base::StandardEvents<FileImpl>, base::stream::StreamReadableEvents<FileImpl>, base::stream::StreamWritableEvents < FileImpl > {
						base::EventEmitter m_emitter;

						FileImpl( boost::string_ref path, FileOpenMode mode, base::EventEmitter emitter );
					public:
						friend LibFile create_file( boost::string_ref, lib::file::FileOpenMode, base::EventEmitter );

						base::EventEmitter& emitter( );

						FileImpl&  read_async( std::shared_ptr<boost::asio::streambuf> read_buffer = nullptr );

						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Read until EoF.  It is ill-advised to call multiple
						///				async_read(...)'s without waiting for the data_received
						///				event to be emitted first
						base::data_t read_async( );

						//////////////////////////////////////////////////////////////////////////
						/// Summary:	Read byte_cound bytes in or until EoF.  It is ill-advised
						///				to call multiple async_read(...)'s without waiting for 
						///				the data_received event to be emitted first
						base::data_t read_async( std::size_t byte_count );

						FileImpl& write_async( base::data_t const & chunk );
						FileImpl& write_async( boost::string_ref chunk, base::Encoding const & encoding = base::Encoding( ) );

						FileImpl& set_position( FilePositionOffset pos_offset, int64_t offset = 0 );

					};	// class LibFileImpl

				}	// namespace impl

				base::data_t read_file( boost::string_ref path );
				void read_file_async( boost::string_ref path, std::function<void( base::OptionalError error, base::data_t data )> listener );

			}	// namespace file
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
