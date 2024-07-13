#include "./filtered_string_view.h"
#include <catch2/catch.hpp>
#include <iterator>
#include <limits>
#include <set>
#include <sstream>
#include <vector>

TEST_CASE("Test default_predicate for all char values") {
	for (char c = std::numeric_limits<char>::min(); c != std::numeric_limits<char>::max(); ++c) {
		REQUIRE(fsv::filtered_string_view::default_predicate(c));
	}
	REQUIRE(fsv::filtered_string_view::default_predicate(std::numeric_limits<char>::max()));
}

TEST_CASE("Test default construct") {
	auto sv = fsv::filtered_string_view{};
	REQUIRE(sv.empty());
}

TEST_CASE("Test implicit string construct") {
	std::string s1 = "unsw";
	auto s = std::string("unsw");
	auto sv = fsv::filtered_string_view{s};
	REQUIRE(sv.size() == 4);
	REQUIRE(sv.data() == s1);
}

TEST_CASE("Test implicit string construct with predicate") {
	auto s = std::string{"cat"};
	auto pred = [](const char& c) { return c == 'a'; };
	auto sv = fsv::filtered_string_view{s, pred};
	REQUIRE(sv.size() == 1);
}

TEST_CASE("Test implicit null_terminated string construct") {
	std::string s1 = "unsw";
	auto sv = fsv::filtered_string_view{"unsw"};
	REQUIRE(sv.size() == 4);
	REQUIRE(sv.data() == s1);
}
TEST_CASE("Test Copy and Move construct") {
	auto sv1 = fsv::filtered_string_view{"bulldog"};
	const auto copy = sv1;
	CHECK(copy.data() == sv1.data()); // pointers compare equal.
	const auto move = std::move(sv1);
	REQUIRE(sv1.data() == nullptr); // true: sv1's guts were moved into `move`
}

TEST_CASE("Test Copy construct") {
	auto sv1 = fsv::filtered_string_view{"hello"};
	const auto& sv2(sv1);
	REQUIRE(sv2.size() == 5);
	REQUIRE(sv1.size() == 5);
	REQUIRE(sv1.data() == sv2.data());
}

TEST_CASE("Test Move construct") {
	const char* text = "world";
	auto sv1 = fsv::filtered_string_view{"world"};
	auto sv2 = std::move(sv1);
	REQUIRE(sv2.size() == 5);
	REQUIRE(sv2.data() == text);
	REQUIRE(sv1.empty());
	REQUIRE(sv1.data() == nullptr);
}

TEST_CASE("Test copy assignment") {
	auto pred = [](const char& c) { return c == '4' || c == '2'; };
	auto fsv1 = fsv::filtered_string_view{"42 bro", pred};
	auto fsv2 = fsv::filtered_string_view{};
	fsv2 = fsv1;
	assert(fsv1 == fsv2);
}

TEST_CASE("Test move assignment") {
	auto pred = [](const char& c) { return c == '8' || c == '9'; };
	auto fsv1 = fsv::filtered_string_view{"'89 baby", pred};
	auto fsv2 = fsv::filtered_string_view{};
	fsv2 = std::move(fsv1);
	assert(fsv1.empty() && fsv1.data() == nullptr);
}

TEST_CASE("Test subscript") {
	auto pred = [](const char& c) { return c == '9' || c == '0' || c == ' '; };
	auto fsv1 = fsv::filtered_string_view{"only 90s kids understand", pred};
	REQUIRE(fsv1[2] == '0');
}

TEST_CASE("Test string type conversion") {
	auto sv = fsv::filtered_string_view("vizsla");
	auto s = static_cast<std::string>(sv);
	REQUIRE(sv.data() != s.data());
}

TEST_CASE("Test at") {
	auto vowels = std::set<char>{'a', 'A', 'e', 'E', 'i', 'I', 'o', 'O', 'u', 'U'};
	auto is_vowel = [&vowels](const char& c) { return vowels.contains(c); };
	auto sv = fsv::filtered_string_view{"Malamute", is_vowel};
	REQUIRE(sv.at(0) == 'a');
	REQUIRE(sv.at(1) == 'a');
	REQUIRE(sv.at(2) == 'u');
	REQUIRE(sv.at(3) == 'e');
}

TEST_CASE("Test at out of index") {
	auto sv = fsv::filtered_string_view{""};
	REQUIRE_THROWS_AS(sv.at(0), std::domain_error);
}

TEST_CASE("Test size") {
	auto sv = fsv::filtered_string_view{"Maltese"};
	REQUIRE(sv.size() == 7);
}

TEST_CASE("Test size after filtered") {
	auto sv = fsv::filtered_string_view{"Toy Poodle", [](const char& c) { return c == 'o'; }};
	REQUIRE(sv.size() == 3);
}

TEST_CASE("Test empty after filter") {
	auto sv = fsv::filtered_string_view{"Australian Shephard"};
	auto empty_sv = fsv::filtered_string_view{};
	REQUIRE(sv.empty() == false);
	REQUIRE(empty_sv.empty() == true);
}

TEST_CASE("Test again empty after filter") {
	auto sv = fsv::filtered_string_view{"Border Collie", [](const char& c) { return c == 'z'; }};
	REQUIRE(sv.empty() == true);
}

TEST_CASE("Test data function ignore filtered function") {
	auto s = "Sum 42";
	auto sv = fsv::filtered_string_view{s, [](const char&) { return false; }};
	CHECK(sv.data() == s);
	REQUIRE(sv.empty());
}

TEST_CASE("Test predicate function") {
	const auto print_and_return_true = [](const char&) {
		std::cout << "hi!";
		return true;
	};
	const auto s = fsv::filtered_string_view{"doggo", print_and_return_true};
	const auto& predicate = s.predicate();
	REQUIRE(predicate(char{}));
}

TEST_CASE("Test operator == and !=") {
	auto const lo = fsv::filtered_string_view{"aaa"};
	auto const hi = fsv::filtered_string_view{"zzz"};

	REQUIRE(lo != hi);
	REQUIRE_FALSE(lo == hi);
}

TEST_CASE("Test operator <=>") {
	auto const lo = fsv::filtered_string_view{"aaa"};
	auto const hi = fsv::filtered_string_view{"zzz"};
	REQUIRE(lo < hi);
	REQUIRE(lo <= hi);
	REQUIRE(lo <=> hi == std::strong_ordering::less);
	REQUIRE_FALSE(lo > hi);
	REQUIRE_FALSE(lo >= hi);
}

TEST_CASE("Test ouput <<") {
	auto fsv = fsv::filtered_string_view{"c++ > rust > java", [](const char& c) { return c == 'c' || c == '+'; }};
	std::ostringstream oss;
	oss << fsv;
	REQUIRE(oss.str() == "c++");
}

TEST_CASE("Test compose function") {
	auto best_languages = fsv::filtered_string_view{"c / c++"};
	auto vf = std::vector<fsv::filter>{[](const char& c) { return c == 'c' || c == '+' || c == '/'; },
	                                   [](const char& c) { return c > ' '; },
	                                   [](const char&) { return true; }};

	auto sv = fsv::compose(best_languages, vf);
	std::ostringstream oss;
	oss << sv;
	std::string result = oss.str();
	REQUIRE(result == "c/c++");
}

TEST_CASE("Test iter begin and end") {
	auto print_via_iterator = [](fsv::filtered_string_view const& sv) {
		std::ostringstream oss;
		std::copy(sv.begin(), sv.end(), std::ostream_iterator<char>(oss, " "));
		return oss.str();
	};

	auto fsv1 = fsv::filtered_string_view{"corgi"};
	auto result = print_via_iterator(fsv1);
	REQUIRE(result == "c o r g i ");
}

// Test iter *
TEST_CASE("Test iter *next") {
	auto fsv = fsv::filtered_string_view{"samoyed", [](const char& c) {
		                                     return !(c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u');
	                                     }};
	auto it = fsv.begin();

	REQUIRE(*it == 's');
	REQUIRE(*std::next(it) == 'm');
	REQUIRE(*std::next(it, 2) == 'y');
	REQUIRE(*std::next(it, 3) == 'd');
}

TEST_CASE("Test iter *prev") {
	const auto str = std::string("tosa");
	const auto s = fsv::filtered_string_view{str};
	auto it = s.cend();
	REQUIRE(*std::prev(it) == 'a');
	REQUIRE(*std::prev(it, 2) == 's');
}

TEST_CASE("Test iterator begin and end") {
	const auto s = fsv::filtered_string_view{"puppy", [](const char& c) { return !(c == 'u' || c == 'y'); }};
	auto v = std::vector<char>{s.begin(), s.end()};
	REQUIRE(v.size() == 3);
	REQUIRE(v[0] == 'p');
	REQUIRE(v[1] == 'p');
	REQUIRE(v[2] == 'p');
}

TEST_CASE("Test again iterator begin and end") {
	auto s = fsv::filtered_string_view{"milo", [](const char& c) { return !(c == 'i' || c == 'o'); }};
	auto v = std::vector<char>{s.rbegin(), s.rend()};
	REQUIRE(v.size() == 2);
	REQUIRE(v[0] == 'l');
	REQUIRE(v[1] == 'm');
}

TEST_CASE("Test split function with interest predicate") {
	auto interest = std::set<char>{'a', 'A', 'b', 'B', 'c', 'C', 'd', 'D', 'e', 'E', 'f', 'F', ' ', '/'};
	auto sv = fsv::filtered_string_view{"0xDEADBEEF / 0xdeadbeef",
	                                    [&interest](const char& c) { return interest.contains(c); }};
	auto tok = fsv::filtered_string_view{" / "};
	auto v1 = fsv::split(sv, tok);

	REQUIRE(v1.size() == 2);
}

TEST_CASE("Test split function with default predicate") {
	auto sv = fsv::filtered_string_view{"xax"};
	auto tok  = fsv::filtered_string_view{"x"};
	auto v2 = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{"", "a", ""};

	REQUIRE(v2.size() == 3);
}

TEST_CASE("Test split function all tok in fsv") {
	auto sv = fsv::filtered_string_view{"xx"};
	auto tok = fsv::filtered_string_view{"x"};
	auto v = fsv::split(sv, tok);
	auto expected = std::vector<fsv::filtered_string_view>{"", "", ""};
	CHECK(v == expected);
}

TEST_CASE("Test substr function") {
	auto sv = fsv::filtered_string_view{"Siberian Husky"};
	REQUIRE(fsv::substr(sv, 9) == "Husky");
}

TEST_CASE("Test substr function with isupper predicate") {
	auto is_upper = [](const char& c) { return std::isupper(static_cast<unsigned char>(c)); };
	auto sv1 = fsv::filtered_string_view{"Sled Dog", is_upper};
	REQUIRE(fsv::substr(sv1, 0, 2) == "SD");
}
