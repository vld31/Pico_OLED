#include "qrcodegen_mock.h"
#include <cstring>
#include <cstdint>

namespace qrcodegen {

static uint32_t fnv1a(const char* s) {
  uint32_t h = 2166136261u;
  if (!s) return h;
  for (const unsigned char* p = (const unsigned char*)s; *p; ++p) {
    h ^= *p;
    h *= 16777619u;
  }
  return h;
}

QrCode QrCode::encodeText(const char* text, Ecc /*ecc*/) {
  QrCode qr;

  const int len = text ? (int)std::strlen(text) : 0;
  qr.size_ = (len > 0 ? 21 : 0);
  qr.modules_.assign((size_t)qr.size_ * (size_t)qr.size_, false);

  if (qr.size_ == 0) return qr;

  uint32_t h = fnv1a(text);

  // Fill modules in a deterministic pattern depending on the hash
  for (int y = 0; y < qr.size_; ++y) {
    for (int x = 0; x < qr.size_; ++x) {
      // Mix x/y into the hash
      uint32_t m = h ^ (uint32_t)(x * 374761393u) ^ (uint32_t)(y * 668265263u);
      m = (m ^ (m >> 13)) * 1274126177u;
      bool on = ((m >> 1) & 1u) != 0;   // or >> 7, >> 16, etc.


      // Keep a couple fixed "finder-ish" corners if you want stability
      if ((x == 0 && y == 0) || (x == qr.size_ - 1 && y == qr.size_ - 1)) {
        on = true;
      }

      qr.modules_[(size_t)y * (size_t)qr.size_ + (size_t)x] = on;
    }
  }

  return qr;
}

int QrCode::getSize() const { return size_; }

bool QrCode::getModule(int x, int y) const {
  if (x < 0 || y < 0 || x >= size_ || y >= size_) return false;
  return modules_[(size_t)y * (size_t)size_ + (size_t)x];
}

} // namespace qrcodegen
