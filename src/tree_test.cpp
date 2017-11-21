#include <netinet/in.h>
#include <iostream>
#include <gtest/gtest.h>
#include "Prefix.hpp"
#include "Tree.hpp"

using namespace std;
using namespace librt;

struct Route {
  struct in_addr nexthop;
};

typedef Tree<Prefix<IPv4>, Route> IPv4RouteTable;

TEST(TreeTest, IPv4Route) {

  auto ipv4_table = make_shared<IPv4RouteTable>();

  Prefix<IPv4> p1("10.10.10.0/24");
  Prefix<IPv4> p2("10.10.0.0/16");
  Prefix<IPv4> p3("10.10.0.0/24");

  Route r1({0x01010101});
  Route r2({0x02020202});
  Route r3({0x03030303});

  ipv4_table->insert(p1, r1);
  ipv4_table->insert(p2, r2);
  ipv4_table->insert(p3, r3);

  stringstream ss;

  // Node iterator tests.
  IPv4RouteTable::iterator it = ipv4_table->begin();
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.0.0/16"));

  it = it->next();
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.0.0/20"));

  it = it->next();
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.0.0/24"));

  it = it->next();
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.10.0/24"));

  it = it->next();
  //  EXPECT_EQ(it, nullptr); TODO

  // Data iterator tests.
  it = ipv4_table->begin();
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.0.0/16"));
  EXPECT_EQ(it->data().nexthop.s_addr, 0x02020202);

  ++it;
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.0.0/24"));
  EXPECT_EQ(it->data().nexthop.s_addr, 0x03030303);

  ++it;
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.10.0/24"));
  EXPECT_EQ(it->data().nexthop.s_addr, 0x01010101);

  ++it;
  //  EXPECT_EQ(it, nullptr); TODO

  // Longext match test.
  it = ipv4_table->match(Prefix<IPv4>("10.10.10.0/28"));
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.10.0/24"));

  it = ipv4_table->match(Prefix<IPv4>("10.10.10.0/18"));
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.0.0/16"));

  // Exect match lookup test.
  it = ipv4_table->find(p3);
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.0.0/24"));

  // Erase test.
  ipv4_table->erase(it); // Erase p3
  it = ipv4_table->begin();
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.0.0/16"));

  ++it;
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.10.0/24"));

  ++it;
  //  EXPECT_EQ(it, nullptr); TODO
  
  
  ipv4_table->erase_at(p1); // Erase p1.
  it = ipv4_table->begin();
  ss.str("");
  ss << it->prefix();
  EXPECT_EQ(ss.str(), string("10.10.0.0/16"));

  ++it;
  //  EXPECT_EQ(it, nullptr); TODO
}

int
main(int argc, char **argv)
{
  testing::InitGoogleTest(&argc, argv);

  return RUN_ALL_TESTS();
}





