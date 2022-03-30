/**
 * cobs.c - Consistent Overhead Byte Stuffing
 *
 */

#include <libalf/details/cobs.h>

size_t cobs_encode(const uint8_t *msg, size_t msg_len, uint8_t *out,
                   size_t out_len) {
  // TODO assert msg, out aren't null
  // TODO assert out_len is large enough to store result
  if (!msg || !out) {
    return 0;
  }

  size_t code = 1;
  size_t code_idx = 0;
  size_t write_idx = 1;

  for (size_t idx = 0; idx < msg_len; ++idx) {
    if (msg[idx] == LIBALF_COBS_DELIMITER_BYTE) {
      out[code_idx] = code;
      code_idx = write_idx++;
      code = 1;
    } else {
      out[write_idx++] = msg[idx];
      if (++code == 0xFF && (idx + 1) < msg_len) {
        out[code_idx] = code;
        code_idx = write_idx++;
        code = 1;
      }
    }
  }

  out[code_idx] = code;
  return write_idx;
}

size_t cobs_decode(const uint8_t *encoded_msg, size_t msg_len, uint8_t *out,
                   size_t out_len) {
  // TODO assert msg, out aren't null
  // TODO assert out_len is large enough to store result
  if (!encoded_msg || !out) {
    return 0;
  }

  size_t read_index = 0;
  size_t write_index = 0;
  uint8_t i;

  while (read_index < msg_len) {
    uint8_t code = encoded_msg[read_index];
    // TODO assert code isn't LIBALF_DELIMITER BYTE
    // TODO write script that CI can run to automatically create TODO issues

    if (read_index + code > msg_len && code != 1) {
      return 0;
    }

    read_index++;

    for (uint8_t i = 1; i < code; ++i) {
      out[write_index++] = encoded_msg[read_index++];
    }
    if (code != 0xFF && read_index != msg_len) {
      out[write_index++] = 0x00;
    }
  }

  return write_index;
}
