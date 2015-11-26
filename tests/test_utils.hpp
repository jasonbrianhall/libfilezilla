#ifndef LIBFILEZILLA_TEST_UTILS_HEADER
#define LIBFILEZILLA_TEST_UTILS_HEADER

#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>

template<typename T>
std::string inline value_to_string(T const& t, typename std::enable_if_t<std::is_enum<T>::value>* = 0)
{
	return std::to_string(std::underlying_type_t<T>(t));
}

template<typename T>
std::string inline value_to_string(T const& t, typename std::enable_if_t<std::is_arithmetic<T>::value>* = 0)
{
	return std::to_string(t);
}

std::string inline value_to_string(std::string const& t) {
	return t;
}

std::string inline value_to_string(std::wstring const& t) {
	return fz::to_string(t);
}

template<typename V, typename D>
void inline assert_equal_data(V const& expected, V const& actual, std::string const& func, D const& data, CppUnit::SourceLine line)
{
	if (expected != actual) {
		CppUnit::Asserter::failNotEqual(value_to_string(expected), value_to_string(actual), line, func + " with " + value_to_string(data));
	}
}

#define ASSERT_EQUAL_DATA(expected, actual, data) assert_equal_data((expected), (actual), #actual, data, CPPUNIT_SOURCELINE())

#endif
