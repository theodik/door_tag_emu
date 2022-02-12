/**
 * Copyright (c) 2015 - 2021, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#include <stdint.h>
#include "nfc_t2t_lib.h"
#include "nfc_ndef_msg.h"
#include "nfc_text_rec.h"
#include "boards.h"
#include "app_error.h"
#include "hardfault.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

#define MAX_REC_COUNT      3     /**< Maximum records count. */

/* Text message in English with its language code. */
/** @snippet [NFC text usage_1] */
static const uint8_t en_payload[] =
{
    'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!'
};
static const uint8_t en_code[] = {'e', 'n'};
/** @snippet [NFC text usage_1] */

/* Text message in Norwegian with its language code. */
static const uint8_t no_payload[] =
{
    'H', 'a', 'l', 'l', 'o', ' ', 'V', 'e', 'r', 'd', 'e', 'n', '!'
};
static const uint8_t no_code[] = {'N', 'O'};

/* Text message in Polish with its language code. */
static const uint8_t pl_payload[] =
{
    'W', 'i', 't', 'a', 'j', ' ', 0xc5, 0x9a, 'w', 'i', 'e', 'c', 'i', 'e', '!'
};
static const uint8_t pl_code[] = {'P', 'L'};

/* Buffer used to hold an NFC NDEF message. */
uint8_t m_ndef_msg_buf[256];

/**
 * @brief Callback function for handling NFC events.
 */
static void nfc_callback(void * p_context, nfc_t2t_event_t event, const uint8_t * p_data, size_t data_length)
{
    (void)p_context;

    switch (event)
    {
        case NFC_T2T_EVENT_FIELD_ON:
            bsp_board_led_on(BSP_BOARD_LED_0);
            break;
        case NFC_T2T_EVENT_FIELD_OFF:
            bsp_board_led_off(BSP_BOARD_LED_0);
            break;
        default:
            break;
    }
}


/**
 * @brief Function for encoding the NDEF text message.
 */
static ret_code_t welcome_msg_encode(uint8_t * p_buffer, uint32_t * p_len)
{
    /** @snippet [NFC text usage_2] */
    ret_code_t err_code;

    /* Create NFC NDEF text record description in English */
    NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_en_text_rec,
                                  UTF_8,
                                  en_code,
                                  sizeof(en_code),
                                  en_payload,
                                  sizeof(en_payload));
    /** @snippet [NFC text usage_2] */

    /* Create NFC NDEF text record description in Norwegian */
    NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_no_text_rec,
                                  UTF_8,
                                  no_code,
                                  sizeof(no_code),
                                  no_payload,
                                  sizeof(no_payload));

    /* Create NFC NDEF text record description in Polish */
    NFC_NDEF_TEXT_RECORD_DESC_DEF(nfc_pl_text_rec,
                                  UTF_8,
                                  pl_code,
                                  sizeof(pl_code),
                                  pl_payload,
                                  sizeof(pl_payload));

    /* Create NFC NDEF message description, capacity - MAX_REC_COUNT records */
    /** @snippet [NFC text usage_3] */
    NFC_NDEF_MSG_DEF(nfc_text_msg, MAX_REC_COUNT);
    /** @snippet [NFC text usage_3] */

    /* Add text records to NDEF text message */
    /** @snippet [NFC text usage_4] */
    err_code = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
                                       &NFC_NDEF_TEXT_RECORD_DESC(nfc_en_text_rec));
    VERIFY_SUCCESS(err_code);
    /** @snippet [NFC text usage_4] */
    err_code = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
                                       &NFC_NDEF_TEXT_RECORD_DESC(nfc_no_text_rec));
    VERIFY_SUCCESS(err_code);
    err_code = nfc_ndef_msg_record_add(&NFC_NDEF_MSG(nfc_text_msg),
                                       &NFC_NDEF_TEXT_RECORD_DESC(nfc_pl_text_rec));
    VERIFY_SUCCESS(err_code);

    /** @snippet [NFC text usage_5] */
    err_code = nfc_ndef_msg_encode(&NFC_NDEF_MSG(nfc_text_msg),
                                   p_buffer,
                                   p_len);
    return err_code;
    /** @snippet [NFC text usage_5] */
}

/**
 *@brief Function for initializing logging.
 */
static void log_init(void)
{
    ret_code_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

/**@brief Function for the Power Management.
 */
static void power_manage(void)
{
    // Use directly __WFE and __SEV macros since the SoftDevice is not available.

    // Wait for event.
    __WFE();

    // Clear Event Register.
    __SEV();
    __WFE();
}

#define CT 0x88

uint32_t make_internal_bytes_from_uid(const uint8_t * uid, size_t uid_length, uint8_t internal_bytes[10]) {
    switch (uid_length) {
        case 4:
            internal_bytes[0] = uid[0];
            internal_bytes[1] = uid[1];
            internal_bytes[2] = uid[2];
            internal_bytes[3] = uid[3];
            internal_bytes[4] = uid[0] ^ uid[1] ^ uid[2] ^ uid[3];
            internal_bytes[5] = 0xFF;
            internal_bytes[6] = 0xFF;
            internal_bytes[7] = 0xFF;
            internal_bytes[8] = 0xFF;
            internal_bytes[9] = 0x48; // "INT." "NFC Lib version" wtf is this?!
            return NRF_SUCCESS;
        case 7:
            internal_bytes[0] = uid[0];
            internal_bytes[1] = uid[1];
            internal_bytes[2] = uid[2];
            internal_bytes[3] = CT ^ uid[0] ^ uid[1] ^ uid[2];
            internal_bytes[4] = uid[3];
            internal_bytes[5] = uid[4];
            internal_bytes[6] = uid[5];
            internal_bytes[7] = uid[6];
            internal_bytes[8] = uid[3] ^ uid[4] ^ uid[5] ^ uid[6];
            internal_bytes[9] = 0x48; // "INT." "NFC Lib version" wtf is this?!
            return NRF_SUCCESS;
        case 10:
            memcpy(internal_bytes, uid, 10);
            return NRF_SUCCESS;
        default:
            return NRF_ERROR_INVALID_PARAM;
    }
}

/**
 * @brief Function for application main entry.
 */
int main(void)
{
    uint32_t  len = sizeof(m_ndef_msg_buf);
    uint32_t  err_code;

    log_init();

    /* Configure LED-pins as outputs */
    bsp_board_init(BSP_INIT_LEDS);

    /* Set up NFC */
    err_code = nfc_t2t_setup(nfc_callback, NULL);
    APP_ERROR_CHECK(err_code);

    /* uint8_t uid[] = {0x04, 0xA1, 0xD9, 0xF2, 0x23, 0x6D, 0x80}; */
    uint8_t uid[] = {0xA6, 0xC8, 0x3E, 0x33};
    uint8_t internal_bytes[10];

    err_code = make_internal_bytes_from_uid(uid, sizeof(uid), internal_bytes);
    APP_ERROR_CHECK(err_code);

    err_code = nfc_t2t_internal_set(internal_bytes, sizeof(internal_bytes));
    APP_ERROR_CHECK(err_code);

    err_code = nfc_t2t_parameter_set(NFC_T2T_PARAM_NFCID1, uid, sizeof(uid));
    APP_ERROR_CHECK(err_code);

    /* Encode welcome message */
    err_code = welcome_msg_encode(m_ndef_msg_buf, &len);
    APP_ERROR_CHECK(err_code);

    /* Set created message as the NFC payload */
    err_code = nfc_t2t_payload_set(m_ndef_msg_buf, len);
    APP_ERROR_CHECK(err_code);

    /* Start sensing NFC field */
    err_code = nfc_t2t_emulation_start();
    APP_ERROR_CHECK(err_code);

    while (1)
    {
        if (NRF_LOG_PROCESS() == false)
        {
            power_manage();
        }
    }
}

