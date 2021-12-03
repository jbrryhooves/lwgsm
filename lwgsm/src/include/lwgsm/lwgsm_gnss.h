/**
 * \file            lwgsm_gnss.h
 * \brief           GNSS API
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
 * Authors:          Tilen MAJERLE <tilen@majerle.eu>,
 *                   Josh Berry <joshua.berry88@gmail.com
 * Version:         v0.1.0
*/

#ifndef LWGSM_HDR_MQTT_H
#define LWGSM_HDR_MQTT_H

lwgsmr_t      lwgsm_gnss_set_power(const uint8_t power_state, lwgsm_api_cmd_evt_fn evt_fn, void* evt_arg, uint32_t blocking);
lwgsmr_t      lwgsm_gnss_get_power(uint8_t* power_state, lwgsm_api_cmd_evt_fn evt_fn, void* evt_arg, uint32_t blocking);

lwgsmr_t      lwgsm_gnss_set_gnss_mode(const uint8_t enable_glonass,const uint8_t enable_beidou,const uint8_t enable_galileo, const uint8_t enable_qzss, lwgsm_api_cmd_evt_fn evt_fn, void* evt_arg, uint32_t blocking);
lwgsmr_t      lwgsm_gnss_get_gnss_mode(uint8_t* glonass_enabled, uint8_t* beidou_enabled, uint8_t* galileo_enabled, uint8_t* qzss_enabled, lwgsm_api_cmd_evt_fn evt_fn, void* evt_arg, uint32_t blocking);

lwgsmr_t      lwgsm_gnss_cold_start(lwgsm_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking)
lwgsmr_t      lwgsm_gnss_warm_start(lwgsm_api_cmd_evt_fn evt_fn, void* evt_arg, uint32_t blocking);
lwgsmr_t      lwgsm_gnss_hot_start(lwgsm_api_cmd_evt_fn evt_fn, void* evt_arg, uint32_t blocking);

lwgsmr_t      lwgsm_gnss_info(lwgsm_gnss_response_t* gnss_response, lwgsm_api_cmd_evt_fn evt_fn, void* evt_arg, uint32_t blocking);


#endif /* LWGSM_HDR_MQTT_H */
