/**
 * @file rtsyn/comedi.h
 * @author Sergio Hidalgo
 * @brief Public descriptor API for the RTSyn Comedi module.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 * @copyright Copyright (c) Sergio Hidalgo 2026
 */
#ifndef RTSYN_COMEDI_H
#define RTSYN_COMEDI_H

#include <rtsyn/abi/node.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Return the ABI descriptor for the comedi node.
 *
 * The returned descriptor has static storage duration and must not be modified
 * or freed by the caller.
 *
 * @return Pointer to the module node descriptor.
 */
RTSYN_ABI_EXPORT const rtsyn_abi_node_descriptor_t *RTSYN_ABI_CALL
rtsyn_comedi_get_descriptor(void);

/**
 * @brief Standard descriptor entry point used by the RTSyn module loader.
 *
 * @return Pointer to the module node descriptor.
 */
RTSYN_ABI_EXPORT const rtsyn_abi_node_descriptor_t *RTSYN_ABI_CALL
rtsyn_module_get_descriptor(void);

#ifdef __cplusplus
}
#endif

#endif /* RTSYN_COMEDI_H */
