#include "libfilezilla/optional.hpp"
#include "libfilezilla/shared.hpp"

#include "test_utils.hpp"
/*
 * This testsuite asserts the correctness of the
 * functions handling IP addresses
 */

class smart_pointer_test final : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(smart_pointer_test);
	CPPUNIT_TEST(test_optional);
	CPPUNIT_TEST(test_shared);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

	void test_optional();
	void test_shared();
};

CPPUNIT_TEST_SUITE_REGISTRATION(smart_pointer_test);

namespace fz {
template<typename T>
std::ostream& operator<<(std::ostream& s, sparse_optional<T> const& t) {
	if (!t) {
		s << "null";
	}
	else {
		s << *t;
	}
	return s;
}

template<typename T>
std::ostream& operator<<(std::ostream& s, shared_optional<T> const& t) {
	if (!t) {
		s << "null";
	}
	else {
		s << *t;
	}
	return s;
}
}

void smart_pointer_test::test_optional()
{
	fz::sparse_optional<int> empty, empty2;
	CPPUNIT_ASSERT(!empty);
	CPPUNIT_ASSERT_EQUAL(empty, empty2);

	fz::sparse_optional<int> two(2);
	CPPUNIT_ASSERT(static_cast<bool>(two));
	CPPUNIT_ASSERT_EQUAL(2, *two);

	CPPUNIT_ASSERT(empty < two);

	fz::sparse_optional<int> two2(2);
	fz::sparse_optional<int> three(3);

	CPPUNIT_ASSERT(two < three);
	CPPUNIT_ASSERT_EQUAL(two, two2);
}

void smart_pointer_test::test_shared()
{
	fz::shared_optional<int> empty, empty2;
	CPPUNIT_ASSERT(!empty);
	CPPUNIT_ASSERT_EQUAL(empty, empty2);

	fz::shared_optional<int> two(2);
	CPPUNIT_ASSERT(static_cast<bool>(two));
	CPPUNIT_ASSERT_EQUAL(2, *two);

	CPPUNIT_ASSERT(empty < two);

	fz::shared_optional<int> two2(2);
	fz::shared_optional<int> three(3);

	CPPUNIT_ASSERT(two < three);
	CPPUNIT_ASSERT_EQUAL(two, two2);


	CPPUNIT_ASSERT(&*two != &*two2);
	fz::shared_optional<int> shared_two(two);
	CPPUNIT_ASSERT(&*two == &*shared_two);

	shared_two.get() = 4;
	CPPUNIT_ASSERT(&*two != &*shared_two);
	CPPUNIT_ASSERT_EQUAL(2, *two);


	fz::shared_value<int> v;
	CPPUNIT_ASSERT(static_cast<bool>(v));
	CPPUNIT_ASSERT(v == int());
}
