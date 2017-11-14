/**
 * @file   tiledb_kv_read.cc
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
 * It shows how to read from a key-value store.
 *
 * You need to run the following to make it work:
 *
 * $ ./tiledb_kv_create
 * $ ./tiledb_kv_write
 * $ ./tiledb_kv_read
 */

#include <tiledb.h>
#include <cstdio>

int main() {
  // Create TileDB context
  tiledb_ctx_t* ctx;
  tiledb_ctx_create(&ctx);

  // Prepare (big enough) attribute buffers
  int buffer_a1[10];
  uint64_t buffer_a2[10];
  char buffer_var_a2[30];
  float buffer_a3[20];
  void* buffers[] = {buffer_a1, buffer_a2, buffer_var_a2, buffer_a3};
  uint64_t buffer_sizes[] = {sizeof(buffer_a1),
                             sizeof(buffer_a2),
                             sizeof(buffer_var_a2),
                             sizeof(buffer_a3)};

  // Prepare key
  tiledb_kv_keys_t* keys;
  tiledb_kv_keys_create(ctx, &keys);
//  int key_1 = 100;
//  tiledb_kv_keys_add(ctx, keys, &key_1, TILEDB_INT32, sizeof(int));
//  float key_2 = 200.0;
//  tiledb_kv_keys_add(ctx, keys, &key_2, TILEDB_FLOAT32, sizeof(float));
  double key_3[2] = {300.0, 300.1};
  tiledb_kv_keys_add(ctx, keys, key_3, TILEDB_FLOAT64, 2 * sizeof(double));
//  char key_4[] = "key_4";
//  tiledb_kv_keys_add(ctx, keys, key_4, TILEDB_CHAR, strlen(key_4) + 1);

  // Create query
  tiledb_kv_query_t* query;
  tiledb_kv_query_create(
      ctx,
      &query,
      "my_kv",
      TILEDB_READ,
      keys,
      nullptr,
      0,
      buffers,
      buffer_sizes);

  // Submit query
  tiledb_kv_query_submit(ctx, query);


  // Print values
  printf(" a1\t   a2\t      (a3.first, a3.second)\n");
  printf("-----------------------------------------\n");
  printf("%3d", buffer_a1[0]);
  uint64_t var_size = buffer_sizes[2];
  printf("\t %4.*s", int(var_size), &buffer_var_a2[0]);
  printf("\t\t (%5.1f, %5.1f)\n", buffer_a3[0], buffer_a3[1]);

  // Clean up
  tiledb_kv_keys_free(ctx, keys);
  tiledb_kv_query_free(ctx, query);
  tiledb_ctx_free(ctx);

  return 0;
}
