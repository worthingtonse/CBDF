# QMail Format For Phase I
The QMail is made up of three parts:
1. Meta Data: Includes the Email ID, Subject text, Number of attachments, To:[], CC:[], Date & Time Sent and From:
2. Styles: Not used in phase I but the starting byte 0x1C and ending byte 0x1C still need to be shown
3. Binary Markup Text: Shows status message until email is downloaded.

## Phase I QMail File Structure
Here is a general look at the file structure.
```c
    PAIR COUNT (2 bytes, little-endian)
    META DATA (key/value pairs)
FS (File Separator)
    STYLES (empty in phase I)
FS (File Separator)
    STX (Start of Text)
        The text starts here and keeps going until EOF.
```

## Hello World QMail (Phase I example)
The following is an actual qmail file used in Phase I. The message is simply "Hello World!"
with two To recipients, no CC recipients, and no attachments.

All the values are shown in hex.

```c
+// METADATA
+//-----------------------------------------------------------------------------
+// Number of key/value pairs (2 bytes, little-endian)
+0700                                    // Pair Count: 7

+// Pair 1: QMail ID (GUID)
+01                                      // Key ID: 1
+10                                      // Value Length: 16 bytes
+bf7b94b391a246b58e48545dd8f13101        // Value: The GUID

+// Pair 2: Subject
+02                                      // Key ID: 2
+0c                                      // Value Length: 12 bytes
+48656c6c6f20576f726c6421                // Value: "Hello World!"

+// Pair 3: Number of Attachments
+0c                                      // Key ID: 12
+01                                      // Value Length: 1 byte
+00                                      // Value: 0 (no attachments)

+// Pair 4: To Mailbox (first recipient)
+0d                                      // Key ID: 13
+07                                      // Value Length: 7 bytes
+0006 02 983f0200                        // Mailbox: group=0x0006, denom=2, SN=147352 (LE)

+// Pair 5: To Mailbox (second recipient)
+0d                                      // Key ID: 13
+07                                      // Value Length: 7 bytes
+0006 02 2e670400                        // Mailbox: group=0x0006, denom=2, SN=288558 (LE)

+// Pair 6: From Mailbox
+13                                      // Key ID: 19
+07                                      // Value Length: 7 bytes
+0006 02 a078e803                        // Mailbox: group=0x0006, denom=2, SN=65566880 (LE)

+// Pair 7: Timestamp
+19                                      // Key ID: 25
+04                                      // Value Length: 4 bytes
+68cfb6ad                                // Value: 1758443181 (LE) = 2025-09-21 08:26:21 UTC

+// SEPARATORS
+//-----------------------------------------------------------------------------
+1c                                      // FS: End of Metadata, Start of Styles
+1c                                      // FS: End of Styles, Start of Markup

+// BODY
+//-----------------------------------------------------------------------------
+02                                      // STX: Start of Text
+48656c6c6f20576f726c6421                // Body: "Hello World!"
```

## Overall Structure
This is the structure of a QMail file for Phase I that is just for sending a text message

| Bytes | Name | Meaning | Description |
|---|---|---|---|
| 2 bytes (little-endian) | Pair Count | Number of key/value pairs that follow | 0 to 65535 pairs possible |
| Variable | Meta Key/Value Pairs | Key (1 byte), Value Length (1 byte), Value (N bytes) | See [Meta Data Key Table](#meta-data-key-table). Arrays use repeated keys. |
| 0x1C | FS (File Separator) | Separates the Meta Part and the Styles Part | The first 0x1C ends the metadata |
| 0x1C | FS (File Separator) | Separates the Styles Part and the Markup Part | The second 0x1C ends the styles (empty in Phase I) |
| 0x02 | STX | Start of Text | Body text follows until EOF. No end marker needed. |

## Key/Value Pair Format
Each pair in the metadata section has the same structure:

| Field | Size | Description |
|---|---|---|
| Key ID | 1 byte | Identifies the field (see table below) |
| Value Length | 1 byte | Length of the value in bytes (0-255) |
| Value | N bytes | The field data, format depends on Key ID |

Arrays (like multiple To or CC recipients) are encoded as **repeated keys** — each
element is its own key/value pair with the same Key ID. The Pair Count includes
every repeated entry individually.

## Meta Data Key Table
These are the keys available for Phase I. Each key/value pair consists of 1 byte Key ID, 1 byte Value Length, then Value bytes.

| Key ID | Name | Value Size | Description | Required | Phase |
|---|---|---|---|---|---|
| 1 | QMail ID | 16 | The GUID the sender assigned to this email | * | 1 |
| 2 | Subject | Variable (max 255) | UTF-8 encoded text of the subject line | | 1 |
| 12 | Number of Attachments | 1 | How many files are associated with the qmail. 0-255. | * | 1 |
| 13 | To Mailbox | 7 | A single To: recipient mailbox address. Repeated for each recipient. | * | 1 |
| 14 | CC Mailbox | 7 | A single CC: recipient mailbox address. Repeated for each recipient. | | 1 |
| 19 | From Mailbox | 7 | The sender's mailbox address. | * | 1 |
| 25 | Timestamp | 4 | Unix epoch timestamp, little-endian uint32. | * | 1 |

## Mailbox Address Format
All mailbox addresses (To, CC, From) are 7 bytes with the same layout:

| Offset | Size | Field | Description |
|---|---|---|---|
| 0 | 2 bytes | Coin Group | Network ID, little-endian. CloudCoin = 0x0006. |
| 2 | 1 byte | Denomination | Coin denomination (e.g., 0x02) |
| 3 | 4 bytes | Serial Number | Little-endian uint32 |

## Markup Control Characters
In Phase I, we only use five of them:

| ASCII Index Decimal | Hex | Symbol | Description | Used For |
|-------|-----|---------|-------------|------------|
| 2 | 02 | STX | Start of Text | Indicator that Markup Text Follows |
| 9 | 09 | HT | Horizontal Tab | Tab character |
| 10 | 0A | LF | Line Feed | Line Break (Unix) |
| 13 | 0D | CR | Carriage Return | Line Break (Windows) |
| 28 | 1C | FS | File Separator | Separates parts of the document |
