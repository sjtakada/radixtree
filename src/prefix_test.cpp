#include <iostream>
#include <iomanip>
#include "Prefix.hpp"

using namespace librt;
using namespace std;

int
main(int argc, char **argv)
{
  /// IPv4 prefixes.
  cout << "IPv4 prefixes" << endl;
  cout << "=============" << endl;

  // default constructor.
  Prefix<IPv4> p0;

  // constructor with struct in_addr (default prefix len).
  IPv4 a1 = { 0x010a0a0a };
  Prefix<IPv4> p1(a1);

  // constructor with struct in_addr and prefix len.
  Prefix<IPv4> p2(a1, 24);

  // constructor with string in host address.
  Prefix<IPv4> p3("10.10.11.1");

  // constructor with string in CIDR format.
  Prefix<IPv4> p4("10.10.11.0/24");

  // Print prefixes.
  cout << "p0 " << p0 << endl; 
  cout << "p1 " << p1 << endl; 
  cout << "p2 " << p2 << endl;
  cout << "p3 " << p3 << endl; 
  cout << "p4 " << p4 << endl; 

  // Retrieve address and prefix len individually.
  cout << "p4.address " << hex << setw(8) << setfill('0')
       << p4.address().s_addr << " len " << p4.len() << endl;

  // Invalid addresses.
  try {
    Prefix<IPv4> px("10.11.12.300/24");
  } catch (std::runtime_error& e) {
    cout << "Error: " << e.what() << endl;
  }
  try {
    Prefix<IPv4> py("10.11.12.0/33");
  } catch (std::runtime_error& e) {
    cout << "Error: " << e.what() << endl;
  }

  return 0;
}
