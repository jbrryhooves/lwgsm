/**
 * \file            lwgsm_network.c
 * \brief           Network API
 */

/*
 * Copyright (c) 2020 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwGSM - Lightweight GSM-AT library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v0.1.0
 */
#include "lwgsm/lwgsm_private.h"
#include "lwgsm/lwgsm_network.h"
#include "lwgsm/lwgsm_mem.h"

#if LWGSM_CFG_NETWORK || __DOXYGEN__

/**
 * \brief           Attach to network and active PDP context
 * \param[in]       apn: APN name
 * \param[in]       user: User name to attach. Set to `NULL` if not used
 * \param[in]       pass: User password to attach. Set to `NULL` if not used
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwgsmOK on success, member of \ref lwgsmr_t enumeration otherwise
 */
lwgsmr_t lwgsm_network_attach(const char *apn, const char *user, const char *pass, const lwgsm_api_cmd_evt_fn evt_fn, void *const evt_arg, const uint32_t blocking)
{
    LWGSM_MSG_VAR_DEFINE(msg);

    LWGSM_MSG_VAR_ALLOC(msg, blocking);
    LWGSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_NETWORK_ATTACH;
#if LWGSM_CFG_CONN
    LWGSM_MSG_VAR_REF(msg).cmd = LWGSM_CMD_CIPSTATUS;
#endif /* LWGSM_CFG_CONN */
    LWGSM_MSG_VAR_REF(msg).msg.network_attach.apn = apn;
    LWGSM_MSG_VAR_REF(msg).msg.network_attach.user = user;
    LWGSM_MSG_VAR_REF(msg).msg.network_attach.pass = pass;

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 200000);
}

/**
 * \brief           Detach from network
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwgsmOK on success, member of \ref lwgsmr_t enumeration otherwise
 */
lwgsmr_t lwgsm_network_detach(const lwgsm_api_cmd_evt_fn evt_fn, void *const evt_arg, const uint32_t blocking)
{
    LWGSM_MSG_VAR_DEFINE(msg);

    LWGSM_MSG_VAR_ALLOC(msg, blocking);
    LWGSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_NETWORK_DETACH;
#if LWGSM_CFG_CONN
    /* LWGSM_MSG_VAR_REF(msg).cmd = LWGSM_CMD_CIPSTATUS; */
#endif /* LWGSM_CFG_CONN */

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 60000);
}

/**
 * \brief           Check network PDP status
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwgsmOK on success, member of \ref lwgsmr_t enumeration otherwise
 */
lwgsmr_t lwgsm_network_check_status(const lwgsm_api_cmd_evt_fn evt_fn, void *const evt_arg, const uint32_t blocking)
{
    LWGSM_MSG_VAR_DEFINE(msg);

    LWGSM_MSG_VAR_ALLOC(msg, blocking);
    LWGSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_CIPSTATUS;

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 60000);
}

/**
 * \brief           Copy IP address from internal value to user variable
 * \param[out]      ip: Pointer to output IP variable
 * \return          \ref lwgsmOK on success, member of \ref lwgsmr_t enumeration otherwise
 */
lwgsmr_t lwgsm_network_copy_ip(lwgsm_ip_t *ip)
{
    if (lwgsm_network_is_attached()) {
        lwgsm_core_lock();
        LWGSM_MEMCPY(ip, &lwgsm.m.network.ip_addr, sizeof(*ip));
        lwgsm_core_unlock();
        return lwgsmOK;
    }
    return lwgsmERR;
}

/**
 * \brief           Check if device is attached to network and PDP context is active
 * \return          `1` on success, `0` otherwise
 */
uint8_t lwgsm_network_is_attached(void)
{
    uint8_t res;
    lwgsm_core_lock();
    res = LWGSM_U8(lwgsm.m.network.is_attached);
    lwgsm_core_unlock();
    return res;
}


/**
 * \brief           Check if device is attached to network and the IP APP is activated (+CNACT)
 * \return          `1` on success, `0` otherwise
 */
uint8_t lwgsm_network_is_ip_app_activated(void)
{
    uint8_t res = 0;
    uint8_t status;
    lwgsm_core_lock();
    for(uint8_t i = 0; i < (sizeof (lwgsm.m.ip_app)/ sizeof (ip_app_t)); i++ )
    {
        if(lwgsm.m.ip_app[i].status > 0)
        {
            return 1;
        }
    }

    lwgsm_core_unlock();
    return res;
}

#endif /* LWGSM_CFG_NETWORK || __DOXYGEN__ */

/**
 * \brief           Read RSSI signal from network operator
 * \param[out]      rssi: RSSI output variable. When set to `0`, RSSI is not valid
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwgsmOK on success, member of \ref lwgsmr_t enumeration otherwise
 */
lwgsmr_t lwgsm_network_rssi(int16_t *rssi, const lwgsm_api_cmd_evt_fn evt_fn, void *const evt_arg, const uint32_t blocking)
{
    LWGSM_MSG_VAR_DEFINE(msg);

    LWGSM_MSG_VAR_ALLOC(msg, blocking);
    LWGSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_CSQ_GET;
    LWGSM_MSG_VAR_REF(msg).msg.csq.rssi = rssi;

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 2000);
}

/**
 * \brief           Get network registration status
 * \return          Member of \ref lwgsm_network_reg_status_t enumeration
 */
lwgsm_network_reg_status_t lwgsm_network_get_reg_status(void)
{
    lwgsm_network_reg_status_t ret;
    lwgsm_core_lock();
    ret = lwgsm.m.network.status;
    lwgsm_core_unlock();
    return ret;
}

/**
 * \brief           Check network registration status (AT+CREG?)
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwgsmOK on success, member of \ref lwgsmr_t enumeration otherwise
 */
lwgsmr_t lwgsm_network_check_reg_status(const lwgsm_api_cmd_evt_fn evt_fn, void *const evt_arg, const uint32_t blocking)
{
    LWGSM_MSG_VAR_DEFINE(msg);

    LWGSM_MSG_VAR_ALLOC(msg, blocking);
    LWGSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_CREG_GET;


    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 2000);
}

/**
 * \brief           Set network registration status unsolicited reporting (AT+CREG=)
 * \param[in]       mode: 0= Disable reporting, 1=Enable unsolicited report (+CREG: <stat>), 2= enable unsolicited report with extra location info
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwgsmOK on success, member of \ref lwgsmr_t enumeration otherwise
 */
lwgsmr_t lwgsm_network_set_reg_status(const lwgsm_network_registration_unsolicited_report_t mode, const lwgsm_api_cmd_evt_fn evt_fn, void *const evt_arg, const uint32_t blocking)
{
    LWGSM_MSG_VAR_DEFINE(msg);

    LWGSM_MSG_VAR_ALLOC(msg, blocking);
    LWGSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_CREG_GET;
    LWGSM_MSG_VAR_REF(msg).msg.network_registration_reporting.mode = mode;

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 2000);
}



/**
 * \brief           Get current system network mode
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwgsmOK on success, member of \ref lwgsmr_t enumeration otherwise
 */
lwgsmr_t lwgsm_network_system_mode_get(const lwgsm_api_cmd_evt_fn evt_fn, void *const evt_arg, const uint32_t blocking)
{
    LWGSM_MSG_VAR_DEFINE(msg);

    LWGSM_MSG_VAR_ALLOC(msg, blocking);
    LWGSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_CNSMOD_GET;
    // LWGSM_MSG_VAR_REF(msg).msg.cops_get.curr = curr;

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 2000);
}

/**
 * \brief           Get current system network preference (CATM1 or NBIoT)
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwgsmOK on success, member of \ref lwgsmr_t enumeration otherwise
 */
lwgsmr_t lwgsm_network_system_preference_get(const lwgsm_api_cmd_evt_fn evt_fn, void *const evt_arg, const uint32_t blocking)
{
    LWGSM_MSG_VAR_DEFINE(msg);

    LWGSM_MSG_VAR_ALLOC(msg, blocking);
    LWGSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_CMNB_GET;

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 2000);
}

/**
 * \brief           Set current system network preference (CATM1 or NBIoT)
 * \param[in]       mode: Preference mode. This parameter can be a value of \ref lwgsm_network_mode_preference_t enumeration
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwgsmOK on success, member of \ref lwgsmr_t enumeration otherwise
 */
lwgsmr_t lwgsm_network_system_preference_set(const lwgsm_network_mode_preference_t mode, const lwgsm_api_cmd_evt_fn evt_fn, void *const evt_arg, const uint32_t blocking)
{
    LWGSM_MSG_VAR_DEFINE(msg);

    LWGSM_MSG_VAR_ALLOC(msg, blocking);
    LWGSM_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWGSM_MSG_VAR_REF(msg).cmd_def = LWGSM_CMD_CMNB_SET;

    LWGSM_MSG_VAR_REF(msg).msg.network_system_mode_preference.mode = mode;

    return lwgsmi_send_msg_to_producer_mbox(&LWGSM_MSG_VAR_REF(msg), lwgsmi_initiate_cmd, 2000);
}

/**
 * \brief           Read network registration status
 * \return          Member of \ref lwgsm_network_mode_preference_t enumeration
 */
lwgsm_network_mode_preference_t lwgsm_network_system_preference_read(void)
{
    lwgsm_network_mode_preference_t ret;
    lwgsm_core_lock();
    ret = lwgsm.m.network.net_mode_preference;
    lwgsm_core_unlock();
    return ret;
}
