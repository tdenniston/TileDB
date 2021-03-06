/**
 * @file   tiledb_sparse_write_unordered_2.cc
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2017 TileDB, Inc.
 * @copyright Copyright (c) 2016 MIT and Intel Corporation
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
 * It shows how to write unordered cells to a sparse array with two write
 * queries.
 *
 * You need to run the following to make this work:
 *
 * ./tiledb_sparse_create
 * ./tiledb_sparse_write_unordered_2
 */

#include <tiledb.h>

int main() {
  // Create TileDB context
  tiledb_ctx_t* ctx;
  tiledb_ctx_create(&ctx, nullptr);

  // Set attributes
  const char* attributes[] = {"a1", "a2", "a3", TILEDB_COORDS};

  // Prepare cell buffers - #1
  int buffer_a1[] = {7, 5, 0};
  uint64_t buffer_a2[] = {0, 4, 6};
  char buffer_var_a2[] = "hhhhffa";
  float buffer_a3[] = {7.1f, 7.2f, 5.1f, 5.2f, 0.1f, 0.2f};
  uint64_t buffer_coords[] = {3, 4, 4, 2, 1, 1};
  void* buffers[] = {
      buffer_a1, buffer_a2, buffer_var_a2, buffer_a3, buffer_coords};
  uint64_t buffer_sizes[] = {
      sizeof(buffer_a1),
      sizeof(buffer_a2),
      sizeof(buffer_var_a2) - 1,  // No need to store the last '\0' character
      sizeof(buffer_a3),
      sizeof(buffer_coords)};

  // Create query
  tiledb_query_t* query;
  tiledb_query_create(ctx, &query, "my_sparse_array", TILEDB_WRITE);
  tiledb_query_set_buffers(ctx, query, attributes, 4, buffers, buffer_sizes);
  tiledb_query_set_layout(ctx, query, TILEDB_UNORDERED);

  // Submit query - #1
  tiledb_query_submit(ctx, query);

  // Prepare cell buffers - #2
  int buffer_a1_2[] = {6, 4, 3, 1, 2};
  uint64_t buffer_a2_2[] = {0, 3, 4, 8, 10};
  char buffer_var_a2_2[] = "gggeddddbbccc";
  float buffer_a3_2[] = {
      6.1f, 6.2f, 4.1f, 4.2f, 3.1f, 3.2f, 1.1f, 1.2f, 2.1f, 2.2f};
  uint64_t buffer_coords_2[] = {3, 3, 3, 1, 2, 3, 1, 2, 1, 4};
  void* buffers_2[] = {
      buffer_a1_2, buffer_a2_2, buffer_var_a2_2, buffer_a3_2, buffer_coords_2};
  uint64_t buffer_sizes_2[] = {
      sizeof(buffer_a1_2),
      sizeof(buffer_a2_2),
      sizeof(buffer_var_a2_2) - 1,  // No need to store the last '\0' character
      sizeof(buffer_a3_2),
      sizeof(buffer_coords_2)};

  // Reset buffers
  tiledb_query_reset_buffers(ctx, query, buffers_2, buffer_sizes_2);

  // Submit query - #2
  tiledb_query_submit(ctx, query);

  // Clean up
  tiledb_query_free(ctx, query);
  tiledb_ctx_free(ctx);

  return 0;
}
