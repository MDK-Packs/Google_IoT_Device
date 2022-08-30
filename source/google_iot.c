/*
 * Copyright (c) 2018-2022 Arm Limited. All rights reserved.
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

#define MBEDTLS_ALLOW_PRIVATE_ACCESS
#include "google_iot.h"

#define JWT_HEAD_RS256  "{\"alg\":\"RS256\",\"typ\":\"JWT\"}"
#define JWT_HEAD_ES256  "{\"alg\":\"ES256\",\"typ\":\"JWT\"}"

// Convert base64 to base64url
static void base64url (char *str, size_t ilen, size_t *olen) {
  unsigned int i, j;

  for (i = 0, j = 0; i < ilen; i++) {
    switch (str[i]) {
      case '+':
        str[j++] = '-';
        break;
      case '/':
        str[j++] = '_';
        break;
      case '=':
        break;
      default:
        str[j++] = str[i];
        break;
    }
  }
  *olen = j;
}

// Compute signature using ECDSA and SHA-256
#if (GOOGLE_IOT_JWT_ES256 != 0)
static int es256_sign (mbedtls_pk_context *pk,
                       const unsigned char *hash,
                       unsigned char *sig,
                       mbedtls_ctr_drbg_context *p_rng) {
  mbedtls_ecdsa_context ecdsa;
  mbedtls_mpi r, s;
  int rc;

  // Initialize objects
  mbedtls_ecdsa_init(&ecdsa);
  mbedtls_mpi_init(&r);
  mbedtls_mpi_init(&s);

  rc = mbedtls_ecdsa_from_keypair(&ecdsa, mbedtls_pk_ec(*pk));
  if (rc != 0) {
    goto exit;
  }

  // Compute signature
  rc = mbedtls_ecdsa_sign_det_ext(&ecdsa.grp, &r, &s, &ecdsa.d,
                                  hash, 32, MBEDTLS_MD_SHA256,
                                  mbedtls_ctr_drbg_random, p_rng);
  if (rc != 0) {
    goto exit;
  }

  // Write signature
  rc = mbedtls_mpi_write_binary(&r, &sig[0],  32);
  if (rc != 0) {
    goto exit;
  }
  rc = mbedtls_mpi_write_binary(&s, &sig[32], 32);
  if (rc != 0) {
    goto exit;
  }

 exit:
  // Clean-up
  mbedtls_ecdsa_free(&ecdsa);
  mbedtls_mpi_free(&r);
  mbedtls_mpi_free(&s);

  return rc;
}
#endif

// Compute signature using RSA-PKCS1 and SHA-256
#if (GOOGLE_IOT_JWT_RS256 != 0)
static int rs256_sign (mbedtls_pk_context *pk,
                       const unsigned char *hash,
                       unsigned char *sig,
                       mbedtls_ctr_drbg_context *p_rng) {
  int rc;

  // Compute signature
  rc = mbedtls_rsa_pkcs1_sign(mbedtls_pk_rsa(*pk),
                              mbedtls_ctr_drbg_random, p_rng,
                              MBEDTLS_MD_SHA256,
                              32, hash, sig);
  return rc;
}
#endif

// Create Google IoT JWT
char *google_iot_jwt (const char *private_key,
                      const char *project_id,
                      unsigned int iat,
                      unsigned int exp) {
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  const char *pers = "google_iot_jwt_sign";
  mbedtls_pk_context pk;
  mbedtls_pk_type_t pk_type;
  unsigned char hash[32];
  unsigned char *sig = NULL;
  char *head;
  char *str = NULL;
  char *jwt = NULL;
  size_t str_len;
  size_t jwt_len;
  size_t sig_len;
  size_t ilen, olen;
  size_t n;
  int rc;

  // Initialize objects
  mbedtls_entropy_init(&entropy);
  mbedtls_ctr_drbg_init(&ctr_drbg);
  mbedtls_pk_init(&pk);

  // Seeding random number generator
  rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                             (const unsigned char *)pers, strlen(pers));
  if (rc != 0) {
    goto exit;
  }

  // Load private key
  rc = mbedtls_pk_parse_key(&pk,
                            (const unsigned char *)private_key,
                            strlen(private_key) + 1,
                            NULL, 0,
                            mbedtls_ctr_drbg_random, &ctr_drbg);
  if (rc != 0) {
    goto exit;
  }

  pk_type = mbedtls_pk_get_type(&pk);

  // Setup based on private key type
  str_len = 45 + strlen(project_id);
  switch (pk_type) {
#if (GOOGLE_IOT_JWT_ES256 != 0)
    case MBEDTLS_PK_ECKEY:
      head = JWT_HEAD_ES256;
      ilen = sizeof(JWT_HEAD_ES256) - 1;
      sig_len = 64;
      jwt_len = (4*((ilen + str_len + sig_len + (3*2)))/3) + 3;
      jwt = calloc(jwt_len, 1);
      if (jwt == NULL) {
        rc = -1;
        goto exit;
      }
      break;
#endif
#if (GOOGLE_IOT_JWT_RS256 != 0)
    case MBEDTLS_PK_RSA:
      head = JWT_HEAD_RS256;
      ilen = sizeof(JWT_HEAD_RS256) - 1;
      sig_len = (mbedtls_pk_rsa(pk))->len;
      jwt_len = (4*((ilen + str_len + sig_len + (3*2)))/3) + 3;
      jwt = calloc(jwt_len, 1);
      if (jwt == NULL) {
        rc = -1;
        goto exit;
      }
      break;
#endif
    default:
      rc = -1;
      goto exit;
  }

  // Encode JWT header
  rc = mbedtls_base64_encode((unsigned char *)jwt, jwt_len, &olen,
                             (unsigned char *)head, ilen);
  if (rc != 0) {
    goto exit;
  }
  ilen = olen;
  base64url(jwt, ilen, &olen);
  n = olen;

  jwt[n++] = '.';

  // Compose and encode JWT claim
  str = malloc(str_len);
  if (str == NULL) {
    rc = -1;
    goto exit;
  }
  rc = sprintf(str, "{\"aud\":\"%s\",\"iat\":%u,\"exp\":%u}",
               project_id, iat, exp);
  if (rc <= 0) {
    rc = -1;
    goto exit;
  }
  ilen = (size_t)rc;
  rc = mbedtls_base64_encode((unsigned char *)&jwt[n], jwt_len - n, &olen,
                             (unsigned char*)str, ilen);
  if (rc != 0) {
    goto exit;
  }
  ilen = olen;
  base64url(&jwt[n], ilen, &olen);
  n += olen;

  memset(str, 0, str_len);
  free(str);
  str = NULL;

  // Compute SHA-256 hash
  mbedtls_sha256((unsigned char *)jwt, n, hash, 0);

  // Compute signature
  switch (pk_type) {
#if (GOOGLE_IOT_JWT_ES256 != 0)
    case MBEDTLS_PK_ECKEY:
      sig = malloc(sig_len);
      if (sig != NULL) {
        rc = es256_sign(&pk, hash, sig, &ctr_drbg);
      } else {
        rc = -1;
      }
      break;
#endif
#if (GOOGLE_IOT_JWT_RS256 != 0)
    case MBEDTLS_PK_RSA:
      sig = malloc(sig_len);
      if (sig != NULL) {
        rc = rs256_sign(&pk, hash, sig, &ctr_drbg);
      } else {
        rc = -1;
      }
      break;
#endif
    default:
      rc = -1;
      break;
  }
  if (rc != 0) {
    goto exit;
  }

  jwt[n++] = '.';

  // Encode signature
  rc = mbedtls_base64_encode((unsigned char *)&jwt[n], jwt_len - n, &olen,
                             sig, sig_len);
  if (rc != 0) {
    goto exit;
  }
  ilen = olen;
  base64url(&jwt[n], ilen, &olen);
  n += olen;

  jwt[n] = 0;

  memset(sig, 0, sig_len);

  exit:
  // Clean-up
  mbedtls_pk_free(&pk);
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);
  free(sig);
  free(str);
  if (rc != 0) {
    free(jwt);
    jwt = NULL;
  }

  return jwt;
}
