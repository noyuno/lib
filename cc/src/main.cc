#include "util.h"
#include <algorithm>

using namespace std;

int main(int argc, char** argv)
{
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
    assert(util::u8tou32(a) == a32);
    assert(util::u32tou8(a32) == a8);
    assert(util::charstou32(a) == a32);
    assert(util::c32tou8(b32) == b8);
    assert(util::twidth(b32) == 2);
    assert(util::twidth(d32) == 1);
    assert(util::twidth(e32) == 2);
    assert(util::twidth(f32) == 1);
    assert(util::endian() == true);
    
    vector<u32string> c32test = util::split<char32_t>(a32, U"\n\n");
    assert(c32.size() == c32test.size());
    assert(c32[2] == c32test[2]);
    
    assert(util::startswith<char32_t>(a32, U"abcde\n\n"));
    return 0;
}
