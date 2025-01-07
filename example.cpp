#include <iostream>
#include <smallstring/smallstring.hpp>

int main() {

    // by default buffer has capacity of 256 bytes,but you could do
    // smallstring::Buffer buff(another_initial_capacity)
    smallstring::Buffer buff;

    std::string test = "1234";
    buff.push(test);
    buff.push("567_8910");
    std::cout << buff.view() << std::endl; // will print 1234567_8910
    buff.pop(4);
    std::cout << buff.view() << std::endl; // will print 567_8910
    buff.pop(buff.find("_8910"));
    std::cout << buff.view() << std::endl; // will print _8910
    buff.clear();
    buff.push(std::string_view(test));
    std::cout << buff.view() << std::endl; // will print 1234
    return 0;
}