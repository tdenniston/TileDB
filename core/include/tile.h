/**
 * @file   tile.h
 * @author Stavros Papadopoulos <stavrosp@csail.mit.edu>
 *
 * @section LICENSE
 *
 * The MIT License
 *
 * @copyright Copyright (c) 2014 Stavros Papadopoulos <stavrosp@csail.mit.edu>
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
 * This file defines class Tile.
 */

#ifndef TILE_H
#define TILE_H

#include "special_values.h"
#include <inttypes.h>
#include <limits>
#include <string>
#include <typeinfo>
#include <vector>

/** Default payload capacity. */
#define TL_PAYLOAD_CAPACITY 100

/** 
 * The tile is the central notion of TileDB. A tile can be a coordinate tile
 * or an attribute tile. In both cases, the cell values are stored sequentially
 * in main memory. We collectively call the cell values as the tile payload.
 *
 * Each tile has a particular cell type, which is one of char, int, int64_t,
 * float, and double for attribute tiles, and int, int64_t, float, and double
 * for coordinate tiles. In order to avoid the tedious templates and for
 * performance purposes, we use a generic void* variable for pointing 
 * to the payload, namely Tile::payload_. The same stands for other member
 * attributes, such as Tile::mbr_. In other words, we make the Tile object
 * oblivious of the cell type. We store the cell type in Tile::cell_type_,
 * and properly cast the void pointers whenever necessary.
 */
class Tile {
 public:
  // TYPE DEFINITIONS
  /** A tile can be either an attribute or a coordinate tile. */
  enum TileType {ATTRIBUTE, COORDINATE};

  // CONSTRUCTORS AND DESTRUCTORS
  /**
   * Constructor. If dim_num is 0, then this is an attribute tile; otherwise,
   * it is a coordinate tile. The val_num indicates how many values are 
   * stored per cell. It is equal to VAR_SIZE if the cell size is variable. 
   */
  Tile(int64_t tile_id, int dim_num, 
       const std::type_info* cell_type, int val_num); 
  /** Destructor. */
  ~Tile(); 

  // ACCESSORS
  /** 
   * Returns the bounding coordinates, i.e., the first and 
   * last coordinates that were appended to the tile. It applies only to
   * coordinate tiles. The bounding coordinates are typically useful
   * when the cells in the tile are sorted in a certain order. 
   */
  std::pair<const void*, const void*> bounding_coordinates() const;
  /** Returns the pointer to the pos-th cell. */
  const void* cell(int64_t pos) const;
  /** Returns the cell size. Applicable only to fixed-sized cells. */
  size_t cell_size() const;
  /** Returns the number of cells in the tile. */
  int64_t cell_num() const { return cell_num_; }
  /** Returns the cell type. */
  const std::type_info* cell_type() const { return cell_type_; } 
  /** Copies the tile payload into buffer. */
  void copy_payload(void* buffer) const;
  /** Returns the number of dimensions. It returns 0 for attribute tiles. */
  int dim_num() const { return dim_num_; }
  /** Returns true if the cell at position pos represents a deletion. */
  bool is_del(int64_t pos) const;
  /** Returns true if the cell at position pos is NULL. */
  bool is_null(int64_t pos) const;
  /** Returns the MBR (see Tile::mbr_). */
  const void* mbr() const { return mbr_; }
  /** Returns the tile id. */
  int64_t tile_id() const { return tile_id_; }
  /** Returns the tile size (in bytes). */
  size_t tile_size() const { return tile_size_; } 
  /** Returns the tile type. */
  TileType tile_type() const { return tile_type_; } 
  /** Returns the cell type size. */
  size_t type_size() const { return type_size_; } 
  /** True if the cells are variable-sized. */
  bool var_size() const { return val_num_ == VAR_SIZE; }
 
  // MUTATORS
  /** Clears the tile. */
  void clear();
  /** MBR setter. Applicable only to coordinate tiles. */
  void set_mbr(const void* mbr);
  /** Payload setter. */
  void set_payload(void* payload, size_t payload_size); 

  // MISC
  /** 
   * Returns true if the pos-th coordinates fall inside the input range. 
   * The range is in the form (dim#1_lower, dim#1_upper, ...).
   * It applies only to coordinate tiles.
   * 
   * Make sure that type T is the same as the cell type. Otherwise, in debug
   * mode an assert is triggered, whereas in release mode the behavior is
   * unpredictable.
   */
  template<class T>
  bool cell_inside_range(int64_t pos, const T* range) const;
  /** Prints the details of the tile on the standard output. */
  void print() const;

  // CELL ITERATOR
  /** This class implements a constant cell iterator. */
  class const_cell_iterator {
   public:
    // CONSTRUCTORS & DESTRUCTORS
    /** Empty iterator constuctor. */
    const_cell_iterator();
    /** 
     * Constuctor that takes as input the tile for which the 
     * iterator is created, and a cell position in the tile payload. 
     */
    const_cell_iterator(const Tile* tile, int64_t pos);
    
    // ACCESSORS
    /** Returns the (potentially variable) size of the current cell. */
    size_t cell_size() const;
    /** Returns the cell type of the tile. */
    const std::type_info* cell_type() const { return tile_->cell_type(); }
    /** Returns the number of dimensions of the tile. */
    int dim_num() const { return tile_->dim_num(); }
    /** Returns true if the end of the iterator is reached. */
    bool end() const { return end_; }
    /** Returns the current payload position of the cell iterator. */
    int64_t pos() const { return pos_; }
    /** Returns the tile the cell iterator belongs to. */
    const Tile* tile() const { return tile_; }
    /** Returns the tile the cell iterator belongs to. */
    int64_t tile_id() const { return tile_->tile_id(); }

    // MISC
    /** True if the iterator points to a cell representing a deletion. */
    bool is_del() const;
    /** True if the iterator points to a NULL cell. */
    bool is_null() const;

    // OPERATORS
    /** Assignment operator. */
    void operator=(const const_cell_iterator& rhs);
    /** Addition operator. */
    const_cell_iterator operator+(int64_t step);
    /** Addition-assignment operator. */
    void operator+=(int64_t step);
    /** Pre-increment operator. */
    const_cell_iterator operator++();
    /** Post-increment operator. */
    const_cell_iterator operator++(int junk);
    /**
     * Returns true if the operands belong to the same tile and they point to
     * the same cell. 
     */
    bool operator==(const const_cell_iterator& rhs) const;
    /**
     * Returns true if either the operands belong to different tiles, or the
     * they point to different cells. 
     */
    bool operator!=(const const_cell_iterator& rhs) const;
    /** Returns the pointer in the tile payload of the cell it points to. */ 
    const void* operator*() const;

    // MISC
    /** 
     * Returns true if the coordinates pointed by the iterator fall 
     * inside the input range. It applies only to coordinate tiles.
     * The range is in the form (dim#1_lower, dim#1_upper, ...).
     * 
     * Make sure that type T is the same as the cell type. Otherwise, in debug
     * mode an assert is triggered, whereas in release mode the behavior is
     * unpredictable.
     */
    template<class T>
    bool cell_inside_range(const T* range) const;

   private:
    /** The current cell. */
    const void* cell_;
    /** True if the end of the iterator is reached. */
    bool end_;
    /** 
     * The position of the cell in the tile payload the iterator
     * currently is pointing to. 
     */
    int64_t pos_;  
    /** The tile object the iterator is created for. */
    const Tile* tile_;
  };
  /** Returns a cell iterator pointing to the first cell of the tile. */
  const_cell_iterator begin() const;
  /** Returns a cell iterator signifying the end of the tile. */
  static const_cell_iterator end();

  // REVERSE CELL ITERATOR
  /** This class implements a constant reverse cell iterator. */
  class const_reverse_cell_iterator {
   public:
    // CONSTRUCTORS & DESTRUCTORS
    /** Empty iterator constuctor. */
    const_reverse_cell_iterator();
    /** 
     * Constuctor that takes as input the tile for which the 
     * iterator is created, and a cell position in the tile payload. 
     */
    const_reverse_cell_iterator(const Tile* tile, int64_t pos);
    
    // ACCESSORS
    /** Returns the (potentially variable) size of the current cell. */
    size_t cell_size() const;
    /** Returns the cell type of the tile. */
    const std::type_info* cell_type() const { return tile_->cell_type(); }
    /** Returns the number of dimensions of the tile. */
    int dim_num() const { return tile_->dim_num(); }
    /** Returns true if the end of the iterator is reached. */
    bool end() const { return end_; }
    /** Returns the current payload position of the cell iterator. */
    int64_t pos() const { return pos_; }
    /** Returns the tile the cell iterator belongs to. */
    const Tile* tile() const { return tile_; }
    /** Returns the tile the cell iterator belongs to. */
    int64_t tile_id() const { return tile_->tile_id(); }

    // MISC
    /** True if the iterator points to a cell representing a deletion. */
    bool is_del() const;
    /** True if the iterator points to a NULL cell. */
    bool is_null() const;

    // OPERATORS
    /** Assignment operator. */
    void operator=(const const_reverse_cell_iterator& rhs);
    /** Addition operator. */
    const_reverse_cell_iterator operator+(int64_t step);
    /** Addition-assignment operator. */
    void operator+=(int64_t step);
    /** Pre-increment operator. */
    const_reverse_cell_iterator operator++();
    /** Post-increment operator. */
    const_reverse_cell_iterator operator++(int junk);
    /**
     * Returns true if the operands belong to the same tile and they point to
     * the same cell. 
     */
    bool operator==(const const_reverse_cell_iterator& rhs) const;
    /**
     * Returns true if either the operands belong to different tiles, or the
     * they point to different cells. 
     */
    bool operator!=(const const_reverse_cell_iterator& rhs) const;
    /** Returns the pointer in the tile payload of the cell it points to. */ 
    const void* operator*() const;

    // MISC
    /** 
     * Returns true if the coordinates pointed by the iterator fall 
     * inside the input range. It applies only to coordinate tiles.
     * The range is in the form (dim#1_lower, dim#1_upper, ...).
     * 
     * Make sure that type T is the same as the cell type. Otherwise, in debug
     * mode an assert is triggered, whereas in release mode the behavior is
     * unpredictable.
     */
    template<class T>
    bool cell_inside_range(const T* range) const;

   private:
    /** The current cell. */
    const void* cell_;
    /** True if the end of the iterator is reached. */
    bool end_;
    /** 
     * The position of the cell in the tile payload the iterator
     * currently is pointing to. 
     */
    int64_t pos_;  
    /** The tile object the iterator is created for. */
    const Tile* tile_;
  };
  /** Returns a cell iterator pointing to the first cell of the tile. */
  const_reverse_cell_iterator rbegin() const;
  /** Returns a cell iterator signifying the end of the tile. */
  static const_reverse_cell_iterator rend();

 private:
  // PRIVATE ATTRIBUTES
  /** The number of cells in the tile. */
  int64_t cell_num_;
  /** The cell size. For variable-sized cells, it is equal to VAR_SIZE. */
  size_t cell_size_;
  /** The cell type. */
  const std::type_info* cell_type_;
  /** The number of dimensions. It is equal to 0 for attribute tiles. */
  int dim_num_; 
  /** 
   * The tile MBR (minimum bounding rectangle), i.e., the tightest 
   * hyper-rectangle in the logical space that contains all the 
   * coordinates in the tile. The MBR is represented as a vector
   * of lower/upper pairs of values in each dimension, i.e., 
   * (dim#1_lower, dim#1_upper, dim#2_lower, dim#2_upper, ...). Applicable
   * only for coordinate tiles (otherwise, it is set to NULL).
   */
  void* mbr_;
  /** 
   * The payload stores the cell (attribute/coordinate) values. 
   * The coordinates are serialized, i.e., the payload first stores
   * the coordinates for dimension 1, then for dimensions 2, etc.
   */
  void* payload_;
  /** The tile id. */
  int64_t tile_id_;
  /** The tile size (in bytes). */
  size_t tile_size_;
  /** The tile type. */
  TileType tile_type_;
  /** The size of the cell type. */
  size_t type_size_;
  /** 
   * Number of cell values. It is equal to VAR_SIZE, if the cell has a
   * variable number of values.
   */
  int val_num_;

  // PRIVATE METHODS
  /** Deletes the MBR. */
  void clear_mbr();
  /** Deletes the payload. */
  void clear_payload();
  /** Expands the tile MBR bounds. Applicable only to coordinate tiles. */
  template<class T>
  void expand_mbr(const T* coords);  
  /** 
   * This is populated only in the case of variable-sized cells. It is a list
   * of offsets where each cell begins in the payload.
   */
  std::vector<size_t> offsets_;
  /** Prints the bounding coordinates on the standard output. */
  template<class T>
  void print_bounding_coordinates() const;
  /** Prints the MBR on the standard output. */
  template<class T>
  void print_mbr() const;
  /** Prints the payload on the standard output. */
  template<class T>
  void print_payload() const;
};

#endif