#include <iostream>
#include <smallstring/smallstring.hpp>

int main() {
    std::string test = "1234";
    smallstring::Buffer buff;
    buff.append(test);
    buff.append("other_thing", 12);
    smallstring::Buffer buff2;
    buff2 = std::move(buff);
    std::cout << buff2.view() << std::endl;
    buff2.clear();
    buff2.append("test", 4);
    std::cout << buff2.view() << std::endl;
    return 0;
}