#ifndef _RADIXTREE_LIB_PREFIX_HPP_
#define _RADIXTREE_LIB_PREFIX_HPP

#include <arpa/inet.h>
#include <string>
#include <ostream>
#include <stdexcept>

namespace librt {

using namespace std;

using IPv4 = struct in_addr;
using IPv6 = struct in6_addr;

template <typename T>
class Prefix
{
public:
  Prefix()
    : address_(), len_(0) { }
  ~Prefix() {}

  Prefix(const T& address, ushort len = maxPrefixLen)
    : address_(address), len_(len) { }
  Prefix(const string& prefix_str);

  // Construct a prefix from prefix p1 and p2.
  // Assuming p1 is shorter than p2.
  Prefix(const Prefix& prefix1, const Prefix& prefix2)
    : address_(), len_()
  {
    const u_char *p1 = (const u_char *)&prefix1.address();
    const u_char *p2 = (const u_char *)&prefix2.address();
    u_char *px = (u_char *)&address_;

    u_char i = 0;
    u_char j = 0;
    for (; i < sizeof(address_); ++i) {
      u_char cp = p1[i] ^ p2[i];
      if (cp == 0) {
        px[i] = p1[i];
      }
      else {
        while ((cp & 0x80) == 0) {
          cp <<= 1;
          j++;
        }

        px[i] = p1[i] & (0xFF << (8 - j));
        break;
      }
    }

    len_ = prefix2.len();
    if (len_ > i * 8 + j) {
      len_ = i * 8 + j;
    }
  }

  // Maximum prefix length in bits.
  static const uint8_t maxPrefixLen;

  // Get address.
  const T& address() const { return address_; }

  // Get prefix length.
  const ushort len() const { return len_; }

  // 1 or 0 at certain position of bit in the prefix.
  const u_char bit_at(const ushort index) const {
    ushort offset = index / 8;
    ushort shift = 7 - (index % 8);

    return (((const u_char *)&address_)[offset] >> shift) & 0x01;
  }

  // True if given prefix is included in this prefix.
  const bool match(const Prefix& prefix) const {
    // False if this prefix is more specifc.
    if (len_ > prefix.len()) {
      return false;
    }

    const u_char *np = (const u_char *)&address();
    const u_char *pp = (const u_char *)&prefix.address();

    ushort offset = len_ / 8;
    ushort shift = len_ % 8;

    if (shift) {
      if (maskbit(shift) & (np[offset] ^ pp[offset])) {
        return false;
      }
    }

    while (offset--) {
      if (np[offset] != pp[offset]) {
        return false;
      }
    }

    return true;
  }

  template <typename U>
  friend ostream& operator<<(ostream& stream, const Prefix<U>& p);

private:
  // Network Address.
  T address_;

  // Prefix Length.
  ushort len_;

  // Maskbits
  const u_char maskbit(ushort bits) const {
    static const u_char masks[] = {0x00, 0x80, 0xc0, 0xe0,
                                   0xf0, 0xf8, 0xfc, 0xfe, 0xff};
    return masks[bits];
  }
};

template <typename T>
const uint8_t Prefix<T>::maxPrefixLen = sizeof(T) * 8;

template<>
Prefix<IPv4>::Prefix(const string& prefix_str) {
  size_t pos = prefix_str.find("/");
  string addr_str;

  if (pos == string::npos) {
    len_ = Prefix<IPv4>::maxPrefixLen;
    addr_str = prefix_str;
  }
  else {
    len_ = stoi(prefix_str.substr(pos + 1));
    if (len_ > Prefix<IPv4>::maxPrefixLen) {
      throw runtime_error(string("Invalid prefix length ") +
                          to_string(len_));
    }

    addr_str = prefix_str.substr(0, pos);
  }

  if (!inet_pton(AF_INET, addr_str.c_str(), (void *)&address_)) {
    throw runtime_error(string("Invalid IPv4 address ") + addr_str);
  }
}

template<>
Prefix<IPv6>::Prefix(const string& prefix_str) {
  size_t pos = prefix_str.find("/");
  string addr_str;

  if (pos == string::npos) {
    len_ = Prefix<IPv6>::maxPrefixLen;
    addr_str = prefix_str;
  }
  else {
    len_ = stoi(prefix_str.substr(pos + 1));
    if (len_ > Prefix<IPv6>::maxPrefixLen) {
      throw runtime_error(string("Invalid prefix length ") +
                          to_string(len_));
    }

    addr_str = prefix_str.substr(0, pos);
  }

  if (!inet_pton(AF_INET6, addr_str.c_str(), (void*)&address_)) {
    throw runtime_error(string("Invalid IPv6 address ") + addr_str);
  }
}

template<>
ostream&
operator<<(ostream& stream, const Prefix<IPv4>& p)
{
  char buf[INET_ADDRSTRLEN];

  inet_ntop(AF_INET, (void *)&p.address_, buf, INET_ADDRSTRLEN);
  stream << buf << "/" << p.len_;

  return stream;
}

template<>
ostream&
operator<<(ostream& stream, const Prefix<IPv6>& p)
{
  char buf[INET6_ADDRSTRLEN];

  inet_ntop(AF_INET6, (void *)&p.address_, buf, INET6_ADDRSTRLEN);
  stream << buf << "/" << p.len_;

  return stream;
}

} // namespace librt

#endif //  _RADIXTREE_LIB_PREFIX_HPP__

