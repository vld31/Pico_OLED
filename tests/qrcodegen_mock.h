#pragma once

#include <vector>

namespace qrcodegen {

class QrCode {
public:
  enum class Ecc { LOW, MEDIUM, QUARTILE, HIGH };

  static QrCode encodeText(const char* text, Ecc ecc);

  int getSize() const;
  bool getModule(int x, int y) const;

private:
  int size_ = 0;
  std::vector<bool> modules_; // row-major: y*size + x
};

} // namespace qrcodegen
