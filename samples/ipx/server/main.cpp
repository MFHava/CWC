#include <numeric>
#include <iostream>
#include <cwc/cwc.hpp>

int main() {
	cwc::internal::server_socket socket{1100};

	for(;;) {
		const auto connection{socket.accept()};

		cwc::array_ref<const std::uint16_t> input;

		const auto ipacket{connection.receive()};
		cwc::internal::iarchive iar{ipacket};
		iar & input;

		std::cout << "input: ";
		for(const auto & elem : input) std::cout << elem << ", ";
		std::cout << std::endl;

		std::vector<std::uint16_t> result;
		std::partial_sum(std::begin(input), std::end(input), std::back_inserter(result));

		std::vector<std::uint8_t> opacket;
		cwc::internal::oarchive oar{opacket};
		oar & cwc::array_ref<const std::uint16_t>{result};
		connection.send(opacket);
	}
}
