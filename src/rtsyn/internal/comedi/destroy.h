/**
 * @file rtsyn/internal/comedi/destroy.h
 * @author Sergio Hidalgo
 * @brief Destroy callback declaration for the RTSyn Comedi module.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_INTERNAL_COMEDI_DESTROY_H
#define RTSYN_INTERNAL_COMEDI_DESTROY_H

#include <rtsyn/abi.h>

/**
 * @brief Release a comedi instance allocated by the create callback.
 *
 * @param instance Instance to destroy.
 */
void RTSYN_ABI_CALL rtsyn_comedi_destroy(void *instance);

#endif /* RTSYN_INTERNAL_COMEDI_DESTROY_H */
