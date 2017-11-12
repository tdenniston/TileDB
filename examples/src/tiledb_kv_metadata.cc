/**
 * @file   tiledb_kv_metadata.cc
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
 * This example explores the C API for the key-value metadata.
 */

#include <tiledb.h>

int main() {
  // Create TileDB context
  tiledb_ctx_t* ctx;
  tiledb_ctx_create(&ctx);

  // Create key-value metadata
  tiledb_kv_metadata_t* kv_metadata;
  tiledb_kv_metadata_create(ctx, &kv_metadata, "my_kv");

  // Print key-value metadata contents
  printf("First dump:\n");
  tiledb_kv_metadata_dump(ctx, kv_metadata, stdout);

  // Add attributes
  tiledb_attribute_t *a1, *a2;
  tiledb_attribute_create(ctx, &a1, "a1", TILEDB_INT32);
  tiledb_attribute_create(ctx, &a2, "a2", TILEDB_FLOAT32);
  tiledb_attribute_set_cell_val_num(ctx, a1, 3);
  tiledb_attribute_set_compressor(ctx, a2, TILEDB_GZIP, -1);
  tiledb_kv_metadata_add_attribute(ctx, kv_metadata, a1);
  tiledb_kv_metadata_add_attribute(ctx, kv_metadata, a2);

  // Print key-value metadata contents again
  printf("\nSecond dump:\n");
  tiledb_kv_metadata_dump(ctx, kv_metadata, stdout);

  // Get some values using getters
  const char* kv_name;
  tiledb_kv_metadata_get_name(ctx, kv_metadata, &kv_name);

  // Print from getters
  printf("\nFrom getters:\n");
  printf("- Key-value store name: %s\n", kv_name);

  // Print the attribute names using iterators
  printf("\nArray metadata attribute names: \n");
  tiledb_attribute_iter_t* attr_iter;
  const tiledb_attribute_t* attr;
  const char* attr_name;
  tiledb_kv_attribute_iter_create(ctx, kv_metadata, &attr_iter);
  int done;
  tiledb_attribute_iter_done(ctx, attr_iter, &done);
  while (done != 1) {
    tiledb_attribute_iter_here(ctx, attr_iter, &attr);
    tiledb_attribute_get_name(ctx, attr, &attr_name);
    printf("* %s\n", attr_name);
    tiledb_attribute_iter_next(ctx, attr_iter);
    tiledb_attribute_iter_done(ctx, attr_iter, &done);
  }
  printf("\n");

  // Print the raw underlying sparse array metadata of the key-value store
  printf("\nUnderlying sparse array metadata:\n");
  tiledb_kv_metadata_dump_as_array(ctx, kv_metadata, stdout);

  // Clean up
  tiledb_attribute_free(ctx, a1);
  tiledb_attribute_free(ctx, a2);
  tiledb_attribute_iter_free(ctx, attr_iter);
  tiledb_kv_metadata_free(ctx, kv_metadata);
  tiledb_ctx_free(ctx);

  return 0;
}
