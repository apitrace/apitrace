/**************************************************************************
 *
 * Copyright 2015 VMware, Inc
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/


#include "qubjson.h"

#include <QDebug>
#include <QBuffer>

#include "gtest/gtest.h"


static inline std::ostream &
operator << (std::ostream &os, const QVariant &var)
{
    QString buf;
    QDebug debug(&buf);
    debug << var;
    os << buf.trimmed().toLocal8Bit().constData();
    return os;
}


static ::testing::AssertionResult
check(const char *buf_expr, const char *exp_expr, QByteArray & bytearray, const QVariant & expected)
{
    QBuffer buffer(&bytearray);
    buffer.open(QIODevice::ReadOnly);

    QVariant actual = decodeUBJSONObject(&buffer);

    if (!buffer.atEnd()) {
        return ::testing::AssertionFailure() << "Trailing bytes";
    }

    if (actual != expected) {
        return ::testing::AssertionFailure() << "Expected " << expected << " but got " << actual;
    }

    return ::testing::AssertionSuccess();
}


#define BYTEARRAY(...) { __VA_ARGS__ }

#define CHECK( x , y ) \
    { \
        static const unsigned char X[] = x; \
        QByteArray bytearray((const char *)X, sizeof X); \
        EXPECT_PRED_FORMAT2(check, bytearray, y); \
    }


TEST(qubjson, null) {
    CHECK(BYTEARRAY('Z'), QVariant());
}


TEST(qubjson, boolean) {
    CHECK(BYTEARRAY('T'), true);
    CHECK(BYTEARRAY('F'), false);
}


TEST(qubjson, integral) {
    CHECK(BYTEARRAY('i', 0x00), +0x00);
    CHECK(BYTEARRAY('i', 0x7f), +0x7f);
    CHECK(BYTEARRAY('i', 0x80), -0x80);
    CHECK(BYTEARRAY('U', 0x00), +0x00);
    CHECK(BYTEARRAY('U', 0x80), +0x80);
    CHECK(BYTEARRAY('U', 0xff), +0xff);
    CHECK(BYTEARRAY('I', 0x01, 0x23), +0x0123);
    CHECK(BYTEARRAY('I', 0x80, 0x00), -0x8000);
    CHECK(BYTEARRAY('l', 0x01, 0x23, 0x45, 0x67), +0x01234567);
    CHECK(BYTEARRAY('l', 0x80, 0x00, 0x00, 0x00), -0x7fffffff - 1); // -0x80000000 causes signed warnings
    CHECK(BYTEARRAY('L', 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF), qlonglong(+0x0123456789ABCDEFLL));
    CHECK(BYTEARRAY('L', 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00), qlonglong(-0x8000000000000000LL));
}


TEST(qubjson, float) {
    CHECK(BYTEARRAY('d', 0x3f, 0x80, 0x00, 0x00), 1.0f);
    CHECK(BYTEARRAY('d', 0x00, 0x00, 0x00, 0x01), 1.40129846e-45f);
    CHECK(BYTEARRAY('d', 0xff, 0x7f, 0xff, 0xff), -3.4028234e38f);

    CHECK(BYTEARRAY('D', 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00), 1.0);
    CHECK(BYTEARRAY('D', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01), 4.9406564584124654e-324);
    CHECK(BYTEARRAY('D', 0xff, 0xef, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff), -1.7976931348623157e308);
}


TEST(qubjson, string) {
    CHECK(BYTEARRAY('C', 'A'), "A");

    CHECK(BYTEARRAY('S', 'U', 0), "");
    CHECK(BYTEARRAY('S', 'U', 5, 'A', 'B', 'C', 'D', 'E'), "ABCDE");
}


TEST(qubjson, array) {
    QVariantList list;
    CHECK(BYTEARRAY('[', ']'), list);
    list.append(1);
    CHECK(BYTEARRAY('[', 'i', 1, ']'), list);
    CHECK(BYTEARRAY('[', '#', 'i', 1, 'i', 1), list);
    list.append(2);
    CHECK(BYTEARRAY('[', 'U', 1, 'i', 2, ']'), list);
    CHECK(BYTEARRAY('[', '#', 'U', 2, 'i', 1, 'i', 2), list);
}


TEST(qubjson, object) {
    QVariantMap map;
    CHECK(BYTEARRAY('{', '}'), map);
    map["A"] = 1;
    CHECK(BYTEARRAY('{', 'U', 1, 'A', 'i', 1, '}'), map);
    map["B"] = 2;
    CHECK(BYTEARRAY('{', 'U', 1, 'A', 'i', 1, 'U', 1, 'B', 'i', 2, '}'), map);
}


TEST(qubjson, binary_data) {
    CHECK(BYTEARRAY('[', '$', 'U', '#', 'U', 0), QByteArray());
    CHECK(BYTEARRAY('[', '$', 'U', '#', 'U', 1, 'A'), QByteArray("A"));
    CHECK(BYTEARRAY('[', '$', 'U', '#', 'U', 2, 'A', 'B'), QByteArray("AB"));
    CHECK(BYTEARRAY('[', '$', 'U', '#', 'U', 3, 'A', 'B', 'C'), QByteArray("ABC"));
}


int
main(int argc, char **argv)
{
#if defined(_MSC_VER) && !defined(NDEBUG)
    return 0;
#else
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
#endif
}
