
//          Copyright Michael Florian Hava 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "sqlite.h"
#include <cwc/bundle.hpp>

namespace sample {
	sqlite::sqlite(cwc::string_ref path) {
		const std::string tmp{path};
		if(sqlite3_open(tmp.c_str(), &db)) {
			std::string tmp{sqlite3_errmsg(db)};
			sqlite3_close(db);
			throw std::runtime_error{"could not open DB (" + tmp + ")"};
		}
	}

	sqlite::~sqlite() noexcept {
		assert(db);
		sqlite3_close(db);
	}

	//TODO: there is no need for 32bit INT as internally only signed 64bit is supported anyways (no way to extract 32bit automatically)
	//TODO: ther si no need for 32bit FLOAT as internally only 64bit FLOAT is supported
	void sqlite::execute(cwc::string_ref sql, cwc::array_ref<const entry> args, const handler & callback) const {
		const std::string tmp{sql};
		sqlite3_stmt * stmt{nullptr};
		if(sqlite3_prepare(db, tmp.c_str(), -1, &stmt, nullptr)) {
			sqlite3_finalize(stmt);
			throw std::runtime_error{"could not prepare statement"};
		}

		{//argument binding
			auto index{1};
			for(auto & arg : args) {
				arg.visit(
					[&](float f) { if(sqlite3_bind_double(stmt, index, f)) throw std::runtime_error{"could not bind double"}; },
					[&](int i) { if(sqlite3_bind_int(stmt, index, i)) throw std::runtime_error{"could not bind int"}; },
					[&](long long l) { if(sqlite3_bind_int64(stmt, index, l)) throw std::runtime_error{"could not bind long"}; },
					[&](cwc::string_ref s) {
						const std::string tmp{s};
						const auto error{sqlite3_bind_text(stmt, index, tmp.c_str(), -1, SQLITE_TRANSIENT)};
						if(error) {
							throw std::runtime_error{sqlite3_errmsg(db)};
						}
					},
					[&](cwc::array_ref<const uint8_t> b) {
						//TODO
					}
				);
				++index;
			}
		}

		auto result{SQLITE_OK};
		{//execute and invoke callback
			std::vector<entry> entries;
			while((result = sqlite3_step(stmt)) == SQLITE_ROW) {
				const auto size{sqlite3_data_count(stmt)};
				assert(size >= 0);
				entries.resize(size);
				for(int index{0}; index < size; ++index) {
					const auto type{sqlite3_column_type(stmt, index)};
					switch(type) {
						case SQLITE_INTEGER: entries[index] = sqlite3_column_int64(stmt, index); break;
						case SQLITE_FLOAT:   entries[index] = static_cast<float>(sqlite3_column_double(stmt, index)); break;
						case SQLITE_TEXT:    entries[index] = cwc::string_ref{reinterpret_cast<const cwc::utf8 *>(sqlite3_column_text(stmt, index))}; break;
						case SQLITE_BLOB: /*TODO*/ break;
					}

				}
				callback(entries);//TODO: try-catch
			}

		}
		sqlite3_finalize(stmt);
		if(result != SQLITE_DONE) throw std::runtime_error{"an error occured"};
	}
}

namespace {
	//exporting sqlite as implementation of file
	struct sqlite : cwc::component_implementation<sqlite, cwc::sample::sqlite::file>, private ::sample::sqlite {
		//export member functions to component_implementation
		using ::sample::sqlite::sqlite;
		using ::sample::sqlite::execute;
	};
}

CWC_EXPORT_COMPONENT(cwc::sample::sqlite::file, sqlite);