// Copyright 2022 Yuchen Liu. All Rights Reserved.
// Author: Yuchen Liu (yuchenliu@deeproute.ai)
#pragma once

#include <functional>
#include <string>

namespace reflection {

/** Base class to all descriptors. It provides reloaded interfaces for users to
 * determine its type. You can cast it to ContainerDescriptor or ClassDescriptor
 * when certain rules fulfilled. You can also visit raw values of pre-defined
 * types without extra casting.
 */
class Descriptor {
 public:
  Descriptor() {}
  explicit Descriptor(const std::string& type_name, size_t size)
      : type_name_(type_name), size_(size) {}
  virtual ~Descriptor() {}

  virtual std::string GetTypeName() const { return type_name_; }

  // If target is a pre-defined type (a POD type or a data structure
  // supported by reflection system). Return false for classes using reflection,
  // which means that if it returns false, you can cast this decriptor to
  // ClassDescriptor.
  virtual bool IsTypePreDefined() const { return true; };

  // If target is a supported container. If it returns true, you can cast this
  // descriptor to ContainerDescriptor.
  virtual bool IsContainer() const { return false; }

  // If target is a derived class of google::protobuf::Message. If it returns
  // true, you can cast this descriptor to MessageDescriptor, and use static
  // function to get Message* instead of void* to use reflection provided by
  // protobuf.
  virtual bool IsProtobufMessage() const { return false; }

  // If target is a supported smart pointer. If it returns true, you can cast
  // this descriptor to SmartPtrDescriptor.
  virtual bool IsSmartPtr() const { return false; }

  // Get raw value.
  virtual void* MutableVal(void* obj) { return obj; }

  // Get pointer to value in its type.
  template <typename T>
  T* ToTypePtr(void* val_ptr) {
    return static_cast<T*>(val_ptr);
  }

  // Get const pointer to value in its type.
  template <typename T>
  const T* ToTypePtr(const void* val_ptr) {
    return static_cast<const T*>(val_ptr);
  }

  std::string type_name_;
  size_t size_;
};

template <typename T>
Descriptor* GetDescriptor();

struct DescPair {
 public:
  DescPair(Descriptor* value_) : key(nullptr), value(value_) {}
  DescPair(Descriptor* key_, Descriptor* value_) : key(key_), value(value_) {}

  Descriptor* key;
  Descriptor* value;
};

}  // namespace reflection