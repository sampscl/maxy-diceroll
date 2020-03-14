//
//  buffer.hpp
//
//
//  Created by Six on 1/9/2020.
//  Copyright 1/9/2020 Six. All rights reserved.
//
#ifndef _GJ_MESSAGES_BUFFER_HPP
#define _GJ_MESSAGES_BUFFER_HPP

#include <string.h> // for memcpy
#include <vector> // for um...vector.
#include <functional> // for std::function
#include <stdint.h> // for int types like uint8_t

///
/// @class Buffer
/// @brief Manages a buffer of memory
/// @tparam T The type of data this buffer represents, must be a built-in
/// data type or an aggregate of built-in data types. In other words,
/// memcpy() must work on T.
/// @details Buffer helps manage memory for data that needs to be aliased,
/// copied, shared, transmitted, and received. It supports a variety of
/// operations that help get these jobs done.
///
template<typename T> class Buffer {
public:

    // type aids
    typedef T value_type; //!< The value type
    typedef value_type* pointer; //!< Pointer to the value type
    typedef const value_type* const_pointer; //!< Pointer to constant value type
    typedef value_type& reference; //!< Reference to value type
    typedef const value_type& const_reference; //!< Constant reference to value type
    typedef std::vector<uint8_t> storage_t; //!< Used to store the data
    typedef typename storage_t::iterator iterator; //!< Used to iterate over the buffer
    typedef typename storage_t::const_iterator const_iterator; //!< Used to iterate over the buffer
    typedef typename storage_t::reverse_iterator reverse_iterator; //!< Used to iterate backwards over the buffer
    typedef typename storage_t::const_reverse_iterator const_reverse_iterator; //!< Used to iterate backwards over the buffer
    typedef typename storage_t::difference_type difference_type; //!< The difference type

    ///
    /// @brief construct an instance, the buffer will not be wrapped
    ///
    Buffer(void) :
        m_Data(),
        m_WrapData(NULL),
        m_WrapDataLen(0),
        m_Wrapped(false) {
    }

    ///
    /// @brief copy an instance, the buffer will not be wrapped
    /// @param[in] right The data to copy
    ///
    Buffer(const_reference right) :
        m_Data(sizeof(value_type)),
        m_WrapData(NULL),
        m_WrapDataLen(0),
        m_Wrapped(false) {
        memcpy(&m_Data.at(0), &right, sizeof(value_type));
    }

    ///
    /// @brief copy an instance, the buffer will not be wrapped
    /// @param[in] right The buffer to copy from
    ///
    Buffer(const Buffer& right) :
    m_Data(right.m_Data),
    m_WrapData(NULL),
    m_WrapDataLen(0),
    m_Wrapped(false) {
    }

    ///
    /// @brief construct an instance, the buffer will not be wrapped
    /// @param[in] buf The location of the data to copy
    /// @param[in] len The length of data to copy
    ///
    Buffer(const void* buf,size_t len) :
    m_Data(len),
    m_WrapData(NULL),
    m_WrapDataLen(0),
    m_Wrapped(false) {
        memcpy(&m_Data.at(0), buf, len);
    }

    ///
    /// @brief construct an instance and hint at the buffer's size
    /// @param[in] len Hint to how big the buffer will need to be (bytes)
    /// @details This will cause the buffer to be created empty, but
    /// give a hint to the underlying system of how much memory the buffer
    /// will eventually need.
    ///
    explicit Buffer(size_t len) :
    m_Data(),
    m_WrapData(NULL),
    m_WrapDataLen(0),
    m_Wrapped(false) {
        m_Data.reserve(len);
    }

    ///
    /// @brief destroy an instance
    ///
    ~Buffer(void) {}

    ///
    /// @brief assign a buffer, a complete duplication is made
    /// @param[in] right The buffer to assign from
    /// @return reference to self
    ///
    Buffer& operator=(const Buffer& right) {
        if(this != &right) {
            m_Data = right.m_Data;
            m_WrapData = right.m_WrapData;
            m_WrapDataLen = right.m_WrapDataLen;
            m_Wrapped = right.m_Wrapped;
            unwrapcopy();
        }
        return *this;
    }

    ///
    /// @brief assign a buffer, a copy is made
    /// @param[in] right The data to assign from
    /// @return reference to self
    ///
    Buffer& operator=(const_reference right) {
        m_Data.resize(sizeof(value_type));
        memcpy(&m_Data.at(0), &right, sizeof(value_type));
        return *this;
    }

    ///
    /// @brief wrap a buffer
    /// @details Wrapping a buffer does not copy memory, it allows this
    /// buffer to reference the memory of another buffer, possibly of a
    /// different type.\n
    /// A wrapped buffer requires the lifetime of the wrapped memory to be
    /// longer than it's own. Also, wrapped buffers do not provide some of
    /// the memory overwrite/overread protection that unwrapped buffers have.
    /// @warning do not resize the source buf in any way until you unwrap
    /// this buffer.
    /// @param[in] buf The buffer to wrap
    template<typename other_t>void wrap(Buffer<other_t>& buf) {

        if(reinterpret_cast<void*>(&buf) == reinterpret_cast<void*>(this)) {
            return;
        }

        wrap(buf.ptr(), buf.size());
    }

    ///
    /// @brief wrap a buffer
    /// @see template<typename other_t>void wrap(Buffer<other_t>& buf)
    /// @tparam other_t The type of the supplied buffer
    /// @warning The supplied buffer is const but this class is not, const
    /// correctness is up to the caller.
    /// @param[in] buf The buffer to wrap
    ///
    template<typename other_t>void wrap(const Buffer<other_t>& buf) {

        if((const void*)&buf == (void*)this) {
            return;
        }

        wrap(buf.ptr(), buf.size());
    }

    ///
    /// @brief wrap a buffer around some raw memory
    /// @param[in] data Pointer to memory to wrap
    /// @param[in] len The length (bytes) of the memory
    ///
    void wrap(void* data, size_t len) {

        unwrap();
        m_Wrapped = true;
        m_WrapData = reinterpret_cast<uint8_t*>(data);
        m_WrapDataLen = len;
    }

    ///
    /// @brief wrap a buffer around some raw memory
    /// @see void wrap(void*, size_t)
    /// @param[in] data Pointer to memory to wrap
    /// @param[in] len The length (bytes) of the memory
    /// @warning The supplied pointer is const but this class is not, const
    /// correctness is up to the caller.
    ///
    void wrap(const void* data, size_t len) {
        wrap(const_cast<void*>(data), len);
    }

    ///
    /// @brief unwrap the buffer
    ///
    void unwrap(void) {
        m_Wrapped = false;
        m_WrapData = NULL;
        m_WrapDataLen = 0;
    }

    ///
    /// @brief unwrap the buffer and copy the data
    /// @details This will copy the wrapped data to internal storage and
    /// unwrap the buffer.
    ///
    void unwrapcopy(void) {

        if(iswrapped()) {
            m_Data.resize(m_WrapDataLen);
            memcpy(&m_Data.at(0), m_WrapData, m_WrapDataLen);
        }
        unwrap();
    }

    ///
    /// @brief Determine if the buffer is wrapped
    /// @return @li true if wrapped, @li false if not wrapped
    ///
    bool iswrapped(void) const { return m_Wrapped; }

    ///
    /// @brief alias an offset in the buffer
    /// @details This allows the buffer to be treated as a different type
    /// based on a byte offset. If this is being used as part of an
    /// accumulation system (like extracting a buffer from a network socket),
    /// then the accumulated parameter is incremented by the size of the
    /// aliased type.
    /// @tparam other_t The aliased type
    /// @param[in] offsetBytes The byte offset into the buffer at which to alias
    /// @param[in,out] accumulated The number of bytes accumulated so far, if not
    ///     NULL then this will be incrememted by sizeof(other_t).
    /// @return Pointer to the aliased type
    template<typename other_t>
    other_t* alias(size_t offsetBytes=0,
                   size_t* accumulated=NULL) {

        if(accumulated) { *accumulated += sizeof(other_t); }
        if (iswrapped()) {
            return reinterpret_cast<other_t*>(&m_WrapData[offsetBytes]);
        }
        else {
            return reinterpret_cast<other_t*>(&m_Data.at(offsetBytes));
        }
    }

    ///
    /// @brief Const version to alias an offset in the buffer
    /// @details This allows the buffer to be treated as a different type
    /// based on a byte offset. If this is being used as part of an
    /// accumulation system (like extracting a buffer from a network socket),
    /// then the accumulated parameter is incremented by the size of the
    /// aliased type.
    /// @tparam other_t The aliased type
    /// @param[in] offsetBytes The byte offset into the buffer at which to alias
    /// @param[in] accumulated The number of bytes accumulated so far, if not
    ///     NULL then this will be incrememted by sizeof(other_t).
    /// @return Pointer to the aliased type
    ///
    template<typename other_t>
    const other_t* alias(size_t offsetBytes=0,
                         size_t* accumulated=NULL) const {

        if(accumulated) { *accumulated += sizeof(other_t); }
        if (iswrapped()) {
            return reinterpret_cast<const other_t*>(&m_WrapData[offsetBytes]);
        }
        else {
            return reinterpret_cast<const other_t*>(&m_Data.at(offsetBytes));
        }
    }

    ///
    /// @brief Realign the buffer to have a new beginning
    /// @details Moves the buffer memory starting at new_beginning to the
    /// front of the buffer and shrinks the buffer size accordingly. Has no
    /// effect on wrapped buffers.
    /// @param[in] new_beginning The byte offset of the new beginning of the
    /// buffer.
    ///
    void realign(size_t new_beginning) {
      if(iswrapped()) { return; }

      const size_t new_size = size() - new_beginning;
      if(0 == new_size) {
        clear();
        return;
      }

      memmove(&m_Data[0], &m_Data[new_beginning], new_size);
      resize(new_size);
    } // end realign

    ///
    /// @brief Copy bytes from the buffer
    /// @param[out] dest The destination pointer
    /// @param[in] amount The number of bytes to copy
    /// @param[in] offsetBytes The byte offset at which to start copying
    /// @param[in] accumulated The number of bytes accumulated so far, if not
    /// NULL then this will be incremented by amount.
    /// @warning Range checking is not performed by this function.
    ///
    void copybytes(void* dest,
              size_t amount,
              size_t offsetBytes=0,
              size_t* accumulated=NULL) const {

        if(accumulated) { *accumulated += amount; }
        if(iswrapped()) {
            memcpy(dest, &m_WrapData[offsetBytes], amount);
        }
        else {
            memcpy(dest, &m_Data.at(offsetBytes), amount);
        }
    }

    ///
    /// @brief Get a raw pointer to the buffer
    /// @return Raw pointer to the buffer
    ///
    pointer ptr(void) {
        return reinterpret_cast<pointer>(alias<uint8_t>(0));
    }

    ///
    /// @brief Get a raw pointer to the buffer
    /// @return Raw pointer to the buffer
    ///
    const_pointer ptr(void) const {
        return reinterpret_cast<const_pointer>(alias<uint8_t>(0));
    }

    ///
    /// @brief Get a reference to the buffer
    /// @return Reference to the buffer
    ///
    reference ref(void) {
        return *alias<value_type>();
    }

    ///
    /// @brief Get a reference to the buffer
    /// @return Reference to the buffer
    ///
    const_reference ref(void) const {
        return *alias<value_type>();
    }

    ///
    /// @brief Treat the buffer as an array and get a reference at an index
    /// @details This will treat the buffer as though it is an array of
    /// value_type and return the value_type at the requested index.
    /// @param[in] index The array index
    /// @returns Reference to the item at array offset index
    ///
    reference array(size_t index) {
        return reinterpret_cast<reference>(ptr()[index]);
    }

    ///
    /// @brief Treat the buffer as an array and get a reference at an index
    /// @details This will treat the buffer as though it is an array of
    /// value_type and return the value_type at the requested index.
    /// @param[in] index The array index
    /// @returns Reference to the item at array offset index
    ///
    const_reference array(size_t index) const {
        return reinterpret_cast<const_reference>(ptr()[index]);
    }

    ///
    /// @brief Treat the buffer as an array and get a pointer at an index
    /// @details This will treat the buffer as though it is an array of
    /// value_type and return the value_type* at the requested index.
    /// @param[in] index The array index
    /// @returns Pointer to the item at array offset index
    ///
    pointer arrayptr(size_t index) {
        return reinterpret_cast<pointer>(&(ptr()[index]));
    }

    ///
    /// @brief Treat the buffer as an array and get a pointer at an index
    /// @details This will treat the buffer as though it is an array of
    /// value_type and return the value_type* at the requested index.
    /// @param[in] index The array index
    /// @returns Pointer to the item at array offset index
    ///
    const_pointer arrayptr(size_t index) const {
        return reinterpret_cast<const_pointer>(&(ptr()[index]));
    }

    ///
    /// @brief Pointer operator
    /// @return a pointer to the start of the buffer
    ///
    const_pointer operator->(void) const {
        return ptr();
    }

    ///
    /// @brief Pointer operator
    /// @return a pointer to the start of the buffer
    ///
    pointer operator->(void) {
        return ptr();
    }

    ///
    /// @brief Dereference operator
    /// @return a reference to the start of the buffer
    ///
    const_reference operator*(void) const {
        return ref();
    }

    ///
    /// @brief Dereference operator
    /// @return a reference to the start of the buffer
    ///
    reference operator*(void) {
        return ref();
    }

    ///
    /// @brief Append to the end of the buffer
    /// @param[in] data Pointer to the data to append
    /// @param[in] len Length of the data to append (bytes)
    /// @return Reference to self
    ///
    Buffer& appendbytes(const void* data,size_t len) {

        const size_t startSize = size();
        resize(startSize + len);
        memcpy(alias<uint8_t>(startSize),data,len);
        return *this;
    }

    ///
    /// @brief Append to the end of the buffer
    /// @tparam other_t Type of the data to append, used to determine the
    /// length to copy
    /// @param[in] data Pointer to the data to append
    /// @return Reference to self
    ///
    template <typename other_t>
    Buffer& append(const other_t* data) {
        return appendbytes(data,sizeof(other_t));
    }

    ///
    /// @brief Append to the end of the buffer
    /// @tparam other_t Type of the data to append, used to determine the
    /// length to copy
    /// @param[in] data Reference to the data to append
    /// @return Reference to self
    ///
    template <typename other_t>
    Buffer& append(const other_t& data) {
        return appendbytes(&data,sizeof(other_t));
    }

    ///
    /// @brief Get the size of the buffer
    /// @return Size of the buffer in bytes
    ///
    size_t size(void) const {
        if (iswrapped()) {
            return m_WrapDataLen;
        }
        else {
            return m_Data.size();
        }
    }

    ///
    /// @brief Resize the buffer
    /// @param[in] size The new size of the buffer (bytes)
    /// @note This does nothing to a wrapped buffer
    ///
    void resize(size_t size) {
        if(!iswrapped()) { // can't resize wrapped buffer
            m_Data.resize(size);
        }
    }

    ///
    /// @brief Get the capacity of the buffer
    /// @returns Maximum number of bytes that the buffer can hold without
    /// suffering a memory reallocation penalty
    /// @note This is not useful for wrapped buffers
    ///
    size_t capacity(void) const { return m_Data.capacity(); }

    ///
    /// @brief Reserve memory for the buffer
    /// @details This hints to the underlying system how big the buffer
    /// will eventually need to be and reserves memory for that size.
    /// @param[in] size The size (bytes) to reserve
    ///
    void reserve(size_t size) { m_Data.reserve(size); }

    ///
    /// @brief Unwrap and empty the buffer
    ///
    void clear(void) {
        m_Data.clear();
        unwrap();
    }

    ///
    /// @brief Determine if the buffer is empty
    /// @return @li true if empty, @li false if not empy
    ///
    bool empty(void) const {

        if(!iswrapped()) {
            return m_Data.empty();
        }
        else {
            return m_WrapDataLen == 0;
        }
    }

    ///
    /// @brief Iterate the buffer byte-by-byte with a callback
    /// @param[in] f The callback function, also supports C++11 lambdas
    ///
    void for_each(std::function<void(uint8_t)> f) {
      for(size_t ndx = 0; ndx < size(); ++ndx) {
        f(*alias<uint8_t>(ndx));
      }
    }

    ///
    /// @brief Get an iterator to the start of the buffer
    /// @note Wrapped buffers do not support iterators
    /// @return iterator
    ///
    iterator begin(void) {
      if(iswrapped()) { return m_Data.end(); }
      return m_Data.begin();
    }

    ///
    /// @brief Get a constant iterator to the start of the buffer
    /// @note Wrapped buffers do not support iterators
    /// @return const_iterator
    ///
    const_iterator begin(void) const {
      if(iswrapped()) { return m_Data.end(); }
      return m_Data.begin();
    }

    ///
    /// @brief Get an iterator to the end of the buffer
    /// @return iterator
    ///
    iterator end(void) {
      return m_Data.end();
    }

    ///
    /// @brief Get a constant iterator to the end of the buffer
    /// @return const_iterator
    ///
    const_iterator end(void) const {
      return m_Data.end();
    }

private:
    storage_t m_Data;
    uint8_t* m_WrapData;
    size_t m_WrapDataLen;
    bool m_Wrapped;
}; // end class Buffer

#endif // not defined _GJ_MESSAGES_BUFFER_HPP
