/**
 * qmail_cbdf.h - Compact Binary Document Format (CBDF) Encoder
 *
 * Encodes email metadata + body into CBDF format for QMail Phase I.
 * Format: [pair_count:2 LE] [key:1][len:1][value:N]... [0x1C][0x1C][0x02][body...EOF]
 */

#ifndef QMAIL_CBDF_H
#define QMAIL_CBDF_H

#include "qmail/qmail_types.h"
#include "results.h"
#include <stdint.h>
#include <stddef.h>

/**
 * Input parameters for CBDF encoding.
 */
typedef struct {
    const uint8_t *file_guid;       /* 16-byte GUID (required) */
    const char    *subject;         /* UTF-8 subject (optional, max 255 bytes) */
    const char    *body;            /* UTF-8 body text (required) */
    int            attachment_count;/* Number of attachments (0 for Phase I) */

    /* From mailbox */
    int8_t         from_denomination;
    uint32_t       from_serial_number;

    /* To recipients */
    const qmail_recipient_entry_t *to_recipients;
    int             to_count;

    /* CC recipients */
    const qmail_recipient_entry_t *cc_recipients;
    int             cc_count;
} qmail_cbdf_params_t;

/**
 * Encode an email into CBDF format.
 *
 * Produces a complete .qmail file blob: metadata pairs + FS + FS + STX + body.
 * Caller must free() the returned buffer.
 *
 * @param params      Encoding parameters
 * @param data_out    Output: malloc'd CBDF buffer (caller frees)
 * @param size_out    Output: size of CBDF buffer
 * @return RESULT_SUCCESS on success
 */
result_t qmail_cbdf_encode(const qmail_cbdf_params_t *params,
                            uint8_t **data_out, size_t *size_out);

#endif /* QMAIL_CBDF_H */
