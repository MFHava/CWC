#include <numeric>
#include <iostream>
#include <cwc/cwc.hpp>

int main() {
	cwc::internal::server_socket socket{1100};

	for(;;) {
		const auto connection{socket.accept()};

		cwc::array_ref<const std::uint16_t> input;

		cwc::internal::iarchive iar{connection.receive()};
		iar & input;

		std::cout << "input: ";
		for(const auto & elem : input) std::cout << elem << ", ";
		std::cout << std::endl;

		std::vector<std::uint16_t> result;
		std::partial_sum(std::begin(input), std::end(input), std::back_inserter(result));

		cwc::internal::oarchive oar;
		oar & cwc::array_ref<const std::uint16_t>{result};
		connection.send(oar);
	}
}
