/**
 * @file   tiledb_kv_write.cc
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2017 TileDB, Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @section DESCRIPTION
 *
 * It shows how to write to a key-value store.
 *
 * Run the following:
 *
 * $ ./tiledb_kv_create
 * $ ./tiledb_kv_write
 */

#include <tiledb.h>
#include <cstring>

int main() {
  // Create TileDB context
  tiledb_ctx_t* ctx;
  tiledb_ctx_create(&ctx);

  // Prepare attribute buffers
  int buffer_a1[] = {0, 1, 2, 3};
  uint64_t buffer_a2[] = {0, 1, 3, 6};
  char buffer_var_a2[] = "abbcccdddd";
  float buffer_a3[] = {0.1, 0.2, 1.1, 1.2, 2.1, 2.2, 3.1, 3.2};
  void* buffers[] = {buffer_a1, buffer_a2, buffer_var_a2, buffer_a3};
  uint64_t buffer_sizes[] = {sizeof(buffer_a1),
                             sizeof(buffer_a2),
                             sizeof(buffer_var_a2) - 1,
                             sizeof(buffer_a3)};

  // Prepare keys
  tiledb_kv_keys_t* keys;
  tiledb_kv_keys_create(ctx, &keys);
  int key_1 = 100;
  tiledb_kv_keys_add(ctx, keys, &key_1, TILEDB_INT32, sizeof(int));
  float key_2 = 200.0;
  tiledb_kv_keys_add(ctx, keys, &key_2, TILEDB_FLOAT32, sizeof(float));
  double key_3[2] = {300.0, 300.1};
  tiledb_kv_keys_add(ctx, keys, key_3, TILEDB_FLOAT64, 2 * sizeof(double));
  char key_4[] = "key_4";
  tiledb_kv_keys_add(ctx, keys, key_4, TILEDB_CHAR, strlen(key_4) + 1);

  // Create query
  tiledb_kv_query_t* query;
  tiledb_kv_query_create(
      ctx,
      &query,
      "my_kv",
      TILEDB_WRITE,
      keys,
      nullptr,
      0,
      buffers,
      buffer_sizes);

  // Submit query
  tiledb_kv_query_submit(ctx, query);

  // Clean up
  tiledb_kv_query_free(ctx, query);
  tiledb_kv_keys_free(ctx, keys);
  tiledb_ctx_free(ctx);

  return 0;
}
