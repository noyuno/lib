#include "lib.h"
#include "property.h"
#include <algorithm>

using namespace std;
using namespace lib;

void test_string();
void test_property();

int main(int argc, char** argv)
{
    test_string();
    //test_property();
    return 0;
}

void test_string() {
    // 15 codepoints
    // UTF-8 29 bytes
    // UTF-16 36 bytes
    const char *a = "abcde\n\nあいう\n𠀋𡈽𡌛\n\n𡑮𡢽";
    string a8 = "abcde\n\nあいう\n𠀋𡈽𡌛\n\n𡑮𡢽";
    u16string a16 = u"abcde\n\nあいう\n𠀋𡈽𡌛\n\n𡑮𡢽";
    u32string a32 = U"abcde\n\nあいう\n𠀋𡈽𡌛\n\n𡑮𡢽";
    string b8 = "𠀋";
    char32_t b32 = U'𠀋';
    char32_t d32 = U'b';
    char32_t e32 = U'が';
    char32_t f32 = U'ｱ';
    vector<u32string> c32;
    c32.push_back(U"abcde");
    c32.push_back(U"あいう\n𠀋𡈽𡌛");
    c32.push_back(U"𡑮𡢽");
    assert(u8tou32(a) == a32);
    assert(u32tou8(a32) == a8);
    assert(charstou32(a) == a32);
    assert(c32tou8(b32) == b8);
    assert(twidth(b32) == 2);
    assert(twidth(d32) == 1);
    assert(twidth(e32) == 2);
    assert(twidth(f32) == 1);
    assert(endian() == true);
    
    vector<u32string> c32test = split<char32_t>(a32, U"\n\n");
    assert(c32.size() == c32test.size());
    assert(c32[2] == c32test[2]);
    
    assert(startswith(a32, U"abcde\n\n"));
    assert(startswith(u"abcde\n\n", u"abcde\n\n"));
    assert(!startswith("abcde\n\n", "be\n\n"));
    assert(startswith(a32, u8tou32(a8)));
}

class person {
private:
    string _name;
    int _age;
    int _agesetter(const int& v) {
        return _age = v;
    };
    int _agegetter() {
        return _age;
    };
    int _birthday;
public:
    wproperty<string> name;
    wproperty<int> age;
    rproperty<int> birthday;

    person() : name(_name), age(_age), birthday(_birthday) {
        _birthday = 123;
    }
};


void test_property() {
    person adam;
    adam.name = "Adam";
    adam.age  = 20;
    // error because birthday is read-only property
    //adam.birthday = 234;
    assert((string)adam.name == "Adam");
    assert((int)adam.age == 20);
    assert((int)adam.birthday == 123);
}

