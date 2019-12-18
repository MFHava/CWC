
//          Copyright Michael Florian Hava 2019.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include "../cwc.sample.sqlite.cwch"

#define CWC_CONTEXT_INIT_IS_NOT_FILE
#define CWC_CONTEXT_INIT_STRING\
	"[cwc.mapping]\n"\
	"cwc::sample::sqlite::file = sample-sqlite"
#include "cwc/host.hpp"

int main() try {
	using namespace cwc::sample::sqlite;

	const cwc::intrusive_ptr<database> db{cwc::this_context()->factory<file>()->create(":memory:")}; //create in memory DB by using special sqlite-path
	db->execute(R"(
		CREATE TABLE persons (
			id INTEGER ROWID,
			first_name TEXT NOT NULL,
			last_name TEXT NOT NULL
		);
	)", {}, [](auto) { std::cerr << "this should never have been called!\n"; });

	constexpr char insert[]{R"(
		INSERT INTO persons (first_name, last_name)
		VALUES(?, ?)
	)"};

	auto neverCalledForInsert{[](auto) { std::cerr << "this should never have been called for an insert!\n"; }};

	using namespace cwc::literals;
	db->execute(insert, cwc::array{entry{"Bjarne"_sr}, entry{"Stroustrup"_sr}}, neverCalledForInsert);
	db->execute(insert, cwc::array{entry{"Herb"_sr}, entry{"Sutter"_sr}}, neverCalledForInsert);

	db->execute(R"(
		SELECT first_name, last_name
		FROM persons
	)", {}, [](auto result) {
		std::cout << cwc::get<cwc::string_ref>(result[1])  << ", " << cwc::get<cwc::string_ref>(result[0]) << std::endl;
	});
} catch(const std::exception & exc) {
	std::cerr << "ERROR: " << exc.what() << "\n";
}