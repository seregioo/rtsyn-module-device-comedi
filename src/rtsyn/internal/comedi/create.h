/**
 * @file rtsyn/internal/comedi/create.h
 * @author Sergio Hidalgo
 * @brief Create callback declaration for the RTSyn Comedi module.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_INTERNAL_COMEDI_CREATE_H
#define RTSYN_INTERNAL_COMEDI_CREATE_H

#include <rtsyn/abi.h>

/**
 * @brief Allocate and initialize a comedi instance.
 *
 * @param out_instance Output pointer that receives the allocated instance.
 * @return RTSyn ABI status code.
 */
rtsyn_abi_status_t RTSYN_ABI_CALL rtsyn_comedi_create(void **out_instance);

#endif /* RTSYN_INTERNAL_COMEDI_CREATE_H */
