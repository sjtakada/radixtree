#include <netinet/in.h>
#include <iostream>
#include "Prefix.hpp"
#include "Tree.hpp"

using namespace librt;

struct Route {
  struct in_addr nexthop;
};

int
main(int argc, char **argv)
{
  typedef Tree<Prefix<IPv4>, Route> IPv4RouteTable;
  //  typedef shared_ptr<IPv4RouteTable> IPv4RouteTablePtr;

  auto ipv4_table = make_shared<IPv4RouteTable>();

  Prefix<IPv4> p1("10.10.10.0/24");
  Prefix<IPv4> p2("10.10.0.0/16");
  Prefix<IPv4> p3("10.10.0.0/24");

  ipv4_table->insert(p1, new Route);
  ipv4_table->insert(p2, new Route);
  ipv4_table->insert(p3, new Route);

  cout << "insert p1, p2, p3" << endl;
  for (IPv4RouteTable::iterator it = ipv4_table->begin();
       it != ipv4_table->end(); ++it) {
    std::cout << it->prefix() << std::endl;
  }

  cout << "lookup p3" << endl;
  auto it = ipv4_table->find(p3);
  if (it != ipv4_table->end()) {
    std::cout << it->prefix() << endl;
  }

  cout << "erase p3" << endl;
  ipv4_table->erase(it);
  for (IPv4RouteTable::iterator it = ipv4_table->begin();
       it != ipv4_table->end(); ++it) {
    std::cout << it->prefix() << std::endl;
  }

  cout << "erase p2" << endl;
  ipv4_table->erase_at(p1);
  for (IPv4RouteTable::iterator it = ipv4_table->begin();
       it != ipv4_table->end(); ++it) {
    std::cout << it->prefix() << std::endl;
  }

  return 0;
}
