
//          Copyright Michael Florian Hava 2012.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file ../../../LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#ifndef CWC_HPP_HEADER_INCLUDED
	#error CWC does not support including internal headers - include <cwc.hpp> instead
#endif

#pragma once

//TODO: add windows-headers
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

namespace cwc::internal {
	struct socket {
		socket(const socket &) =delete;
		socket(socket &&) noexcept =delete;//TODO
		auto operator=(const socket &) -> socket & =delete;
		auto operator=(socket &&) noexcept -> socket & =delete; //TODO

		~socket() noexcept { close(fd); }

		void send(const oarchive & ar) const {
			const auto & data{ar.data};
			const auto size{static_cast<std::uint32_t>(data.size())};
			//TODO: error checking is missing
			write(fd, &size, sizeof(size));
			write(fd, data.data(), data.size());
		}

		auto receive() const -> iarchive {
			std::vector<std::uint8_t> data;
			std::uint32_t size;
			//TODO: error checking is missing
			read(fd, &size, sizeof(size));
			data.resize(size);
			read(fd, data.data(), data.size());
			return data;
		}

	protected:
		socket(int fd) noexcept : fd{fd} {}

		socket() : socket{::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)} {
			using namespace std::string_literals;
			if(fd == -1) exception("socket");
		}

		[[noreturn]]
		static
		void exception(const char * step) {
			using namespace std::string_literals;
			throw std::runtime_error{step + " failed: "s + std::strerror(errno)};
		}

		const int fd;
	};

	struct client_socket final : socket {
		client_socket(const std::string & ip, std::uint16_t port) {
			sockaddr_in sa;
			std::memset(&sa, 0, sizeof(sa));
			sa.sin_family = AF_INET;
			sa.sin_port = htons(port);
			using namespace std::string_literals;
			if(inet_pton(AF_INET, ip.data(), &sa.sin_addr) != 1) exception("pton");
			if(connect(fd, reinterpret_cast<sockaddr *>(&sa), sizeof(sa)) == -1) exception("connect");
		}

		~client_socket() noexcept {
			if(shutdown(fd, SHUT_RDWR) == -1) exception("shutdown");
		}
	private:
		friend
		struct server_socket;

		client_socket(int fd) noexcept : socket{fd} {}
	};

	struct server_socket final : socket {
		server_socket(std::uint16_t port) {
			sockaddr_in sa;
			std::memset(&sa, 0, sizeof(sa));
			sa.sin_family = AF_INET;
			sa.sin_port = htons(port);
			sa.sin_addr.s_addr = htonl(INADDR_ANY);

			using namespace std::string_literals;
			if(bind(fd, reinterpret_cast<sockaddr *>(&sa), sizeof(sa)) == -1) exception("bind");
			if(listen(fd, 10) == -1) exception("listen");
		}

		auto accept() const -> client_socket {
			const auto tmp{::accept(fd, NULL, NULL)};
			if(tmp < 0) exception("accept");
			return tmp;
		}
	};
}
