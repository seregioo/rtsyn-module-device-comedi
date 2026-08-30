/**
 * @file rtsyn/internal/comedi/process.h
 * @author Sergio Hidalgo
 * @brief Process callback declaration for the RTSyn Comedi module.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_INTERNAL_COMEDI_PROCESS_H
#define RTSYN_INTERNAL_COMEDI_PROCESS_H

#include <rtsyn/abi.h>

/**
 * @brief Execute one runtime cycle for a comedi instance.
 *
 * @param instance Module instance.
 * @param context Runtime callback context supplied by the host.
 * @return RTSyn ABI status code.
 */
rtsyn_abi_status_t RTSYN_ABI_CALL rtsyn_comedi_process(void *instance,
                                                       const rtsyn_abi_runtime_context_t *context);

#endif /* RTSYN_INTERNAL_COMEDI_PROCESS_H */
