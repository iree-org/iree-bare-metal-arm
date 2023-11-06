// SPDX-FileCopyrightText: 2023 Fraunhofer-Gesellschaft zur Förderung der angewandten Forschung e.V.
// SPDX-License-Identifier: BSD-3-Clause

#ifndef NRFX_GLUE_NRFX5340_H__
#define NRFX_GLUE_NRFX5340_H__

// File providing macros as requested in `nrfx_glue.h`.

#ifdef __cplusplus
extern "C" {
#endif

#include <nrfx_atomic.h>

#define nrfx_atomic_t nrfx_atomic_u32_t

#define NRFX_ATOMIC_FETCH_OR(p_data, value) \
  nrfx_atomic_u32_fetch_or(p_data, value)

#define NRFX_ATOMIC_FETCH_AND(p_data, value) \
  nrfx_atomic_u32_fetch_and(p_data, value)

#ifdef __cplusplus
}
#endif

#endif  // NRFX_GLUE_NRFX5340_H__
