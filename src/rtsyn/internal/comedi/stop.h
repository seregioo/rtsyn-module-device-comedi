/**
 * @file rtsyn/internal/comedi/stop.h
 * @author Sergio Hidalgo
 * @brief Stop callback declaration for the RTSyn Comedi module.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_INTERNAL_COMEDI_STOP_H
#define RTSYN_INTERNAL_COMEDI_STOP_H

#include <rtsyn/abi.h>

/**
 * @brief Stop runtime execution for a comedi instance.
 *
 * @param instance Module instance.
 * @return RTSyn ABI status code.
 */
rtsyn_abi_status_t RTSYN_ABI_CALL rtsyn_comedi_stop(void *instance);

#endif /* RTSYN_INTERNAL_COMEDI_STOP_H */
