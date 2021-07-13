#include "gw_smt.h"
#include "smt.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define GW_MAX_KV_PROOF_SIZE 4096

int main() {
  FILE *fp;
  uint8_t *data;
  size_t data_size;

  fp = fopen("../data", "rb");
  fseek(fp, 0, SEEK_END);
  data_size = ftell(fp);
  rewind(fp);

  data = (uint8_t *)malloc(data_size * sizeof(uint8_t));
  fread(data, data_size, 1, fp);
  fclose(fp);

  mol_seg_t smt_case_seg;
  smt_case_seg.ptr = data;
  smt_case_seg.size = data_size;
  if (MolReader_SMTCase_verify(&smt_case_seg, false) != MOL_OK) {
    printf("invalid smt case\n");
    exit(1);
  }

  uint8_t proof[GW_MAX_KV_PROOF_SIZE];
  size_t proof_size;

  uint8_t root[32];

  gw_state_t kv_state;
  gw_pair_t kv_pairs[GW_MAX_KV_PAIRS];

  int ret = 0;

  mol_seg_t proof_seg = MolReader_SMTCase_get_proof(&smt_case_seg);
  mol_seg_t proof_bytes_seg = MolReader_Bytes_raw_bytes(&proof_seg);

  memcpy(proof, proof_bytes_seg.ptr, proof_bytes_seg.size);
  proof_size = proof_bytes_seg.size;

  mol_seg_t root_seg = MolReader_SMTCase_get_root(&smt_case_seg);
  memcpy(root, root_seg.ptr, 32);

  mol_seg_t kv_state_seg = MolReader_SMTCase_get_kv_state(&smt_case_seg);
  uint32_t kv_pairs_len = MolReader_KVPairVec_length(&kv_state_seg);

  gw_state_init(&kv_state, kv_pairs, GW_MAX_KV_PAIRS);
  for (uint32_t i = 0; i < kv_pairs_len; i++) {
    mol_seg_res_t kv_res = MolReader_KVPairVec_get(&kv_state_seg, i);
    if (kv_res.errno != MOL_OK) {
      printf("invalid kv state pair");
      return GW_ERROR_INVALID_DATA;
    }

    mol_seg_t key_seg = MolReader_KVPair_get_k(&kv_res.seg);
    mol_seg_t value_seg = MolReader_KVPair_get_v(&kv_res.seg);
    ret = gw_state_insert(&kv_state, key_seg.ptr, value_seg.ptr);
    if (ret != 0) {
      printf("insert kv failed");
      return ret;
    }
  }

  gw_state_normalize(&kv_state);
  ret = gw_smt_verify(root, &kv_state, proof, proof_size);
  if (ret != 0) {
    printf("verify merkle root failed");
    return ret;
  }

  printf("successful verify merkle root");
  return 0;
}
