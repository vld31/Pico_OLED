#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "qrcodegen_mock.h"


TEST(QrCode, EncodeTextProducesNonEmptyQr) {
    const std::string text = "Hello Pico";

    // The C++ wrapper uses qrcodegen::QrCode
    const qrcodegen::QrCode qr = qrcodegen::QrCode::encodeText(
        text.c_str(),
        qrcodegen::QrCode::Ecc::LOW
    );

    const int size = qr.getSize();
    EXPECT_GT(size, 0);
    EXPECT_LT(size, 200);

    // Sanity: at least one dark module exists
    bool anyDark = false;
    for (int y = 0; y < size && !anyDark; y++) {
        for (int x = 0; x < size; x++) {
            if (qr.getModule(x, y)) {
                anyDark = true;
                break;
            }
        }
    }
    EXPECT_TRUE(anyDark);
}

TEST(QrCode, DifferentInputsGiveDifferentPatterns) {
    const qrcodegen::QrCode a = qrcodegen::QrCode::encodeText(
        "ABC",
        qrcodegen::QrCode::Ecc::LOW
    );
    const qrcodegen::QrCode b = qrcodegen::QrCode::encodeText(
        "XYZ",
        qrcodegen::QrCode::Ecc::LOW
    );

    ASSERT_EQ(a.getSize(), b.getSize());

    bool different = false;
    for (int y = 0; y < a.getSize() && !different; y++) {
        for (int x = 0; x < a.getSize(); x++) {
            if (a.getModule(x, y) != b.getModule(x, y)) {
                different = true;
                break;
            }
        }
    }
    EXPECT_TRUE(different);
}
