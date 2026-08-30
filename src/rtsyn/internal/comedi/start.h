/**
 * @file rtsyn/internal/comedi/start.h
 * @author Sergio Hidalgo
 * @brief Start callback declaration for the RTSyn Comedi module.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_INTERNAL_COMEDI_START_H
#define RTSYN_INTERNAL_COMEDI_START_H

#include <rtsyn/abi.h>

/**
 * @brief Prepare a comedi instance for runtime execution.
 *
 * @param instance Module instance.
 * @return RTSyn ABI status code.
 */
rtsyn_abi_status_t RTSYN_ABI_CALL rtsyn_comedi_start(void *instance);

#endif /* RTSYN_INTERNAL_COMEDI_START_H */
