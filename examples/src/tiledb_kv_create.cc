/**
 * @file   tiledb_kv_create.cc
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
 * It shows how to create a key-value store.
 */

#include <tiledb.h>
#include <iostream>

int main() {
  // Create TileDB context
  tiledb_ctx_t* ctx;
  tiledb_ctx_create(&ctx);

  // Create attributes
  tiledb_attribute_t* a1;
  tiledb_attribute_create(ctx, &a1, "a1", TILEDB_INT32);
  tiledb_attribute_set_compressor(ctx, a1, TILEDB_BLOSC, -1);
  tiledb_attribute_set_cell_val_num(ctx, a1, 1);
  tiledb_attribute_t* a2;
  tiledb_attribute_create(ctx, &a2, "a2", TILEDB_CHAR);
  tiledb_attribute_set_compressor(ctx, a2, TILEDB_GZIP, -1);
  tiledb_attribute_set_cell_val_num(ctx, a2, TILEDB_VAR_NUM);
  tiledb_attribute_t* a3;
  tiledb_attribute_create(ctx, &a3, "a3", TILEDB_FLOAT32);
  tiledb_attribute_set_compressor(ctx, a3, TILEDB_ZSTD, -1);
  tiledb_attribute_set_cell_val_num(ctx, a3, 2);

  // Create key-value metadata
  const char* kv_name = "my_kv";
  tiledb_kv_metadata_t* kv_metadata;
  tiledb_kv_metadata_create(ctx, &kv_metadata, kv_name);
  tiledb_kv_metadata_add_attribute(ctx, kv_metadata, a1);
  tiledb_kv_metadata_add_attribute(ctx, kv_metadata, a2);
  tiledb_kv_metadata_add_attribute(ctx, kv_metadata, a3);

  // Check kv metadata
  if (tiledb_kv_metadata_check(ctx, kv_metadata) != TILEDB_OK) {
    std::cout << "Invalid key-value metadata\n";
    return -1;
  }

  // Create key-value store
  tiledb_kv_create(ctx, kv_metadata);

  // Clean up
  tiledb_attribute_free(ctx, a1);
  tiledb_attribute_free(ctx, a2);
  tiledb_attribute_free(ctx, a3);
  tiledb_kv_metadata_free(ctx, kv_metadata);
  tiledb_ctx_free(ctx);

  return 0;
}
