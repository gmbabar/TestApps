#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

int main() {
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    std::cout << uuid << std::endl;
}