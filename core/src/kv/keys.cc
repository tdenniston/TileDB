/**
 * @file   keys.cc
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
 * This file implements class Keys.
 */

#include "keys.h"

namespace tiledb {

/* ********************************* */
/*     CONSTRUCTORS & DESTRUCTORS    */
/* ********************************* */

Keys::Keys() {
  key_num_ = 0;
}

Keys::~Keys() = default;

/* ********************************* */
/*                API                */
/* ********************************* */

Status Keys::add_key(void* key, Datatype type, uint64_t size) {
  uint64_t offset = keys_var_.size();
  RETURN_NOT_OK(keys_.write(&offset, sizeof(size)));
  RETURN_NOT_OK(keys_var_.write(key, size));
  RETURN_NOT_OK(types_.write(&type, sizeof(type)));
  ++key_num_;

  return Status::Ok();
}

uint64_t Keys::key_num() const {
  return key_num_;
}

void* Keys::keys_data() const {
  return keys_.data();
}

void* Keys::keys_var_data() const {
  return keys_var_.data();
}

uint64_t Keys::keys_size() const {
  return keys_.size();
}

uint64_t Keys::keys_var_size() const {
  return keys_var_.size();
}

void* Keys::types_data() const {
  return types_.data();
}

uint64_t Keys::types_size() const {
  return types_.size();
}

}  // namespace tiledb
