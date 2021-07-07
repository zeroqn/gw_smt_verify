#ifndef GW_COMMON_H_
#define GW_COMMON_H_

#include "blake2b.h"
#include "gw_def.h"
#include "gw_errors.h"
#include "stddef.h"

/* common functions */

/* Implement of gw_blake2b_hash_fn
 * Note: this function is used in layer2 contract
 */
void blake2b_hash(uint8_t output_hash[GW_KEY_BYTES], uint8_t *input_data,
                  uint32_t len) {
  blake2b_state blake2b_ctx;
  blake2b_init(&blake2b_ctx, GW_KEY_BYTES);
  blake2b_update(&blake2b_ctx, input_data, len);
  blake2b_final(&blake2b_ctx, output_hash, GW_KEY_BYTES);
}

/* Generate raw key
 * raw_key: blake2b(id | type | key)
 *
 * We use raw key in the underlying KV store
 */
void gw_build_account_key(uint32_t id, const uint8_t *key, const size_t key_len,
                          uint8_t raw_key[GW_KEY_BYTES]) {
  uint8_t type = GW_ACCOUNT_KV;
  blake2b_state blake2b_ctx;
  blake2b_init(&blake2b_ctx, GW_KEY_BYTES);
  blake2b_update(&blake2b_ctx, (uint8_t *)&id, 4);
  blake2b_update(&blake2b_ctx, (uint8_t *)&type, 1);
  blake2b_update(&blake2b_ctx, key, key_len);
  blake2b_final(&blake2b_ctx, raw_key, GW_KEY_BYTES);
}

void gw_build_account_field_key(uint32_t id,
                                uint8_t field_type,
                                uint8_t key[GW_KEY_BYTES]) {
  memset(key, 0, 32);
  memcpy(key, (uint8_t *)(&id), sizeof(uint32_t));
  key[sizeof(uint32_t)] = field_type;
}

void gw_build_script_hash_to_account_id_key(uint8_t script_hash[GW_KEY_BYTES],
                                            uint8_t raw_key[GW_KEY_BYTES]) {
  blake2b_state blake2b_ctx;
  blake2b_init(&blake2b_ctx, GW_KEY_BYTES);
  uint32_t placeholder = 0;
  blake2b_update(&blake2b_ctx, (uint8_t *)&placeholder, 4);
  uint8_t type = GW_ACCOUNT_SCRIPT_HASH_TO_ID;
  blake2b_update(&blake2b_ctx, (uint8_t *)&type, 1);
  blake2b_update(&blake2b_ctx, script_hash, GW_KEY_BYTES);
  blake2b_final(&blake2b_ctx, raw_key, GW_KEY_BYTES);
}

void gw_build_data_hash_key(uint8_t data_hash[GW_KEY_BYTES], 
                            uint8_t raw_key[GW_KEY_BYTES]) {
  blake2b_state blake2b_ctx;
  blake2b_init(&blake2b_ctx, GW_KEY_BYTES);
  uint32_t placeholder = 0;
  blake2b_update(&blake2b_ctx, (uint8_t *)&placeholder, 4);
  uint8_t type = GW_DATA_HASH_PREFIX;
  blake2b_update(&blake2b_ctx, (uint8_t *)&type, 1);
  blake2b_update(&blake2b_ctx, data_hash, GW_KEY_BYTES);
  blake2b_final(&blake2b_ctx, raw_key, GW_KEY_BYTES);
}

#endif /* GW_COMMON_H_ */
