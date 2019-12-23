
//          Copyright Michael Florian Hava 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "sqlite.h"
#include <cwc/bundle.hpp>

namespace sample {
	sqlite3::sqlite3(cwc::string_ref path) {
		if(std::string tmp{path}; sqlite3_open(tmp.c_str(), &db)) {
			tmp = "could not open DB (";
			tmp += sqlite3_errmsg(db);
			tmp += ")";
			sqlite3_close(db);
			throw std::runtime_error{tmp};
		}
	}

	sqlite3::~sqlite3() noexcept {
		assert(db);
		sqlite3_close(db);
	}

	void sqlite3::execute(cwc::string_ref sql, cwc::array_ref<const entry> args, const control_handler & callback) const {
		const struct prepared_stmt {
			prepared_stmt(::sqlite3 * db, cwc::string_ref sql) {
				if(sqlite3_prepare(db, sql.data(), static_cast<int>(sql.size()), &ptr, nullptr)) {
					sqlite3_finalize(ptr);
					//TODO: add error message
					throw std::runtime_error{"could not prepare statement"};
				}
			}
			~prepared_stmt() noexcept { sqlite3_finalize(ptr); }

			void bind(int index, double d) const { if(sqlite3_bind_double(ptr, index, d)) throw std::runtime_error{"could not bind double"}; }
			void bind(int index, long long l) const { if(sqlite3_bind_int64(ptr, index, l)) throw std::runtime_error{"could not bind long"}; }
			void bind(int index, cwc::string_ref s) const { if(sqlite3_bind_text(ptr, index, s.data(), static_cast<int>(s.size()), SQLITE_STATIC)) throw std::runtime_error{"could not bind string"}; }
			void bind(int index, cwc::array_ref<const uint8_t> b) const { if(sqlite3_bind_blob(ptr, index, b.data(), static_cast<int>(b.size()), SQLITE_STATIC)) throw std::runtime_error{"coult not bind blob"}; }

			auto step() const -> bool {
				const auto result{sqlite3_step(ptr)};
				if(result == SQLITE_ROW) return true;
				if(result == SQLITE_DONE) return false;
				//TODO: just assume error
				throw std::runtime_error{"an error occured"}; //TODO: add error message
			}

			auto column_count() const noexcept -> int { return sqlite3_data_count(ptr); }

			auto get(int index) const -> entry {
				switch(sqlite3_column_type(ptr, index)) {
					case SQLITE_INTEGER: return sqlite3_column_int64(ptr, index);
					case SQLITE_FLOAT:   return sqlite3_column_double(ptr, index);
					case SQLITE_TEXT:    return cwc::string_ref{reinterpret_cast<const cwc::utf8 *>(sqlite3_column_text(ptr, index)), static_cast<std::size_t>(sqlite3_column_bytes(ptr, index))};
					case SQLITE_BLOB:    return cwc::array_ref{reinterpret_cast<const cwc::uint8 *>(sqlite3_column_blob(ptr, index)), static_cast<std::size_t>(sqlite3_column_bytes(ptr, index))};
					default: throw std::runtime_error{"unknown type detected"};
				}
			}
		private:
			sqlite3_stmt * ptr{nullptr};
		} stmt{db, sql};

		{//argument binding
			auto index{1};//could be init-range-for in C++20
			for(auto & arg : args) { 
				arg.visit([&](auto val) { stmt.bind(index, val); });
				++index;
			}
		}

		{//execute and invoke callback
			std::vector<entry> entries;
			while(stmt.step()) {
				const auto size{stmt.column_count()};
				assert(size >= 0);
				entries.resize(size);
				for(int index{0}; index < size; ++index) entries[index] = stmt.get(index);
				if(!callback(entries)) break;//TODO: try-catch
			}
		}
	}
}

namespace {
	//exporting sqlite as implementation of file
	struct sqlite : cwc::component_implementation<sqlite, cwc::sample::sqlite::file>, private ::sample::sqlite3 {
		//export member functions to component_implementation
		using ::sample::sqlite3::sqlite3;
		using ::sample::sqlite3::execute;
	};
}

CWC_EXPORT_COMPONENT(cwc::sample::sqlite::file, sqlite);