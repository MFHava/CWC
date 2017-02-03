#include <iterator>
#include <algorithm>
#include "utils.hpp"

namespace cwcc {
	auto remove_double_colon(std::string s) -> std::string {
		s.erase(std::unique(std::begin(s), std::end(s), [](char c1, char c2) { return c1 == c2 && c1 == ':'; }), std::end(s));
		return s;
	}

	auto base_file_name(std::string s) -> std::string {
		s = remove_double_colon(std::move(s));
		std::replace(std::begin(s), std::end(s), ':', '.');
		return s;
	}
}