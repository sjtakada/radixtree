#include <iostream>
#include <sstream>
#include <iomanip>
#include <gtest/gtest.h>
#include "Prefix.hpp"

using namespace librt;
using namespace std;

TEST(PrefixTest, IPv4Prefix) {
    // Default constructor.
    // Expect "0.0.0.0/0"
    Prefix<IPv4> p0;
    EXPECT_EQ(p0.address().s_addr, 0);
    EXPECT_EQ(p0.len(), 0);

    // Constructor with struct in_addr (default prefix len).
    // Expect "10.10.10.1/32"
    IPv4 a1 = { 0x010a0a0a };
    Prefix<IPv4> p1(a1);
    EXPECT_EQ(p1.address().s_addr, 0x10a0a0a);
    EXPECT_EQ(p1.len(), 32);

    // Constructor with struct in_addr and prefix len.
    // Expect "10.10.10.0/24"
    Prefix<IPv4> p2(a1, 24);
    EXPECT_EQ(p2.address().s_addr, 0x000a0a0a);
    EXPECT_EQ(p2.len(), 24);

    // Constructor with string as host address.
    // Expect "10.10.11.1/32"
    Prefix<IPv4> p3("10.10.11.1");
    EXPECT_EQ(p3.address().s_addr, 0x010b0a0a);
    EXPECT_EQ(p3.len(), 32);

    // Constructor with string as CIDR format.
    // Expect "10.10.11.0/24"
    Prefix<IPv4> p4("10.10.11.0/24");
    EXPECT_EQ(p4.address().s_addr, 0x000b0a0a);
    EXPECT_EQ(p4.len(), 24);

    // Check prefix len, applying mask.
    // Expect "10.10.11.0/24"
    Prefix<IPv4> p5("10.10.11.1/24");
    EXPECT_EQ(p5.address().s_addr, 0x000b0a0a);
    EXPECT_EQ(p5.len(), 24);

    // Invalid addresses.
    try {
      Prefix<IPv4> px("10.11.12.300/24");
      ASSERT_EQ(true, false); // TODO
    } catch (std::runtime_error& e) {
      stringstream ss;
      ss << e.what();
      EXPECT_EQ(ss.str(), "Invalid IPv4 address 10.11.12.300");
    }

    // Invalid prefix length.
    try {
      Prefix<IPv4> py("10.11.12.0/33");
      ASSERT_EQ(true, false); // TODO
    } catch (std::runtime_error& e) {
      stringstream ss;
      ss << e.what();
      EXPECT_EQ(ss.str(), "Invalid prefix length 33");
    }

#if 0
    // mask test.
    cout << "mask test" << endl;
    cout << "=========" << endl;
    IPv4 b1 = { 0xFFFFFFFF };
    for (size_t i = 0; i <= 32; ++i) {
      cout << Prefix<IPv4>(b1, i) << endl;
    }
#endif
}

TEST(PrefixTest, IPv6Prefix) {
  // constructor with struct in6_addr (default prefix len).
  IPv6 a1 = { 0x26, 0x07, 0xf8, 0xb0, 0x40, 0x00, 0x08, 0x15,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04 };
  Prefix<IPv6> p1(a1);
  stringstream s1;
  s1 << p1;
  EXPECT_EQ(s1.str(), string("2607:f8b0:4000:815::2004/128"));

  // constructor with struct in6_addr and prefix len.
  Prefix<IPv6> p2(a1, 64);
  stringstream s2;
  s2 << p2;
  EXPECT_EQ(s2.str(), string("2607:f8b0:4000:815::/64"));

  // constructor with string as host address
  Prefix<IPv6> p3("2001:face:b00c::1");
  stringstream s3;
  s3 << p3;
  EXPECT_EQ(s3.str(), string("2001:face:b00c::1/128"));

  // constructor with string as CIDR format.
  Prefix<IPv6> p4("2001:dead:beef::/48");
  EXPECT_EQ(p4.len(), 48);
  EXPECT_EQ(((u_char *)&p4.address())[0], 0x20);
  EXPECT_EQ(((u_char *)&p4.address())[1], 0x01);
  EXPECT_EQ(((u_char *)&p4.address())[2], 0xde);
  EXPECT_EQ(((u_char *)&p4.address())[3], 0xad);
  EXPECT_EQ(((u_char *)&p4.address())[4], 0xbe);
  EXPECT_EQ(((u_char *)&p4.address())[5], 0xef);
}

int
main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}
