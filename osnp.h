/*
 * Copyright (C) 2014, Michele Balistreri
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#ifndef OSNP_H
#define	OSNP_H

struct ieee802_15_4_frame {
    unsigned char *backing_buffer;
    unsigned int header_len;
    unsigned int payload_len;
    unsigned char *fc_low;
    unsigned char *fc_high;
    unsigned char *seq_no;
    unsigned char *dst_pan;
    unsigned char *dst_addr;
    unsigned char *src_pan;
    unsigned char *src_addr;
    unsigned char *sc;
    unsigned char *frame_counter;
    unsigned char *key_id;
    unsigned char *payload;
};


/* Frame type */
#define FCFRTYP_BEACON	0x00
#define FCFRTYP_DATA	0x01
#define FCFRTYP_ACK     0x02
#define FCFRTYP_MCMD	0x03

/* Addressing mode */
#define FCADDR_NONE	0x00
#define FCADDR_SHORT	0x02
#define FCADDR_EXT	0x03

/* Security Level */
#define SL_NONE         0x00
#define SL_MIC_32       0x01
#define SL_MIC_64       0x02
#define SL_MIC_128      0x03
#define SL_ENC          0x04
#define SL_ENC_MIC_32   0x05
#define SL_ENC_MIC_64   0x06
#define SL_ENC_MIC_128  0x07

/* Key Identifier */
#define KIM_IMPLICIT    0x00
#define KIM_1IDX        0x01
#define KIM_4SRC_1IDX   0x02
#define KIM_8SRC_1IDX   0x03

/* Frame Control Low bits */
#define FCFRTYP(x)	(x & 0x07)
#define FCSECEN		(1 << 3)
#define FCFRPEN		(1 << 4)
#define FCREQACK	(1 << 5)
#define FCPANCOMP	(1 << 6)

/* Frame Control High bits */
#define FCDSTADDR(x)	((x & 0x03) << 2)
#define FCFRVER(x)	((x & 0x03) << 4)
#define FCSRCADDR(x)    ((x & 0x03) << 6)

/* Security Control Field */
#define SECLEV(x)	(x & 0x03)
#define KEYIDM(x)	((x & 0x03) << 2)

/* Extraction Macro for Frame Control Low bits */
#define EXTRACT_FCFRTYP(x)      (x & 0x07)
#define EXTRACT_FCSECEN(x)      ((x >> 3) & 0x01)
#define EXTRACT_FCFRPEN(x)      ((x >> 4) & 0x01)
#define EXTRACT_FCREQACK(x)     ((x >> 5) & 0x01)
#define EXTRACT_FCPANCOMP(x)    ((x >> 6) & 0x01)

/* Extraction Macro for Frame Control High bits */
#define EXTRACT_FCDSTADDR(x)	((x >> 2) & 0x03)
#define EXTRACT_FCFRVER(x)	((x >> 4) & 0x03)
#define EXTRACT_FCSRCADDR(x)    ((x >> 6) & 0x03)

/* Extraction Macro for Security Control Field */
#define EXTRACT_SECLEV(x)	(x & 0x03)
#define EXTRACT_KEYIDM(x)	((x >> 2) & 0x03)

/* OSNP Commands */
#define OSNP_GET_DEVICE_INFO 0xA0
#define OSNP_CONFIGURE 0xA1
#define OSNP_GET_DATA 0xA2
#define OSNP_PERFORM 0xA3
#define OSNP_SUBSCRIBE 0xA4
#define OSNP_UNSUBSCRIBE 0xA5

/* OSNP Errors */
#define OSNP_UNSUPPORTED_COMMAND 0x01
#define OSNP_UNSUPPORTED_PARAMETERS 0x02
#define OSNP_SECURITY_ERROR 0x03
#define OSNP_DEVICE_BUSY 0x04

/* MAC Commands */
#define OSNP_MCMD_ASSOCIATION_REQ 0x01
#define OSNP_MCMD_ASSOCIATION_RES 0x02
#define OSNP_MCMD_DISASSOCIATED 0x03
#define OSNP_MCMD_DATA_REQ 0x04
#define OSNP_MCMD_DISCOVER 0x07 // roughly equivalent to IEEE 802.15.4 "Beacon request"

/* Transmission Status */
#define OSNP_TX_STATUS_OK 0
#define OSNP_TX_STATUS_NOACK 1
#define OSNP_TX_STATUS_CHANNEL_BUSY 2

/* Device Capabilities */
#define RX_POLL_DRIVEN 0x00
#define RX_ALWAYS_ON 0x01

/**
 * Initialize the OSNP state machine.
 */
void osnp_initialize(void);

/**
 * Callback on any OSNP-related timer interrupt.
 */
void osnp_timer_expired_cb(void);

/** Callback on frame receive event */
void osnp_frame_received_cb(unsigned char *frame_buf, int frame_len);

/** Callback on frame receive event */
void osnp_frame_sent_cb(unsigned char status);

/**
 * Polls the OSNP Hub asking if data is available.
 */
void osnp_poll(void);

/**
 * Associates the given buffer to the frame and sets all pointers at the correct place for easy access to all fields
 * of the frame. No data is copied.
 *
 * @param buf the buffer where the frame has been received
 * @param frame_len the total len of the received frame
 * @param frame a zero'ed frame structure
 */
void osnp_parse_frame(unsigned char *buf, unsigned int frame_len, struct ieee802_15_4_frame *frame);

/**
 * Initializes the frame with the given frame control and security control parameters. This sets all pointers
 * at the correct place according the Frame Control and Security Control bytes. It also sets the sequence counter,
 * and the source address according to the Source Addressing Mode using the OSNP_PAN, OSNP_SHORT_ADDRESS and OSNP_EUI
 * variables as needed.
 * 
 * @param fc_low The low byte of the control frame
 * @param fc_high The high byte of the control frame
 * @param sc The Security Control byte - give 0 if no security
 * @param buf The buffer backing this frame
 * @param frame The frame to initialize
 */
void osnp_initialize_frame(unsigned char fc_low, unsigned char fc_high, unsigned char sc, unsigned char *buf, struct ieee802_15_4_frame *frame);

/**
 * Initializes the destination frame as a response to the source frame. This means copying most of the header, but the source becomes the destination
 * and the source uses the OSNP_PAN, OSNP_SHORT_ADDRESS and OSNP_EUI field, according to the addressing mode.
 * The security frame counter is also not copied.
 *
 * @param src_frame the source frame
 * @param dst_frame the destination frame
 * @param dst_buf the backing buffer of the destination frame
 */
void osnp_initialize_response_frame(struct ieee802_15_4_frame *src_frame, struct ieee802_15_4_frame *dst_frame, unsigned char *dst_buf);

#endif	/* OSNP_H */

