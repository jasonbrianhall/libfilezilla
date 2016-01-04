#include "libfilezilla/string.hpp"

#include "test_utils.hpp"
/*
 * This testsuite asserts the correctness of the
 * string functions
 */

class string_test : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(string_test);
	CPPUNIT_TEST(test_conversion);
	CPPUNIT_TEST(test_conversion2);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

	void test_conversion();
	void test_conversion2();
};

CPPUNIT_TEST_SUITE_REGISTRATION(string_test);

void string_test::test_conversion()
{
	std::string const s("hello world!");
	
	std::wstring const w = fz::to_wstring(s);

	CPPUNIT_ASSERT_EQUAL(s.size(), w.size());

	for (size_t i = 0; i < s.size(); ++i) {
		CPPUNIT_ASSERT_EQUAL(static_cast<wchar_t>(s[i]), w[i]);
	}


	std::string const s2 = fz::to_string(s);

	CPPUNIT_ASSERT_EQUAL(s, s2);
}

void string_test::test_conversion2()
{
	wchar_t p[] = { 'M', 'o', 't', 0xf6, 'r', 'h', 'e', 'a', 'd', 0 };
	std::wstring const w(p);

	std::string const s = fz::to_string(w);

	CPPUNIT_ASSERT(s.size() >= w.size());

	std::wstring const w2 = fz::to_wstring(s);

	ASSERT_EQUAL(w, w2);
}
