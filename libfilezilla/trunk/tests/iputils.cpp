#include "libfilezilla/iputils.hpp"

#include "test_utils.hpp"
/*
 * This testsuite asserts the correctness of the
 * functions handling IP addresses
 */

class ip_address_test final : public CppUnit::TestFixture
{
	CPPUNIT_TEST_SUITE(ip_address_test);
	CPPUNIT_TEST(test_addresses);
	CPPUNIT_TEST_SUITE_END();

public:
	void setUp() {}
	void tearDown() {}

	void test_addresses();
};

CPPUNIT_TEST_SUITE_REGISTRATION(ip_address_test);

namespace {

template<typename String>
void do_test_address(String const& address, fz::address_type const expected_type, bool const routable, String const& long_form)
{
	ASSERT_EQUAL_DATA(expected_type, fz::get_address_type(address), address);
	ASSERT_EQUAL_DATA(routable, fz::is_routable_address(address), address);
	ASSERT_EQUAL_DATA(long_form, fz::get_ipv6_long_form(address), address);
}

void test_address(std::string const& address, fz::address_type const expected_type, bool const routable, std::string const& long_form)
{
	do_test_address(address, expected_type, routable, long_form);
	do_test_address(fz::to_wstring(address), expected_type, routable, fz::to_wstring(long_form));
}
}

void ip_address_test::test_addresses()
{
	// Some basic IPv4 address tests
	test_address("255.255.255.255", fz::address_type::ipv4,    true,  "");
	test_address("127.0.0.1",       fz::address_type::ipv4,    false, "");
	test_address("127.0.0.0.1",     fz::address_type::unknown, false, "");
	test_address("127.-0.0.1",      fz::address_type::unknown, false, "");
	test_address("127.a.0.1",       fz::address_type::unknown, false, "");
	test_address("127.256.0.1",     fz::address_type::unknown, false, "");
	test_address("127.0.1",         fz::address_type::unknown, false, "");
	test_address("127.0..1",        fz::address_type::unknown, false, "");
	test_address("127.0..0.1",      fz::address_type::unknown, false, "");

	// Valid IPv6 addresses
	test_address("::1",                                     fz::address_type::ipv6, false, "0000:0000:0000:0000:0000:0000:0000:0001");
	test_address("1234::1",                                 fz::address_type::ipv6, true,  "1234:0000:0000:0000:0000:0000:0000:0001");
	test_address("4::1",                                    fz::address_type::ipv6, true,  "0004:0000:0000:0000:0000:0000:0000:0001");
	test_address("1234:abcd::1234:ef01",                    fz::address_type::ipv6, true,  "1234:abcd:0000:0000:0000:0000:1234:ef01");
	test_address("1234:ABCD::1234:ef01",                    fz::address_type::ipv6, true,  "1234:abcd:0000:0000:0000:0000:1234:ef01");
	test_address("0:0:0:0:0:0:0:1",                         fz::address_type::ipv6, false,  "0000:0000:0000:0000:0000:0000:0000:0001");
	test_address("0:0:0::0:0:0:0:1",                        fz::address_type::ipv6, false,  "0000:0000:0000:0000:0000:0000:0000:0001");
	test_address("::0:0:0:0:0:0:0:1",                       fz::address_type::ipv6, false,  "0000:0000:0000:0000:0000:0000:0000:0001");
	test_address("0000:0000:0000:0000:0000:0000:0000:0001", fz::address_type::ipv6, false,  "0000:0000:0000:0000:0000:0000:0000:0001");

	test_address("[::1]",                                     fz::address_type::ipv6, false, "0000:0000:0000:0000:0000:0000:0000:0001");
	test_address("[1234::1]",                                 fz::address_type::ipv6, true,  "1234:0000:0000:0000:0000:0000:0000:0001");
	test_address("[4::1]",                                    fz::address_type::ipv6, true,  "0004:0000:0000:0000:0000:0000:0000:0001");
	test_address("[1234:abcd::1234:ef01]",                    fz::address_type::ipv6, true,  "1234:abcd:0000:0000:0000:0000:1234:ef01");
	test_address("[1234:ABCD::1234:ef01]",                    fz::address_type::ipv6, true,  "1234:abcd:0000:0000:0000:0000:1234:ef01");
	test_address("[0:0:0:0:0:0:0:1]",                         fz::address_type::ipv6, false,  "0000:0000:0000:0000:0000:0000:0000:0001");
	test_address("[::0:0:0:0:0:0:0:1]",                       fz::address_type::ipv6, false,  "0000:0000:0000:0000:0000:0000:0000:0001");
	test_address("[0:0:0::0:0:0:0:1]",                        fz::address_type::ipv6, false,  "0000:0000:0000:0000:0000:0000:0000:0001");
	test_address("[0000:0000:0000:0000:0000:0000:0000:0001]", fz::address_type::ipv6, false,  "0000:0000:0000:0000:0000:0000:0000:0001");

	// Invalid IPv6 addresses
	test_address("::",                                fz::address_type::unknown, false, "");
	test_address(":::",                               fz::address_type::unknown, false, "");
	test_address(":1234:abcd::1234:ef01",             fz::address_type::unknown, false, "");
	test_address("1234:abcd::1234:ef01:",             fz::address_type::unknown, false, "");
	test_address("1234:abcd::1234::ef01",             fz::address_type::unknown, false, "");
	test_address("[1234:abcd::1234::ef01",            fz::address_type::unknown, false, "");
	test_address("1234:abcd::1234::ef01]",            fz::address_type::unknown, false, "");
	test_address("[[1234:abcd::1234::ef01]]",         fz::address_type::unknown, false, "");
	test_address("1234:abcde:1234::ef01",             fz::address_type::unknown, false, "");
	test_address("1234:abcg:1234::ef01",              fz::address_type::unknown, false, "");
	test_address(":::1",                              fz::address_type::unknown, false, "");
	test_address("0:0:0:0:0:0:0:1:2",                 fz::address_type::unknown, false, "");
	test_address("0:0:0:0:0:0:0:1:2:0:0:0:0:0:0:1:2", fz::address_type::unknown, false, "");
	test_address("0::0:0:0:0:0:0:1:2:0:0:0:0:0:0:1",  fz::address_type::unknown, false, "");

	// 127.0.0.0/8
	test_address("126.255.255.255", fz::address_type::ipv4, true,  "");
	test_address("127.0.0.0",       fz::address_type::ipv4, false, "");
	test_address("127.255.255.255", fz::address_type::ipv4, false, "");
	test_address("128.0.0.0",       fz::address_type::ipv4, true,  "");

	// 10.0.0.0/8
	test_address("9.255.255.255",   fz::address_type::ipv4, true,  "");
	test_address("10.0.0.0",        fz::address_type::ipv4, false, "");
	test_address("10.255.255.255",  fz::address_type::ipv4, false, "");
	test_address("11.0.0.0",        fz::address_type::ipv4, true,  "");

	// 169.254.0.0/16
	test_address("169.253.255.255", fz::address_type::ipv4, true,  "");
	test_address("169.254.0.0",     fz::address_type::ipv4, false, "");
	test_address("169.254.255.255", fz::address_type::ipv4, false, "");
	test_address("169.255.0.0",     fz::address_type::ipv4, true,  "");

	// 192.168.0.0/16
	test_address("192.167.255.255", fz::address_type::ipv4, true,  "");
	test_address("192.168.0.0",     fz::address_type::ipv4, false, "");
	test_address("192.168.255.255", fz::address_type::ipv4, false, "");
	test_address("102.169.0.0",     fz::address_type::ipv4, true,  "");

	// 172.16.0.0/20
	test_address("172.15.255.255",  fz::address_type::ipv4, true,  "");
	test_address("172.16.0.0",      fz::address_type::ipv4, false, "");
	test_address("172.31.255.255",  fz::address_type::ipv4, false, "");
	test_address("172.32.0.0",      fz::address_type::ipv4, true,  "");
	test_address("172.112.0.0",     fz::address_type::ipv4, true,  "");
	test_address("172.3.6.0",       fz::address_type::ipv4, true,  "");

	test_address("::1", fz::address_type::ipv6,                  false, "0000:0000:0000:0000:0000:0000:0000:0001");
	test_address("::0", fz::address_type::ipv6,                  false, "0000:0000:0000:0000:0000:0000:0000:0000");
	test_address("::2", fz::address_type::ipv6,                  true,  "0000:0000:0000:0000:0000:0000:0000:0002");
	test_address("1234:ABCD::1234:ef01", fz::address_type::ipv6, true,  "1234:abcd:0000:0000:0000:0000:1234:ef01");

	// fe80::/10 (link local)
	test_address("fe7f:ffff:ffff:ffff:ffff:ffff:ffff:ffff", fz::address_type::ipv6, true,  "fe7f:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
	test_address("fe80:0000:0000:0000:0000:0000:0000:0000", fz::address_type::ipv6, false, "fe80:0000:0000:0000:0000:0000:0000:0000");
	test_address("febf:ffff:ffff:ffff:ffff:ffff:ffff:ffff", fz::address_type::ipv6, false, "febf:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
	test_address("fec0:0000:0000:0000:0000:0000:0000:0000", fz::address_type::ipv6, true,  "fec0:0000:0000:0000:0000:0000:0000:0000");

	// fc00::/7 (site local)
	test_address("fbff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", fz::address_type::ipv6, true,  "fbff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
	test_address("fc00:0000:0000:0000:0000:0000:0000:0000", fz::address_type::ipv6, false, "fc00:0000:0000:0000:0000:0000:0000:0000");
	test_address("fdff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", fz::address_type::ipv6, false, "fdff:ffff:ffff:ffff:ffff:ffff:ffff:ffff");
	test_address("fe00:0000:0000:0000:0000:0000:0000:0000", fz::address_type::ipv6, true,  "fe00:0000:0000:0000:0000:0000:0000:0000");

	// IPv4 mapped

	// 127.0.0.0/8
	test_address("::ffff:7eff:ffff", fz::address_type::ipv6, true,  "0000:0000:0000:0000:0000:ffff:7eff:ffff");
	test_address("::ffff:7f00:0000", fz::address_type::ipv6, false, "0000:0000:0000:0000:0000:ffff:7f00:0000");
	test_address("::ffff:7fff:ffff", fz::address_type::ipv6, false, "0000:0000:0000:0000:0000:ffff:7fff:ffff");
	test_address("::ffff:8000:0000", fz::address_type::ipv6, true,  "0000:0000:0000:0000:0000:ffff:8000:0000");

	// 10.0.0.0/8
	test_address("::ffff:9ff:ffff",  fz::address_type::ipv6, true,  "0000:0000:0000:0000:0000:ffff:09ff:ffff");
	test_address("::ffff:0a00:0000", fz::address_type::ipv6, false, "0000:0000:0000:0000:0000:ffff:0a00:0000");
	test_address("::ffff:0aff:ffff", fz::address_type::ipv6, false, "0000:0000:0000:0000:0000:ffff:0aff:ffff");
	test_address("::ffff:0b00:0000", fz::address_type::ipv6, true,  "0000:0000:0000:0000:0000:ffff:0b00:0000");

	// 169.254.0.0/16
	test_address("::ffff:a9fd:ffff", fz::address_type::ipv6, true,  "0000:0000:0000:0000:0000:ffff:a9fd:ffff");
	test_address("::ffff:a9fe:0000", fz::address_type::ipv6, false, "0000:0000:0000:0000:0000:ffff:a9fe:0000");
	test_address("::ffff:a9fe:ffff", fz::address_type::ipv6, false, "0000:0000:0000:0000:0000:ffff:a9fe:ffff");
	test_address("::ffff:a9ff:0000", fz::address_type::ipv6, true,  "0000:0000:0000:0000:0000:ffff:a9ff:0000");

	// 192.168.0.0/16
	test_address("::ffff:c0a7:ffff", fz::address_type::ipv6, true,  "0000:0000:0000:0000:0000:ffff:c0a7:ffff");
	test_address("::ffff:c0a8:0000", fz::address_type::ipv6, false, "0000:0000:0000:0000:0000:ffff:c0a8:0000");
	test_address("::ffff:c0a8:ffff", fz::address_type::ipv6, false, "0000:0000:0000:0000:0000:ffff:c0a8:ffff");
	test_address("::ffff:c0a9:0000", fz::address_type::ipv6, true,  "0000:0000:0000:0000:0000:ffff:c0a9:0000");

	// 172.16.0.0/20
	test_address("::ffff:ac0f:ffff", fz::address_type::ipv6, true,  "0000:0000:0000:0000:0000:ffff:ac0f:ffff");
	test_address("::ffff:ac10:0000", fz::address_type::ipv6, false, "0000:0000:0000:0000:0000:ffff:ac10:0000");
	test_address("::ffff:ac1f:ffff", fz::address_type::ipv6, false, "0000:0000:0000:0000:0000:ffff:ac1f:ffff");
	test_address("::ffff:ac20:0000", fz::address_type::ipv6, true,  "0000:0000:0000:0000:0000:ffff:ac20:0000");
}
