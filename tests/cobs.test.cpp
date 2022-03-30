#include <libalf/details/cobs.h>

#include <gtest/gtest.h>

TEST(CobsEncode, EncodeMsgIsNull) {
  uint8_t *msg = NULL;
  uint8_t out[256];
  size_t len = cobs_encode(msg, 0, out, sizeof(out));

  EXPECT_EQ(len, 0);
}

TEST(CobsEncode, EncodeOutIsNull) {
  uint8_t msg[] = {1, 2, 3, 4, 5};
  size_t len = cobs_encode(msg, sizeof(msg), NULL, 1000);

  EXPECT_EQ(len, 0);
}

TEST(CobsEncode, EncodeZeroLengthMessage) {
  uint8_t msg[] = {0};
  uint8_t out[256];
  size_t len = cobs_encode(msg, 0, out, sizeof(out));

  EXPECT_EQ(len, 1);
  EXPECT_EQ(out[0], 0x01);
}

TEST(CobsEncode, EncodeSmallMessageWithoutDelimiter) {
  uint8_t msg[] = {0x11, 0x22, 0x33, 0x44};
  uint8_t expected_msg[] = {0x05, 0x11, 0x22, 0x33, 0x44};
  uint8_t encoded_msg[sizeof(msg) + 1] = {0};
  size_t len = cobs_encode(msg, sizeof(msg), encoded_msg, sizeof(encoded_msg));

  EXPECT_EQ(len, sizeof(msg) + 1);
  EXPECT_TRUE(0 == memcmp(encoded_msg, expected_msg, sizeof(encoded_msg)));
}

TEST(CobsEncode, SmallMessageWithDelimiter) {
  uint8_t msg[] = {0x11, 0x22, 0x00, 0x33};
  uint8_t expected_msg[] = {0x03, 0x11, 0x22, 0x02, 0x33};
  uint8_t encoded_msg[sizeof(msg) + 1] = {0};
  size_t len = cobs_encode(msg, sizeof(msg), encoded_msg, sizeof(encoded_msg));

  EXPECT_EQ(len, sizeof(msg) + 1);
  EXPECT_TRUE(0 == memcmp(encoded_msg, expected_msg, sizeof(encoded_msg)));
}

TEST(CobsEncode, ByteLimitMessageWithoutDelimiter) {
  uint8_t msg[0xFE];
  uint8_t expected_msg[0xFF];
  uint8_t encoded_msg[sizeof(msg) + 2] = {0};

  // Build message: 0x01, 0x02, 0x03 ... 0xFD, 0xFE
  for (size_t i = 1; i < 0xFF; ++i) {
    msg[i - 1] = i;
    expected_msg[i] = i;
  }
  expected_msg[0] = 0xFF;

  size_t len = cobs_encode(msg, sizeof(msg), encoded_msg, sizeof(encoded_msg));

  EXPECT_EQ(len, sizeof(msg) + 1);
  EXPECT_TRUE(0 == memcmp(encoded_msg, expected_msg, sizeof(encoded_msg)));
}

TEST(CobsEncode, ByteLimitMessageWithDelimiter) {
  uint8_t msg[0xFE];
  uint8_t expected_msg[0xFF];
  uint8_t encoded_msg[sizeof(msg) + 2] = {0};

  // Build message: 0x00, 0x02, 0x03 ... 0xFD
  for (size_t i = 0; i < 0xFE; ++i) {
    msg[i] = i;
    expected_msg[i + 1] = i;
  }
  expected_msg[0] = 0x01;
  expected_msg[1] = 0xFE;

  size_t len = cobs_encode(msg, sizeof(msg), encoded_msg, sizeof(encoded_msg));

  EXPECT_EQ(len, sizeof(msg) + 1);
  EXPECT_TRUE(0 == memcmp(encoded_msg, expected_msg, sizeof(encoded_msg)));
}

TEST(CobsEncode, ByteMaxMessageWithoutDelimiter) {
  uint8_t msg[0xFF];
  uint8_t expected_msg[0x101];
  uint8_t encoded_msg[sizeof(msg) + 2] = {0};

  // Build message: 0x01, 0x02, 0x03 ... 0xFF
  for (size_t i = 0; i < 0xFF; ++i) {
    msg[i] = i + 1;
    expected_msg[i + 1] = i + 1;
  }
  expected_msg[0] = 0xFF;
  expected_msg[0xFF] = 0x02;
  expected_msg[0x100] = 0xFF;

  size_t len = cobs_encode(msg, sizeof(msg), encoded_msg, sizeof(encoded_msg));

  EXPECT_EQ(len, sizeof(msg) + 2);
  EXPECT_TRUE(0 == memcmp(encoded_msg, expected_msg, sizeof(encoded_msg)));
}

TEST(CobsEncode, ByteMaxMessageWithDelimiter) {
  uint8_t msg[0xFF];
  uint8_t expected_msg[0x101] = {0};
  uint8_t encoded_msg[sizeof(msg) + 1] = {0};

  // Build message: 0x01, 0x02, 0x03 ... 0xFF
  for (size_t i = 0; i < 0xFF; ++i) {
    msg[i] = i;
    expected_msg[i + 1] = i;
  }
  expected_msg[0] = 0x01;
  expected_msg[1] = 0xFF;

  size_t len = cobs_encode(msg, sizeof(msg), encoded_msg, sizeof(encoded_msg));

  EXPECT_EQ(len, sizeof(msg) + 1);
  EXPECT_TRUE(0 == memcmp(encoded_msg, expected_msg, sizeof(encoded_msg)));
}

TEST(CobsEncode, LongMessageOfAllZeroes) {
  uint8_t msg[0x200] = {0};
  uint8_t expected_msg[sizeof(msg) + 1];
  uint8_t encoded_msg[sizeof(msg) + 1] = {0};

  for (size_t i = 0; i < sizeof(expected_msg); ++i) {
    expected_msg[i] = 0x01;
  }

  size_t len = cobs_encode(msg, sizeof(msg), encoded_msg, sizeof(encoded_msg));
  EXPECT_EQ(len, sizeof(msg) + 1);
  EXPECT_TRUE(0 == memcmp(encoded_msg, expected_msg, sizeof(encoded_msg)));
}

TEST(CobsDecode, MsgIsNULL) {
  size_t len = cobs_decode(NULL, 20, (uint8_t *)0xDEADBEEF, 42);
  EXPECT_EQ(len, 0);
}

TEST(CobsDecode, ZeroLengthMessage) {
  size_t len = cobs_decode((uint8_t *)0xBAD, 0, (uint8_t *)0xBAD, 42);
  EXPECT_EQ(len, 0);
}

TEST(CobsDecode, BasicMessage) {
  uint8_t msg[] = {0x11, 0x22, 0x33, 0x44};
  uint8_t encoded_msg[] = {0x05, 0x11, 0x22, 0x33, 0x44};
  uint8_t decoded_msg[sizeof(msg)] = {0};

  size_t len = cobs_decode(encoded_msg, sizeof(encoded_msg), decoded_msg,
                           sizeof(decoded_msg));

  EXPECT_EQ(len, sizeof(msg));
  EXPECT_TRUE(0 == memcmp(msg, decoded_msg, sizeof(msg)));
}

TEST(CobsDecode, DecodeEncodedMessage) {
  uint8_t msg[] = {0x11, 0x22, 0x00, 0x33};
  uint8_t encoded_msg[sizeof(msg) + 1] = {0};
  uint8_t decoded_msg[sizeof(msg)] = {0};

  size_t enc_len =
      cobs_encode(msg, sizeof(msg), encoded_msg, sizeof(encoded_msg));
  size_t len =
      cobs_decode(encoded_msg, enc_len, decoded_msg, sizeof(decoded_msg));

  EXPECT_EQ(len, sizeof(msg));
  EXPECT_TRUE(0 == memcmp(msg, decoded_msg, sizeof(msg)));
}
