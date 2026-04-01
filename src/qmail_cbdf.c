/**
 * qmail_cbdf.c - Compact Binary Document Format (CBDF) Encoder
 *
 * Phase I format:
 *   [pair_count:2 LE] [key/value pairs...] [0x1C] [0x1C] [0x02] [body...EOF]
 *
 * Keys: 1=GUID(16), 2=Subject(var), 12=NumAttachments(1),
 *       13=To(7,repeated), 14=CC(7,repeated), 19=From(7), 25=Timestamp(4 LE)
 */

#include "qmail/qmail_cbdf.h"
#include "logging.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Write a 7-byte mailbox address: coin_group(2 LE) + denomination(1) + SN(4 LE) */
static void write_mailbox(uint8_t *p, int8_t denomination, uint32_t serial_number) {
    p[0] = 0x00;  /* coin_group low (CloudCoin = 0x0006) */
    p[1] = 0x06;  /* coin_group high */
    p[2] = (uint8_t)denomination;
    p[3] = (uint8_t)(serial_number & 0xFF);
    p[4] = (uint8_t)((serial_number >> 8) & 0xFF);
    p[5] = (uint8_t)((serial_number >> 16) & 0xFF);
    p[6] = (uint8_t)((serial_number >> 24) & 0xFF);
}

result_t qmail_cbdf_encode(const qmail_cbdf_params_t *params,
                            uint8_t **data_out, size_t *size_out) {
    if (!params || !params->file_guid || !params->body || !data_out || !size_out) {
        return RESULT_INVALID_PARAM;
    }

    *data_out = NULL;
    *size_out = 0;

    size_t subject_len = params->subject ? strlen(params->subject) : 0;
    if (subject_len > 255) subject_len = 255;  /* value_len is 1 byte */ // what is value_len
    size_t body_len = strlen(params->body);

    int to_count = params->to_count;
    int cc_count = params->cc_count;

    /* Count pairs: GUID + Subject + NumAttachments + From + Timestamp + N*To + N*CC */
    uint16_t pair_count = 5 + (uint16_t)to_count + (uint16_t)cc_count;  // what if there is no subject?

    /* Calculate total CBDF size */
    size_t meta_size = 2;                     /* pair count */
    meta_size += 2 + 16;                      /* key 1: GUID */
    meta_size += 2 + subject_len;             /* key 2: Subject */
    meta_size += 2 + 1;                       /* key 12: NumAttachments */
    meta_size += (size_t)to_count * (2 + 7);  /* key 13: To (repeated) */
    meta_size += (size_t)cc_count * (2 + 7);  /* key 14: CC (repeated) */
    meta_size += 2 + 7;                       /* key 19: From */
    meta_size += 2 + 4;                       /* key 25: Timestamp */

    size_t total_len = meta_size + 3 + body_len;  /* +3 for FS, FS, STX */

    uint8_t *buf = (uint8_t *)malloc(total_len);
    if (!buf) return RESULT_MEMORY_ERROR;

    size_t off = 0;

    /* Pair count (2 bytes, little-endian) */
    buf[off++] = (uint8_t)(pair_count & 0xFF);
    buf[off++] = (uint8_t)((pair_count >> 8) & 0xFF);

    /* Key 1: GUID (16 bytes) */
    buf[off++] = 1;
    buf[off++] = 16;
    memcpy(buf + off, params->file_guid, 16);
    off += 16;

    /* Key 2: Subject (variable length) */
    buf[off++] = 2;
    buf[off++] = (uint8_t)subject_len;
    if (subject_len > 0) {
        memcpy(buf + off, params->subject, subject_len);
        off += subject_len;
    }

    /* Key 12: Number of Attachments (1 byte) */
    buf[off++] = 12;
    buf[off++] = 1;
    buf[off++] = (uint8_t)params->attachment_count;

    /* Key 13: To mailboxes (repeated, 7 bytes each) */
    for (int i = 0; i < to_count; i++) {
        buf[off++] = 13;
        buf[off++] = 7;
        write_mailbox(buf + off,
                      params->to_recipients[i].denomination,
                      params->to_recipients[i].serial_number);
        off += 7;
    }

    /* Key 14: CC mailboxes (repeated, 7 bytes each) */
    for (int i = 0; i < cc_count; i++) {
        buf[off++] = 14;
        buf[off++] = 7;
        write_mailbox(buf + off,
                      params->cc_recipients[i].denomination,
                      params->cc_recipients[i].serial_number);
        off += 7;
    }

    /* Key 19: From mailbox (7 bytes) */
    buf[off++] = 19;
    buf[off++] = 7;
    write_mailbox(buf + off, params->from_denomination, params->from_serial_number);
    off += 7;

    /* Key 25: Timestamp (4 bytes, Unix epoch, little-endian) */
    buf[off++] = 25;
    buf[off++] = 4;
    uint32_t now_ts = (uint32_t)time(NULL);
    buf[off++] = (uint8_t)(now_ts & 0xFF);
    buf[off++] = (uint8_t)((now_ts >> 8) & 0xFF);
    buf[off++] = (uint8_t)((now_ts >> 16) & 0xFF);
    buf[off++] = (uint8_t)((now_ts >> 24) & 0xFF);

    /* Separators */
    buf[off++] = 0x1C;  /* FS: end of metadata */
    buf[off++] = 0x1C;  /* FS: end of styles (empty in Phase I) */
    buf[off++] = 0x02;  /* STX: start of text */

    /* Body */
    memcpy(buf + off, params->body, body_len);
    off += body_len;

    log_info(LOG_CAT_COMMAND, "CBDF encode: %zu bytes (%u pairs, body=%zu)",
             off, pair_count, body_len);

    *data_out = buf;
    *size_out = off;
    return RESULT_SUCCESS;
}
