/**
 * @file rtsyn/internal/comedi/read_state.h
 * @author Sergio Hidalgo
 * @brief Read-state callback declaration for the RTSyn Comedi module.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_INTERNAL_COMEDI_READ_STATE_H
#define RTSYN_INTERNAL_COMEDI_READ_STATE_H

#include <rtsyn/abi.h>
#include <stdint.h>

/**
 * @brief Read a public state value from a comedi instance.
 *
 * @param instance Module instance.
 * @param state_index State index from the descriptor.
 * @param out_value Output storage for the state value.
 * @return RTSyn ABI status code.
 */
rtsyn_abi_status_t RTSYN_ABI_CALL rtsyn_comedi_read_state(const void *instance,
                                                          uint32_t state_index, void *out_value);

#endif /* RTSYN_INTERNAL_COMEDI_READ_STATE_H */
