#include <numeric>
#include <iostream>
#include <iterator>
#include <cwc/cwc.hpp>

int main() {
	cwc::internal::client_socket socket{"192.168.0.14", 1100};

	std::vector<std::uint16_t> input(100);
	std::iota(std::begin(input), std::end(input), 0);

	cwc::internal::oarchive oar;
	oar & cwc::array_ref<const std::uint16_t>{input};
	socket.send(oar);

	cwc::array_ref<const std::uint16_t> result;

	cwc::internal::iarchive iar{socket.receive()};
	iar & result;

	std::cout << "result: ";
	for(const auto & elem : result) std::cout << elem << ", ";
	std::cout << std::endl;
}
