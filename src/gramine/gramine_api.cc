//  Copyright (c) 2021-22, VMware Inc, and the Certifier Authors.  All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "gramine_api.h"

#define ATTESTATION_TYPE_SIZE 32

GramineFunctions gramineFuncs;

int gramine_file_size(const char *file_name) {
  struct stat file_info;

  if (stat(file_name, &file_info) != 0)
    return false;
  if (!S_ISREG(file_info.st_mode))
    return false;
  return (int)file_info.st_size;
}

bool gramine_Init(const int cert_size, byte *cert) {
  char attestation_type_str[ATTESTATION_TYPE_SIZE] = {0};
  void* ra_tls_attest_lib;
  size_t ret = 0;

  ret = gramine_rw_file("/dev/attestation/attestation_type", (uint8_t*)attestation_type_str,
                sizeof(attestation_type_str) - 1, /*do_write=*/false);
  if (ret < 0 && ret != -ENOENT) {
    printf("User requested SGX attestation but cannot read SGX-specific file "
           "/dev/attestation/attestation_type\n");
    return false;
  }

#ifdef DEBUG
  printf("Attestation type: %s\n", attestation_type_str);
#endif

  if (ret == -ENOENT || !strcmp(attestation_type_str, "none")) {
    ra_tls_attest_lib = NULL;
  } else if (!strcmp(attestation_type_str, "epid") || !strcmp(attestation_type_str, "dcap")) {
   ra_tls_attest_lib = dlopen("libra_tls_attest.so", RTLD_LAZY);
    if (!ra_tls_attest_lib) {
        printf("User requested RA-TLS attestation but cannot find lib\n");
        return false;
    }
  } else {
    printf("Unrecognized remote attestation type: %s\n", attestation_type_str);
    return false;
  }

  /* Setup Gramine specific API calls */
  gramine_setup_functions(&gramineFuncs);

  return true;
}

bool gramine_Attest(int claims_size, byte* claims, int* size_out, byte* out) {
  byte assertion[MAX_ASSERTION_SIZE];
  memset(assertion, 0, MAX_ASSERTION_SIZE);
  int assertion_size = 0;
  bool result = false;

#ifdef DEBUG
  printf("Invoking Gramine Attest %d\n", claims_size);
  gramine_print_bytes(claims_size, claims);
  printf("\n");
#endif

  result = (*gramineFuncs.Attest)
           (claims_size, claims, &assertion_size, assertion);
  if (!result) {
    printf("Gramine attest failed\n");
    return false;
  }

  int total_size = assertion_size + claims_size + (sizeof(int) * 2);

  int i, j = 0;
  for (i = 0; i < sizeof(int); i++, j++) {
    out[j] = ((byte*)&assertion_size)[i];
  }

  for (i = 0; i < assertion_size; i++, j++) {
    out[j] = assertion[i];
  }

  for (i = 0; i < sizeof(int); i++, j++) {
    out[j] = ((byte*)&claims_size)[i];
  }

  for (i = 0; i < claims_size; i++, j++) {
    out[j] = claims[i];
  }

  *size_out = j;

#ifdef DEBUG
  printf("Done Gramine Attest assertion size %d:\n", *size_out);
  gramine_print_bytes(*size_out, out);
#endif

  return true;
}

bool gramine_Verify(int claims_size, byte* claims, int *user_data_out_size,
                    byte *user_data_out, int* size_out, byte* out) {
  byte assertion[MAX_ASSERTION_SIZE];
  memset(assertion, 0, MAX_ASSERTION_SIZE);
  int assertion_size = 0;
  bool result = false;

#ifdef DEBUG
  printf("\nInput claims sent to gramine_Verify claims_size %d\n", claims_size);
  gramine_print_bytes(claims_size, claims);
#endif

  int i, j = 0;
  for (i = 0; i < sizeof(int); i++, j++) {
    ((byte*)&assertion_size)[i] = claims[j];
  }

  for (i = 0; i < assertion_size; i++, j++) {
    assertion[i] = claims[j];
  }

#ifdef DEBUG
  printf("\nAssertion:\n");
  gramine_print_bytes(assertion_size, assertion);
#endif

  for (i = 0; i < sizeof(int); i++, j++) {
    ((byte*)user_data_out_size)[i] = claims[j];
  }

  for (i = 0; i < *user_data_out_size; i++, j++) {
    user_data_out[i] = claims[j];
  }

#ifdef DEBUG
  printf("\nuser_data_out:\n");
  gramine_print_bytes(*user_data_out_size, user_data_out);
  printf("Invoking Gramine Verify %d\n", claims_size);
#endif

  result = (*gramineFuncs.Verify)
           (*user_data_out_size, user_data_out, assertion_size,
             assertion, size_out, out);
  if (!result) {
    printf("Gramine verify failed\n");
    return false;
  }

#ifdef DEBUG
  printf("Done Gramine Verification via API\n");
#endif

  return true;
}

bool gramine_Seal(int in_size, byte* in, int* size_out, byte* out) {
  bool result = false;

#ifdef DEBUG
  printf("Invoking Gramine Seal size: %d\n", in_size);
#endif

  result = (*gramineFuncs.Seal)(in_size, in, size_out, out);
  if (!result) {
    printf("Gramine seal failed\n");
    return false;
  }

#ifdef DEBUG
  printf("Done Gramine Seal size: %d\n", *size_out);
#endif

  return true;
}

bool gramine_Unseal(int in_size, byte* in, int* size_out, byte* out) {
  bool result = false;

#ifdef DEBUG
  printf("Invoking Gramine Unseal size: %d\n", in_size);
#endif

  result = (*gramineFuncs.Unseal)(in_size, in, size_out, out);
  if (!result) {
    printf("Gramine unseal failed\n");
    return false;
  }

#ifdef DEBUG
  printf("Done Gramine Unseal size: %d\n", *size_out);
#endif

  return true;
}

int gramine_Getkey(byte *user_report_data, sgx_key_128bit_t* key) {
  return gramine_Sgx_Getkey(user_report_data, key);
}
