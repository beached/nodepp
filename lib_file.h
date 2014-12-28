#pragma once

#include "base_event_emitter.h"
#include "base_stream.h"
#include "base_write_buffer.h"
#include "semaphore.h"

namespace daw {
	namespace nodepp {
		namespace lib {	
			namespace file {
				namespace impl {
					class FileImpl;
				}	// namespace impl

				using LibFile = std::shared_ptr < impl::FileImpl > ;

				enum class FileOpenMode { ReadOnly, WriteOnly, ReadAndWrite };

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

						base::data_t read( );
						base::data_t read( std::size_t bytes );

						FileImpl& write_async( base::data_t const & chunk );
						FileImpl& write_async( boost::string_ref chunk, base::Encoding const & encoding = base::Encoding( ) );

					private:

						static void handle_read( std::weak_ptr<FileImpl> obj, std::shared_ptr<boost::asio::streambuf> read_buffer, boost::system::error_code const & err, std::size_t const & bytes_transfered );
						static void handle_write( std::weak_ptr<daw::thread::Semaphore<int>> outstanding_writes, std::weak_ptr<FileImpl> obj, base::write_buffer buff, boost::system::error_code const & err, size_t const & bytes_transfered );

					};	// class LibFileImpl

				}	// namespace impl
			}	// namespace file
		}	// namespace lib
	}	// namespace nodepp
}	// namespace daw
