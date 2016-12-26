#include "libfilezilla/event_handler.hpp"

#include <cppunit/extensions/HelperMacros.h>

class DispatchTest final : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(DispatchTest);
	CPPUNIT_TEST(testSingle);
	CPPUNIT_TEST(testArgs);
	CPPUNIT_TEST(testMultiple);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

	void testSingle();
	void testArgs();
	void testMultiple();
};

CPPUNIT_TEST_SUITE_REGISTRATION(DispatchTest);

namespace {
struct dispatch_target {

	void a() { ++a_; }
	void b() { ++b_; }
	void c() { ++c_; }

	void two(int a, int b)
	{
		a_ += a;
		b_ += b;
	}

	int a_{};
	int b_{};
	int c_{};
};

struct type1;
typedef fz::simple_event<type1> T1;

struct type2;
typedef fz::simple_event<type2> T2;

struct type3;
typedef fz::simple_event<type3> T3;

struct type4;
typedef fz::simple_event<type4, int, int> T4;
}

void DispatchTest::testSingle()
{
	dispatch_target t;

	T1 const t1{};
	CPPUNIT_ASSERT(fz::dispatch<T1>(t1, &t, &dispatch_target::a));
	CPPUNIT_ASSERT(fz::dispatch<T1>(t1, &t, &dispatch_target::b));

	T2 const t2{};
	CPPUNIT_ASSERT(!fz::dispatch<T1>(t2, &t, &dispatch_target::b));

	CPPUNIT_ASSERT_EQUAL(t.a_, 1);
	CPPUNIT_ASSERT_EQUAL(t.b_, 1);
}

void DispatchTest::testArgs()
{
	dispatch_target t;

	T4 const t4(1, 5);
	CPPUNIT_ASSERT(fz::dispatch<T4>(t4, &t, &dispatch_target::two));

	T3 const t3{};
	CPPUNIT_ASSERT(!fz::dispatch<T4>(t3, &t, &dispatch_target::two));

	CPPUNIT_ASSERT_EQUAL(t.a_, 1);
	CPPUNIT_ASSERT_EQUAL(t.b_, 5);
}

void DispatchTest::testMultiple()
{
	dispatch_target t;

	T1 const t1{};
	T2 const t2{};
	T3 const t3{};
	T4 const t4(3, 8);

	CPPUNIT_ASSERT((fz::dispatch<T1, T2, T3>(t1, &t, &dispatch_target::a, &dispatch_target::b, &dispatch_target::c)));
	CPPUNIT_ASSERT((fz::dispatch<T1, T2, T3>(t2, &t, &dispatch_target::a, &dispatch_target::b, &dispatch_target::c)));
	CPPUNIT_ASSERT((fz::dispatch<T1, T2, T3>(t3, &t, &dispatch_target::a, &dispatch_target::b, &dispatch_target::c)));
	CPPUNIT_ASSERT((!fz::dispatch<T1, T2, T3>(t4, &t, &dispatch_target::a, &dispatch_target::b, &dispatch_target::c)));

	CPPUNIT_ASSERT_EQUAL(t.a_, 1);
	CPPUNIT_ASSERT_EQUAL(t.b_, 1);
	CPPUNIT_ASSERT_EQUAL(t.c_, 1);

	CPPUNIT_ASSERT((fz::dispatch<T1, T4>(t4, &t, &dispatch_target::a, &dispatch_target::two)));

	CPPUNIT_ASSERT_EQUAL(t.a_, 4);
	CPPUNIT_ASSERT_EQUAL(t.b_, 9);
}
