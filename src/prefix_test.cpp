#include <iostream>
#include <iomanip>
#include "Prefix.hpp"

using namespace librt;
using namespace std;

int
main(int argc, char **argv)
{
  /// IPv4 prefixes.
  {
    cout << "IPv4 prefixes" << endl;
    cout << "=============" << endl;

    // default constructor.
    Prefix<IPv4> p0;

    // constructor with struct in_addr (default prefix len).
    IPv4 a1 = { 0x010a0a0a };
    Prefix<IPv4> p1(a1);

    // constructor with struct in_addr and prefix len.
    Prefix<IPv4> p2(a1, 24);

    // constructor with string as host address.
    Prefix<IPv4> p3("10.10.11.1");

    // constructor with string as CIDR format.
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

    cout << endl;

    // mask test.
    cout << "mask test" << endl;
    cout << "=========" << endl;
    IPv4 b1 = { 0xFFFFFFFF };
    for (size_t i = 0; i <= 32; ++i) {
      cout << Prefix<IPv4>(b1, i) << endl;
    }

    cout << endl;
  }

  /// IPv6 prefixes.
  {
    cout << "IPv6 prefixes" << endl;
    cout << "=============" << endl;

    // default constructor.
    Prefix<IPv6> p0;

    // constructor with struct in6_addr (default prefix len).
    IPv6 a1 = { 0x26, 0x07, 0xf8, 0xb0, 0x40, 0x00, 0x08, 0x15,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x04 };
    Prefix<IPv6> p1(a1);

    // constructor with struct in6_addr and prefix len.
    Prefix<IPv6> p2(a1, 64);

    // constructor with string as host address
    Prefix<IPv6> p3("2001:face:b00c::1");

    // constructor with string as CIDR format.
    Prefix<IPv6> p4("2001:dead:beef::/48");

    // Print prefixes.
    cout << "p0 " << p0 << endl;
    cout << "p1 " << p1 << endl;
    cout << "p2 " << p2 << endl;
    cout << "p3 " << p3 << endl;
    cout << "p4 " << p4 << endl;

    cout << endl;
  }

  return 0;
}
