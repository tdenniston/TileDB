/**
 * @file   kv_query.h
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
 * This file implements class KVQuery.
 */

#include "kv_query.h"
#include "logger.h"
#include "md5/md5.h"

/* ****************************** */
/*   CONSTRUCTORS & DESTRUCTORS   */
/* ****************************** */

namespace tiledb {

KVQuery::KVQuery() {
  query_ = nullptr;
}

KVQuery::~KVQuery() {
  // In the case of writes, the last buffer (coords) is created by this object
  if (type_ == QueryType::WRITE)
    std::free(buffers_.back());
  delete query_;
};

/* ****************************** */
/*               API              */
/* ****************************** */

Status KVQuery::init(
    StorageManager* storage_manager,
    const ArrayMetadata* array_metadata,
    const std::vector<FragmentMetadata*>& fragment_metadata,
    QueryType type,
    Keys* keys,
    const char** attributes,
    unsigned int attribute_num,
    void** buffers,
    uint64_t* buffer_sizes) {
  // Set members
  keys_ = keys;
  type_ = type;

  // Set layout and subarray
  Layout layout =
      (type_ == QueryType::WRITE) ? Layout::UNORDERED : Layout::GLOBAL_ORDER;

  // Get attribute ids
  std::vector<unsigned int> attribute_ids;
  RETURN_NOT_OK(get_attribute_ids(
      array_metadata, attributes, attribute_num, &attribute_ids));

  // Set buffers
  RETURN_NOT_OK(
      set_buffers(array_metadata, attribute_ids, buffers, buffer_sizes));

  // Compute coordinates
  if (type == QueryType::WRITE)
    RETURN_NOT_OK(compute_coords());

  // Compute subarray
  void* subarray = nullptr;
  if (type == QueryType::READ)
    RETURN_NOT_OK(compute_subarray(&subarray));

  // Initialize query
  delete query_;
  query_ = new Query();
  Status st = query_->init(
      storage_manager,
      array_metadata,
      fragment_metadata,
      type,
      layout,
      subarray,
      attribute_ids,
      &buffers_[0],
      &buffer_sizes_[0],
      URI());

  // Clean up
  if (subarray != nullptr)
    std::free(subarray);

  return st;
}

Query* KVQuery::query() const {
  return query_;
}

void KVQuery::reset_user_buffer_sizes() {
  for(unsigned int i=0; i<user_buffer_sizes_num_; ++i)
    user_buffer_sizes_[i] = buffer_sizes_[i];
}

/* ****************************** */
/*         PRIVATE METHODS        */
/* ****************************** */

Status KVQuery::compute_coords() const {
  // For easy reference
  auto coords = (uint64_t*)buffers_.back();
  auto keys = (uint64_t*)keys_->keys_data();
  auto keys_var = (unsigned char*)keys_->keys_var_data();
  auto types = (unsigned char*)keys_->types_data();
  auto key_num = keys_->key_num();
  auto keys_var_size = keys_->keys_var_size();

  // Compute an MD5 digest per <key_type | key_size | key> tuple
  md5::MD5_CTX md5_ctx = {};
  uint64_t coords_size = sizeof(md5_ctx.digest);
  assert(coords_size == 2 * sizeof(uint64_t));
  uint64_t key_size;
  for (uint64_t i = 0; i < key_num; ++i) {
    key_size = (i != key_num - 1) ? (keys[i + 1] - keys[i]) :
                                    (keys_var_size - keys[i]);
    md5::MD5Init(&md5_ctx);
    md5::MD5Update(&md5_ctx, &types[i], sizeof(char));
    md5::MD5Update(&md5_ctx, (unsigned char*)&key_size, sizeof(uint64_t));
    md5::MD5Update(&md5_ctx, &keys_var[keys[i]], (unsigned int)key_size);
    md5::MD5Final(&md5_ctx);
    std::memcpy(&coords[2 * i], md5_ctx.digest, coords_size);
  }

  return Status::Ok();
}

Status KVQuery::compute_subarray(void** subarray) const {
  // Allocate memory
  *subarray = malloc(4 * sizeof(uint64_t));
  if (*subarray == nullptr)
    return LOG_STATUS(
        Status::KVQueryError("Failed to allocate memory for subarray"));

  // Sanity check
  if (keys_->key_num() != 1)
    return LOG_STATUS(Status::KVQueryError(
        "Cannot compute subarray; More than one keys provided"));

  // For easy reference
  auto keys_var = (unsigned char*)keys_->keys_var_data();
  auto types = (unsigned char*)keys_->types_data();
  auto keys_var_size = keys_->keys_var_size();

  md5::MD5_CTX md5_ctx = {};
  md5::MD5Init(&md5_ctx);
  md5::MD5Update(&md5_ctx, types, sizeof(char));
  md5::MD5Update(&md5_ctx, (unsigned char*)&keys_var_size, sizeof(uint64_t));
  md5::MD5Update(&md5_ctx, keys_var, (unsigned int)keys_var_size);
  md5::MD5Final(&md5_ctx);
  auto s = (uint64_t*)*subarray;
  std::memcpy(&s[0], md5_ctx.digest, sizeof(uint64_t));
  std::memcpy(&s[1], md5_ctx.digest, sizeof(uint64_t));
  std::memcpy(&s[2], md5_ctx.digest+sizeof(uint64_t), sizeof(uint64_t));
  std::memcpy(&s[3], md5_ctx.digest+sizeof(uint64_t), sizeof(uint64_t));

  return Status::Ok();
}

Status KVQuery::get_attribute_ids(
    const ArrayMetadata* array_metadata,
    const char** attributes,
    unsigned int attribute_num,
    std::vector<unsigned int>* attribute_ids) const {
  // Prepare attributes vector
  std::vector<std::string> attributes_vec;
  if (attributes != nullptr && attribute_num > 0) {
    for (unsigned int i = 0; i < attribute_num; ++i)
      attributes_vec.emplace_back(attributes[i]);

    if (type_ == QueryType::WRITE) {
      attributes_vec.emplace_back(constants::key_attr_name);
      attributes_vec.emplace_back(constants::key_type_attr_name);
      attributes_vec.emplace_back(constants::coords);
    }
  } else {
    attributes_vec = array_metadata->attribute_names();

    if (type_ == QueryType::READ) {
      attributes_vec.pop_back();  // coords
      attributes_vec.pop_back();  // key_type_attr_name
      attributes_vec.pop_back();  // key_attr_name
    }
  }

  // Get attribute ids
  Layout layout =
      (type_ == QueryType::WRITE) ? Layout::UNORDERED : Layout::GLOBAL_ORDER;
  return array_metadata->get_attribute_ids(
      type_, layout, attributes_vec, attribute_ids);
}

Status KVQuery::set_buffers(
    const ArrayMetadata* array_metadata,
    const std::vector<unsigned int>& attribute_ids,
    void** buffers,
    uint64_t* buffer_sizes) {
  // Prepare buffers
  unsigned int buff_i = 0;
  for (auto& attr_id : attribute_ids) {
    auto attr_name = array_metadata->attribute_name(attr_id);
    if (attr_name == constants::key_attr_name) {
      buffer_sizes_.emplace_back(keys_->keys_size());
      buffers_.emplace_back(keys_->keys_data());
      buffer_sizes_.emplace_back(keys_->keys_var_size());
      buffers_.emplace_back(keys_->keys_var_data());
    } else if (attr_name == constants::key_type_attr_name) {
      buffer_sizes_.emplace_back(keys_->types_size());
      buffers_.emplace_back(keys_->types_data());
    } else if (attr_name == constants::coords) {
      uint64_t coords_buff_size =
          keys_->key_num() * array_metadata->coords_size();
      void* coords_buff = malloc(coords_buff_size);
      if (coords_buff == nullptr)
        return LOG_STATUS(Status::StorageManagerError(
            "Failed to allocate coordinates buffer"));
      buffer_sizes_.emplace_back(coords_buff_size);
      buffers_.emplace_back(coords_buff);
    } else {
      buffer_sizes_.emplace_back(buffer_sizes[buff_i]);
      buffers_.emplace_back(buffers[buff_i++]);
      if (array_metadata->var_size(attr_id)) {
        buffer_sizes_.emplace_back(buffer_sizes[buff_i]);
        buffers_.emplace_back(buffers[buff_i++]);
      }
    }
  }

  // Set user buffer sizes
  user_buffer_sizes_ = buffer_sizes;
  user_buffer_sizes_num_ = buff_i;

  return Status::Ok();
}

}  // namespace tiledb
