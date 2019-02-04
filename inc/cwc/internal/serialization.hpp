
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

//simple serialization framework
//  syntax inspired by Boost.Serialization
//  design simplified for usecase as on-the-wire-format for IPX => no versioning, inheritance, etc.

#define CWC_SERIALIZATION\
	friend\
	struct cwc::internal::serializer;\
private:\
	\
	template<typename Archive>\
	void cwc_serialization(Archive & ar)

namespace cwc::internal {
	template<typename Type>
	struct serializer final {
		template<typename Archive>
		static
		void do_(Type & val, Archive & ar) { val.cwc_serialization(ar); }
	};

	class oarchive final {
		struct obuffer final : std::streambuf {
			obuffer(std::vector<std::uint8_t> & buffer) : buffer{buffer} { setp(reinterpret_cast<char *>(buffer.data()), 0); }

			auto overflow(int_type c) -> int_type override {
				assert(c != EOF);
				buffer.push_back(static_cast<std::uint8_t>(c));
				return c;
			}
		private:
			std::vector<std::uint8_t> & buffer;
		};

		obuffer buffer;
		std::ostream os{&buffer};
	public:
		oarchive(std::vector<std::uint8_t> & target) : buffer{target} {
			os.exceptions(std::ios::badbit | std::ios::eofbit | std::ios::failbit);
		}

		template<typename Type>
		void operator&(const Type *) =delete;

		template<typename Type>
		auto operator&(const Type & val) -> std::enable_if_t<std::is_fundamental_v<Type>> { os.write(reinterpret_cast<const char *>(&val), sizeof(Type)); }

		template<typename Type>
		auto operator&(const Type & val) -> std::enable_if_t<std::is_enum_v<Type>> { *this & static_cast<std::underlying_type_t<Type>>(val); }

		template<typename Type>
		auto operator&(const Type & val) -> std::enable_if_t<std::is_class_v<Type>> { serializer<Type>::do_(const_cast<Type &>(val), *this); }

		template<typename Type, std::size_t Size>
		void operator&(const cwc::array<Type, Size> & val) { for(const auto & elem : val) *this & elem; }

		template<typename Type>
		void operator&(const cwc::array_ref<Type> & val) {
			*this & val.size();
			for(const auto & elem : val) *this & elem;
		}
#if 0//TODO
		template<std::size_t Size>
		void operator&(const cwc::bitset<Size> & val) {
			constexpr std::size_t bytes{(Size + 7) / 8};
			//TODO: index is strongly monotonic and should not be computed every time
			for(std::size_t byte{0}; byte < bytes; ++byte) {
				std::uint8_t word{0};
				for(std::size_t bit{0}; bit < 8; ++bit) {
					const auto index{byte * 8 + bit};
					if(index >= Size) break;
					if(val.test(index)) word |= (1U << bit);
				}
				*this & word;
			}
		}
#endif
		template<typename Type>
		void operator&(const cwc::optional<Type> & val) {
			*this & static_cast<bool>(val);
			if(val) *this & val;
		}

		void operator&(const cwc::string_ref & val) {
			*this & val.size();
			for(const auto & elem : val) *this & elem;
		}
#if 0//TODO
		template<typename... Types>
		void operator&(const cwc::tuple<Types...> & val) =delete;//TODO
#endif
		template<typename... Types>
		void operator&(const cwc::variant<Types...> & val) {
			assert(!val.valueless_by_exception());//TODO: how to handle this?!
			val.visit([&](const auto & val) {
				constexpr auto index{find_v<std::decay_t<decltype(val)>, Types...>};
				static_assert(index != internal::not_found);
				*this & index;
				*this & val;
			});
		}
	};

	class iarchive final {
		struct ibuffer final : std::streambuf {
			ibuffer(const std::vector<std::uint8_t> & buffer) : buffer{buffer} {
				const auto ptr{const_cast<char *>(reinterpret_cast<const char *>(buffer.data()))};
				setg(ptr, ptr, ptr + buffer.size());
			}
		private:
			const std::vector<std::uint8_t> & buffer;
		};

		ibuffer buffer;
		std::istream is{&buffer};
		std::vector<boost::shared_ptr<void>> cache;//TODO: better name

		template<typename Head, typename... Tail, typename... Types>
		void construct_in_variant_from_index(cwc::variant<Types...> & val, std::uint8_t index) {//TODO: if this works - ptl::internal::visit can be vastly simplified
			if(index == 0) val.template emplace<Head>();
			else {
				if constexpr(sizeof...(Tail) == 0) throw std::bad_variant_access{};
				else construct_in_variant_from_index<Tail...>(val, index - 1);
			}
		}
	public:
		iarchive(const std::vector<std::uint8_t> & source) : buffer{source} { is.exceptions(std::ios::badbit | std::ios::eofbit | std::ios::failbit); }

		template<typename Type>
		void operator&(Type *) =delete;

		template<typename Type>
		auto operator&(Type & val) -> std::enable_if_t<std::is_fundamental_v<Type>> { is.read(reinterpret_cast<char *>(&val), sizeof(Type)); }

		template<typename Type>
		auto operator&(Type & val) -> std::enable_if_t<std::is_enum_v<Type>> { *this & static_cast<std::underlying_type_t<Type>>(val); }

		template<typename Type>
		auto operator&(Type & val) -> std::enable_if_t<std::is_class_v<Type>> { serializer<Type>::do_(val, *this); }

		template<typename Type, std::size_t Size>
		void operator&(cwc::array<Type, Size> & val) { for(auto & elem : val) *this & elem; }

		template<typename Type>
		void operator&(cwc::array_ref<Type> & val) {
			std::size_t size;
			*this & size;
			if(!size) val = {};
			else {
				auto ptr{boost::make_shared<std::remove_const_t<Type>[]>(size)};//TODO: using boost as GCC has no support for C++17-style smartpointer
				for(std::size_t i{0}; i < size; ++i) *this & ptr[i];
				val = {ptr.get(), size};
				cache.push_back(ptr);
			}
		}
#if 0//TODO
		template<std::size_t Size>
		void operator&(cwc::bitset<Size> & val) {
			constexpr std::size_t bytes{(Size + (CHAR_BIT - 1)) / CHAR_BIT};
			//TODO: index is strongly monotonic and should not be computed every time
			val.reset();
			for(std::size_t byte{0}; byte < bytes; ++byte) {
				std::uint8_t word;
				*this & word;
				for(std::size_t bit{0}; bit < 8; ++bit) {
					const auto index{byte * 8 + bit};
					if(index >= Size) break;
					if(word & (1U << bit)) val.set(index);
				}
			}
		}
#endif
		template<typename Type>
		void operator&(cwc::optional<Type> & val) {
			val.reset();
			bool valid;
			*this & valid;
			if(valid) {
				val.emplace();
				*this & *val;
			}
		}

		void operator&(cwc::string_ref & val) =delete;//TODO
#if 0//TODO
		template<typename... Types>
		void operator&(cwc::tuple<Types...> & val) =delete;//TODO
#endif
		template<typename... Types>
		void operator&(cwc::variant<Types...> & val) {
			std::uint8_t index;
			*this & index;
			construct_in_variant_from_index<Types...>(val, index);
			val.visit([&](auto & val) { *this & val; });
		}
	};
}
