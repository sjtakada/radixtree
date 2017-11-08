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
    : address_(), prefix_len_(0) { }
  ~Prefix() {}

  Prefix(const T& address, ushort prefix_len = maxPrefixLen)
    : address_(address), prefix_len_(prefix_len) { }
  Prefix(const string& prefix_str);

  static const uint8_t maxPrefixLen;

  const T& address() { return address_; }
  const ushort prefix_len() { return prefix_len_; }

  template <typename U>
  friend ostream& operator<<(ostream& stream, const Prefix<U>& p);

private:
  // Network Address.
  T address_;

  // Prefix Length.
  ushort prefix_len_;
};

template <typename T>
const uint8_t Prefix<T>::maxPrefixLen = sizeof(T) * 8;

template<>
Prefix<IPv4>::Prefix(const string& prefix_str) {
  size_t pos = prefix_str.find("/");
  string addr_str;

  if (pos == string::npos) {
    prefix_len_ = Prefix<IPv4>::maxPrefixLen;
    addr_str = prefix_str;
  }
  else {
    prefix_len_ = stoi(prefix_str.substr(pos + 1));
    if (prefix_len_ > Prefix<IPv4>::maxPrefixLen) {
      throw runtime_error(string("Invalid prefix length ") +
                          to_string(prefix_len_));
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
    prefix_len_ = Prefix<IPv6>::maxPrefixLen;
    addr_str = prefix_str;
  }
  else {
    prefix_len_ = stoi(prefix_str.substr(pos + 1));
    if (prefix_len_ > Prefix<IPv6>::maxPrefixLen) {
      throw runtime_error(string("Invalid prefix length ") +
                          to_string(prefix_len_));
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
  stream << buf << "/" << p.prefix_len_;

  return stream;
}

template<>
ostream&
operator<<(ostream& stream, const Prefix<IPv6>& p)
{
  char buf[INET6_ADDRSTRLEN];

  inet_ntop(AF_INET6, (void *)&p.address_, buf, INET6_ADDRSTRLEN);
  stream << buf << "/" << p.prefix_len_;

  return stream;
}

#endif //  _RADIXTREE_LIB_PREFIX_HPP__

} // namespace librt
