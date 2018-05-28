/*
 * Copyright (c) 2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GOOGLE_IOT_H
#define GOOGLE_IOT_H

#ifndef GOOGLE_IOT_JWT_ES256
#define GOOGLE_IOT_JWT_ES256    1
#endif

#ifndef GOOGLE_IOT_JWT_RS256
#define GOOGLE_IOT_JWT_RS256    1
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mbedTLS.h"

#define GOOGLE_IOT_CLIENT_ID(project_id, cloud_region, registry_id, device_id) \
  "projects/"project_id"/locations/"cloud_region"/registries/"registry_id"/devices/"device_id

/*
  \brief       Create Google IoT JWT.
  \param[in]   private_key   Pointer to PEM-encoded private key
  \param[in]   project_id    Pointer to Google Cloud project ID
  \param[in]   iat           "Issued At" timestamp (seconds since 00:00:00 UTC, January 1, 1970)
  \param[in]   iat           "Expiration" timestamp (seconds since 00:00:00 UTC, January 1, 1970)
  \return      JWT string (caller must free)
*/
char *google_iot_jwt (const char *private_key,
                      const char *project_id,
                      unsigned int iat,
                      unsigned int exp);

#endif /* GOOGLE_IOT_H */
