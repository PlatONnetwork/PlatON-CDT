#include "platon/RLP.h"
#include "platon/chain.hpp"
using namespace std;

namespace platon {

// bytes RLPNull = rlp("");
// bytes RLPEmptyList = rlpList();

RLP::RLP(const bytesConstRef& _d, Strictness _s) : m_data(_d) {
#ifdef RLP_CHECK_THROW
  if ((_s & FailIfTooBig) && actualSize() < _d.size()) {
    if (_s & ThrowOnFail)
      internal::platon_throw("over size rlp");
    else
      m_data.reset();
  }
  if ((_s & FailIfTooSmall) && actualSize() > _d.size()) {
    if (_s & ThrowOnFail)
      internal::platon_throw("under size rlp");
    else
      m_data.reset();
  }
#endif
}

RLP::iterator& RLP::iterator::operator++() {
  if (m_remaining) {
    m_currentItem.retarget(m_currentItem.next().data(), m_remaining);
    size_t count = sizeAsEncoded(m_currentItem);
    m_currentItem.retarget(m_currentItem.data(), count);
    m_remaining -= std::min<size_t>(m_remaining, m_currentItem.size());
  } else
    m_currentItem.retarget(m_currentItem.next().data(), 0);
  return *this;
}

RLP::iterator::iterator(RLP const& _parent, bool _begin) {
  if (_begin && _parent.isList()) {
    auto pl = _parent.payload();
    m_currentItem = pl.cropped(0, sizeAsEncoded(pl));
    m_remaining = pl.size() - m_currentItem.size();
  } else {
    m_currentItem = _parent.data().cropped(_parent.data().size());
    m_remaining = 0;
  }
}

RLP RLP::operator[](size_t _i) const {
  if (_i < m_lastIndex) {
    m_lastEnd = sizeAsEncoded(payload());
    m_lastItem = payload().cropped(0, m_lastEnd);
    m_lastIndex = 0;
  }
  for (; m_lastIndex < _i && m_lastItem.size(); ++m_lastIndex) {
    m_lastItem = payload().cropped(m_lastEnd);
    m_lastItem = m_lastItem.cropped(0, sizeAsEncoded(m_lastItem));
    m_lastEnd += m_lastItem.size();
  }
  return RLP(m_lastItem, int(ThrowOnFail) | int(FailIfTooSmall));
}

size_t RLP::actualSize() const {
  auto lengthSize = [this]() -> unsigned {
    if (m_data[0] < c_rlpListStart && m_data[0] > c_rlpDataIndLenZero)
      return m_data[0] - c_rlpDataIndLenZero;
    if (m_data[0] >= c_rlpListStart && m_data[0] > c_rlpListIndLenZero)
      return m_data[0] - c_rlpListIndLenZero;
    return 0;
  };
  if (isNull()) return 0;
  if (m_data[0] < c_rlpDataImmLenStart) return 1;
  if (m_data[0] < c_rlpListStart || m_data[0] >= c_rlpListStart)
    return (1 + lengthSize()) + length();
  return 0;
}

void RLP::requireGood() const {
#ifdef RLP_CHECK_THROW
  if (isNull()) internal::platon_throw("bad rlp");
  byte n = m_data[0];
  if (n != c_rlpDataImmLenStart + 1) return;
  if (m_data.size() < 2) internal::platon_throw("bad rlp");
  if (m_data[1] < c_rlpDataImmLenStart) internal::platon_throw("bad rlp");
#endif
}

bool RLP::isInt() const {
  if (isNull()) return false;
  requireGood();
  byte n = m_data[0];
  if (n < c_rlpDataImmLenStart)
    return !!n;
  else if (n == c_rlpDataImmLenStart)
    return true;
  else if (n <= c_rlpDataIndLenZero) {
#ifdef RLP_CHECK_THROW
    if (m_data.size() <= 1) internal::platon_throw("bad rlp");
#endif
    return m_data[1] != 0;
  } else if (n < c_rlpListStart) {
#ifdef RLP_CHECK_THROW
    if (m_data.size() <= size_t(1 + n - c_rlpDataIndLenZero))
      internal::platon_throw("bad rlp");
#endif
    return m_data[1 + n - c_rlpDataIndLenZero] != 0;
  } else
    return false;
}

size_t RLP::length() const {
  if (isNull()) return 0;
  requireGood();
  size_t ret = 0;
  byte const n = m_data[0];
  if (n < c_rlpDataImmLenStart)
    return 1;
  else if (n <= c_rlpDataIndLenZero)
    return n - c_rlpDataImmLenStart;
  else if (n < c_rlpListStart) {
#ifdef RLP_CHECK_THROW
    if (m_data.size() <= size_t(n - c_rlpDataIndLenZero))
      internal::platon_throw("bad rlp");
    if (m_data.size() > 1)
      if (m_data[1] == 0) internal::platon_throw("bad rlp");
#endif
    unsigned lengthSize = n - c_rlpDataIndLenZero;
#ifdef RLP_CHECK_THROW
    if (lengthSize > sizeof(ret))
      // We did not check, but would most probably not fit in our memory.
      internal::platon_throw("under size rlp");
    No leading zeroes.if (!m_data[1]) internal::platon_throw("bad rlp");
#endif
    ret = decodeSize(lengthSize);
#ifdef RLP_CHECK_THROW
    // Must be greater than the limit.
    if (ret < c_rlpListStart - c_rlpDataImmLenStart - c_rlpMaxLengthBytes)
      internal::platon_throw("bad rlp");
#endif
  } else if (n <= c_rlpListIndLenZero)
    return n - c_rlpListStart;
  else {
    unsigned lengthSize = n - c_rlpListIndLenZero;
#ifdef RLP_CHECK_THROW
    if (m_data.size() <= lengthSize) internal::platon_throw("bad rlp");
    if (m_data.size() > 1)
      if (m_data[1] == 0) internal::platon_throw("bad rlp");
    if (lengthSize > sizeof(ret))
      // We did not check, but would most probably not fit in our memory.
      internal::platon_throw("under size rlp");
    if (!m_data[1]) internal::platon_throw("bad rlp");
#endif
    ret = decodeSize(lengthSize);
#ifdef RLP_CHECK_THROW
    if (ret < 0x100 - c_rlpListStart - c_rlpMaxLengthBytes)
      internal::platon_throw("bad rlp");
#endif
  }
  // We have to be able to add payloadOffset to length without overflow.
  // This rejects roughly 4GB-sized RLPs on some platforms.
#ifdef RLP_CHECK_THROW
  if (ret >= std::numeric_limits<size_t>::max() - 0x100)
    internal::platon_throw("under size rlp");
#endif
  return ret;
}

size_t RLP::items() const {
  bytesConstRef d = payload();
  size_t i = 0;
  for (; d.size(); ++i) d = d.cropped(sizeAsEncoded(d));
  return i;
}

// RLPStream
RLPStream& RLPStream::appendRaw(const bytesConstRef& _s, size_t _itemCount) {
  m_out.append(_s.begin(), _s.end());
  noteAppended(_itemCount);
  return *this;
}

void RLPStream::noteAppended(size_t _itemCount) {
  if (!_itemCount) return;
  while (m_listStack.size()) {
    if (m_listStack.back().first < _itemCount)
      internal::platon_throw("itemCount too large");
    m_listStack.back().first -= _itemCount;
    if (m_listStack.back().first)
      break;
    else {
      auto p = m_listStack.back().second;
      m_listStack.pop_back();
      size_t s = m_out.size() - p;  // list size

      size_t appned_length = rlp_list_size(s);
      m_out.resize(p + appned_length);
      byte* data = m_out.data() + p;
      platon_rlp_list(data, s, data);
    }
    _itemCount = 1;  // for all following iterations, we've effectively appended
                     // a single item only since we completed a list.
  }
}

RLPStream& RLPStream::appendList(size_t _items) {
  if (_items)
    m_listStack.push_back(std::make_pair(_items, m_out.size()));
  else
    appendList(bytes());
  return *this;
}

RLPStream& RLPStream::appendList(bytesConstRef _rlp) {
  size_t length = _rlp.size();
  byte const* data = _rlp.data();
  size_t appned_length = rlp_list_size(length);
  size_t old_size = m_out.size();
  m_out.resize(old_size + appned_length);
  platon_rlp_list(data, length, m_out.data() + old_size);
  noteAppended();
  return *this;
}

RLPStream& RLPStream::append(bytesConstRef _s) {
  size_t length = _s.size();
  byte const* data = _s.data();
  size_t appned_length = rlp_bytes_size(data, length);
  size_t old_size = m_out.size();
  m_out.resize(old_size + appned_length);
  platon_rlp_bytes(data, length, m_out.data() + old_size);
  noteAppended();
  return *this;
}

RLPStream& RLPStream::append(bigint _i) {
  uint64_t low = _i;
  uint64_t heigh = _i >> 64;
  size_t appned_length = rlp_u128_size(heigh, low);
  size_t old_size = m_out.size();
  m_out.resize(old_size + appned_length);
  platon_rlp_u128(heigh, low, m_out.data() + old_size);
  noteAppended();
  return *this;
}
}  // namespace platon
