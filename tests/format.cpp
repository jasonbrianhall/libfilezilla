#include "libfilezilla/format.hpp"

#include "test_utils.hpp"
/*
 * This testsuite asserts the correctness of the
 * string formatting functions
 */

class format_test : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(format_test);
	CPPUNIT_TEST(test_sprintf);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

	void test_sprintf();
};

CPPUNIT_TEST_SUITE_REGISTRATION(format_test);

void format_test::test_sprintf()
{
	CPPUNIT_ASSERT_EQUAL(std::string("foo"), fz::sprintf("foo"));
	CPPUNIT_ASSERT_EQUAL(std::string("foo % bar"), fz::sprintf("foo %% bar"));

	CPPUNIT_ASSERT_EQUAL(std::string("foo bar"), fz::sprintf("foo %s", "bar"));
	CPPUNIT_ASSERT_EQUAL(std::string("foo bar"), fz::sprintf("foo %s", L"bar"));
	CPPUNIT_ASSERT_EQUAL(std::string("foo bar"), fz::sprintf("foo %s", std::string("bar")));
	CPPUNIT_ASSERT_EQUAL(std::string("foo bar"), fz::sprintf("foo %s", std::wstring(L"bar")));

	CPPUNIT_ASSERT_EQUAL(std::string("0"), fz::sprintf("%d", 0));
	CPPUNIT_ASSERT_EQUAL(std::string("   0"), fz::sprintf("%4d", 0));
	CPPUNIT_ASSERT_EQUAL(std::string("0000"), fz::sprintf("%04d", 0));
	CPPUNIT_ASSERT_EQUAL(std::string("   0"), fz::sprintf("% 4d", 0));
	CPPUNIT_ASSERT_EQUAL(std::string(" 000"), fz::sprintf("% 04d", 0));
	CPPUNIT_ASSERT_EQUAL(std::string(" 000"), fz::sprintf("%0 4d", 0));

	CPPUNIT_ASSERT_EQUAL(std::string("0"), fz::sprintf("%0d", 0));
	CPPUNIT_ASSERT_EQUAL(std::string(" 0"), fz::sprintf("% 0d", 0));
	CPPUNIT_ASSERT_EQUAL(std::string(" 0"), fz::sprintf("% d", 0));

	CPPUNIT_ASSERT_EQUAL(std::string("1"), fz::sprintf("%d", 1));
	CPPUNIT_ASSERT_EQUAL(std::string("   1"), fz::sprintf("%4d", 1));
	CPPUNIT_ASSERT_EQUAL(std::string("0001"), fz::sprintf("%04d", 1));
	CPPUNIT_ASSERT_EQUAL(std::string("   1"), fz::sprintf("% 4d", 1));
	CPPUNIT_ASSERT_EQUAL(std::string(" 001"), fz::sprintf("% 04d", 1));
	CPPUNIT_ASSERT_EQUAL(std::string(" 001"), fz::sprintf("%0 4d", 1));

	CPPUNIT_ASSERT_EQUAL(std::string("123"), fz::sprintf("%d", 123));
	CPPUNIT_ASSERT_EQUAL(std::string(" 123"), fz::sprintf("%4d", 123));
	CPPUNIT_ASSERT_EQUAL(std::string("0123"), fz::sprintf("%04d", 123));
	CPPUNIT_ASSERT_EQUAL(std::string(" 123"), fz::sprintf("% 4d", 123));
	CPPUNIT_ASSERT_EQUAL(std::string(" 123"), fz::sprintf("% 04d", 123));
	CPPUNIT_ASSERT_EQUAL(std::string(" 123"), fz::sprintf("%0 4d", 123));

	CPPUNIT_ASSERT_EQUAL(std::string("1234"), fz::sprintf("%d", 1234));
	CPPUNIT_ASSERT_EQUAL(std::string("1234"), fz::sprintf("%4d", 1234));
	CPPUNIT_ASSERT_EQUAL(std::string("1234"), fz::sprintf("%04d", 1234));
	CPPUNIT_ASSERT_EQUAL(std::string(" 1234"), fz::sprintf("% 4d", 1234));
	CPPUNIT_ASSERT_EQUAL(std::string(" 1234"), fz::sprintf("% 04d", 1234));
	CPPUNIT_ASSERT_EQUAL(std::string(" 1234"), fz::sprintf("%0 4d", 1234));

	CPPUNIT_ASSERT_EQUAL(std::string("12345"), fz::sprintf("%d", 12345));
	CPPUNIT_ASSERT_EQUAL(std::string("12345"), fz::sprintf("%4d", 12345));
	CPPUNIT_ASSERT_EQUAL(std::string("12345"), fz::sprintf("%04d", 12345));
	CPPUNIT_ASSERT_EQUAL(std::string(" 12345"), fz::sprintf("% 4d", 12345));
	CPPUNIT_ASSERT_EQUAL(std::string(" 12345"), fz::sprintf("% 04d", 12345));
	CPPUNIT_ASSERT_EQUAL(std::string(" 12345"), fz::sprintf("%0 4d", 12345));

	CPPUNIT_ASSERT_EQUAL(std::string("foo 7 foo"), fz::sprintf("%2$s %1$d %2$s", 7, "foo"));
}
