/**
 * @file rtsyn/internal/comedi/set_param.h
 * @author Sergio Hidalgo
 * @brief Set-param callback declaration for the RTSyn Comedi module.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_INTERNAL_COMEDI_SET_PARAM_H
#define RTSYN_INTERNAL_COMEDI_SET_PARAM_H

#include <rtsyn/abi.h>
#include <stdint.h>

/**
 * @brief Set one parameter value on a comedi instance.
 *
 * @param instance Module instance.
 * @param param_index Parameter index from the descriptor.
 * @param value Pointer to the parameter value.
 * @return RTSyn ABI status code.
 */
rtsyn_abi_status_t RTSYN_ABI_CALL rtsyn_comedi_set_param(void *instance, uint32_t param_index,
                                                         const void *value);

#endif /* RTSYN_INTERNAL_COMEDI_SET_PARAM_H */
