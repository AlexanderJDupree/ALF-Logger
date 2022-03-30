/**
 * cobs.h - Consistent Overhead Byte Stuffing
 *
 * COBS is an encoding scheme that removes all 'delimiter' bytes (usually zero)
 * from a message and stuffs the message with values only from 0x01 -> 0xFF.
 * This allows the 'delimiter' byte to unambigously define a packet boundary.
 */

#ifndef LIBALF_DETAILS_COBS_H
#define LIBALF_DETAILS_COBS_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
 * Includes
 ******************************************************************************/
#include <stddef.h>
#include <stdint.h>

/*******************************************************************************
 * Constants
 ******************************************************************************/
#ifndef LIBALF_COBS_DELIMITER_BYTE
#define LIBALF_COBS_DELIMITER_BYTE (0x00)
#endif

/**
 * @brief: COBS encode a message
 *
 * @param msg:         message to encode
 * @param msg_len:     length of message to encode
 * @param out:         output buffer
 * @param out_len:     length of output buffer
 *
 * @note: To ensure there is enough space for the encoded message in `out`
 * the output buff should be at least `msg_len` + ceil(msg_len/254) bytes in
 * size.
 *
 * @returns: Length of encoded message in bytes
 */
size_t cobs_encode(const uint8_t *msg, size_t msg_len, uint8_t *out,
                   size_t out_len);

/**
 * @brief: COBS decode a message
 *
 * @param encoded_msg:  message to decode
 * @param msg_len:      length of encoded message
 * @param out:          output buffer
 * @param out_len:      length of output buffer
 *
 * @returns: Length of decoded message in bytes
 *
 */
size_t cobs_decode(const uint8_t *encoded_msg, size_t msg_len, uint8_t *out,
                   size_t out_len);

#ifdef __cplusplus
}
#endif

#endif // LIBALF_DETAILS_COBS_H