// Copyright 2022 Yuchen Liu. All Rights Reserved.
// Author: Yuchen Liu (yuchenliu@deeproute.ai)
#pragma once

#include <functional>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <typeinfo>
#include <vector>

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "google/protobuf/message.h"
#include "src/descriptor_base.h"
#include "src/reflection_macros.h"
#include "src/string_util.h"

namespace reflection {

/**
 * Reflection is an essential tool for user-defined class to be flexiblly
 * handled. In our realization, we allow users to initialize a REFLECTED class
 * without knowing its actual type, but only type string of the class; we also
 * allow users to get field size/name/value inside any REFLECTED class using
 * Descriptors. Class needs only minimum adaption to use those features.
 *
 * How-to-define:
 * - in .h file:
 *   class A {
 *     USE_REFLECTION_CLASS();
 *
 *     // a variable that needs to be reflected
 *     int32_t a_;
 *     // a variable that doesn't need to be reflected
 *     int32_t b_;
 *   };
 *
 * - in .cc file:
 *   // add you class name, as well as all class variables need to be reflected
 *   ADD_REFLECTION_CLASS_MEMBER(A, a_);
 *
 * How-to-use:
 * - in main.cc file:
 *   //...
 *   A a;
 *   auto* desc_a = prophet::reflection::DescriptorAccessor<A>::Get();
 *   desc_a->GetFieldName(0); // "a_"
 *   desc_a->MutableFieldValueById(&a, 0); // a void* ptr to a_ in a
 *   //...
 *   auto* b = NEW_CLASS_PTR("A");
 *   auto* desc_b = DESC("A"); // same with desc_b
 *   desc_b->MutableFieldValueById(b, 0); // a void* ptr to a_ in b
 *
 *   You can also use prophet::reflection::ClassReflFactory to get new class
 * instances and descriptors.
 */

typedef std::function<void*(void)> instance_ptr_creator;

struct ClassReflUtil {
 public:
  ClassReflUtil() {}

  instance_ptr_creator func{[]() { return nullptr; }};
  Descriptor* desc{nullptr};
};

class ClassReflFactory {
 public:
  explicit ClassReflFactory(const ClassReflFactory&) = delete;
  explicit ClassReflFactory(ClassReflFactory&&) noexcept = delete;
  ClassReflFactory& operator=(const ClassReflFactory&) = delete;
  ClassReflFactory& operator=(ClassReflFactory&&) noexcept = delete;
  static ClassReflFactory& Get() {
    static ClassReflFactory instance;
    return instance;
  }

  void* GetClassByName(const std::string& name);

  void* FindClassByTypeInfo(const std::type_info& type_info);

  Descriptor* GetDescriptorByName(const std::string& name);

  Descriptor* FindDescriptorByTypeInfo(const std::type_info& type_info);

  template <typename T>
  Descriptor* GetDescriptorByBaseClassPtr(T* t) {
    return FindDescriptorByTypeInfo(typeid(*t));
  }

  void RegisterClassFunc(const std::string& name, instance_ptr_creator func);

  void RegisterClassDescriptor(const std::string& name, Descriptor* desc);

 private:
  ClassReflFactory() = default;

  std::map<std::string, ClassReflUtil> util_;
};

class ClassReflRegister {
 public:
  ClassReflRegister(const std::string& name, instance_ptr_creator func) {
    ClassReflFactory::Get().RegisterClassFunc(name, func);
  }

  ClassReflRegister(const std::string& name, Descriptor* desc) {
    ClassReflFactory::Get().RegisterClassDescriptor(name, desc);
  }
};

// Using SFINAE (substitution failure is not an error) here to determine if we
// are reaching out an pre-defined descriptor or ClassDescriptor.
class DescriptorAccessorHelper {
 public:
  template <typename T>
  static char Dummy(decltype(&T::reflection));

  template <typename T>
  static int Dummy(...);

  template <typename T>
  struct HasReflection {
    enum { value = (sizeof(Dummy<T>(nullptr)) == sizeof(char)) };
  };

  template <typename T,
            typename std::enable_if<HasReflection<T>::value, int>::type = 0>
  static Descriptor* Get() {
    return &T::reflection;
  }

  template <typename T,
            typename std::enable_if<!HasReflection<T>::value, int>::type = 0>
  static Descriptor* Get() {
    // Handle protobuf-defined message with special care, and use reflection
    // provided by protobuf.
    if (std::is_base_of<google::protobuf::Message, T>::value) {
      return GetDescriptor<google::protobuf::Message>();
    }
    return GetDescriptor<T>();
  }
};

// An interface for users to get Descriptor* when having a specific type.
// Each time a new pre-defined type & container is supported, this class should
// be reloaded manually (see descriptor_base.h/.cc, as well as VectorDescriptor
// below).
template <typename T>
class DescriptorAccessor {
 public:
  static Descriptor* Get() { return DescriptorAccessorHelper::Get<T>(); }
};

class Int32Descriptor : public Descriptor {
 public:
  Int32Descriptor() : Descriptor("int32_t", sizeof(int32_t)) {}
};

class Int64Descriptor : public Descriptor {
 public:
  Int64Descriptor() : Descriptor("int64_t", sizeof(int64_t)) {}
};

class FloatDescriptor : public Descriptor {
 public:
  FloatDescriptor() : Descriptor("float", sizeof(float)) {}
};

class DoubleDescriptor : public Descriptor {
 public:
  DoubleDescriptor() : Descriptor("double", sizeof(double)) {}
};

class BoolDescriptor : public Descriptor {
 public:
  BoolDescriptor() : Descriptor("bool", sizeof(bool)) {}
};
class StringDescriptor : public Descriptor {
 public:
  StringDescriptor() : Descriptor("std::string", sizeof(std::string)) {}
};

class CharDescriptor : public Descriptor {
 public:
  CharDescriptor() : Descriptor("char", sizeof(char)) {}
};

class Vec3Descriptor : public Descriptor {
 public:
  Vec3Descriptor() : Descriptor("glm::vec3", sizeof(glm::vec3)) {}
};

class Vec4Descriptor : public Descriptor {
 public:
  Vec4Descriptor() : Descriptor("glm::vec4", sizeof(glm::vec4)) {}
};

class Mat3Descriptor : public Descriptor {
 public:
  Mat3Descriptor() : Descriptor("glm::mat3", sizeof(glm::mat3)) {}
};

class Mat4Descriptor : public Descriptor {
 public:
  Mat4Descriptor() : Descriptor("glm::mat4", sizeof(glm::mat4)) {}
};

class QuaternionDescriptor : public Descriptor {
 public:
  QuaternionDescriptor() : Descriptor("glm::quat", sizeof(glm::quat)) {}
};

class MessageDescriptor : public Descriptor {
 public:
  MessageDescriptor()
      : Descriptor("google::protobuf::Message", sizeof(int32_t)) {}

  virtual bool IsProtobufMessage() const override { return true; }

  static google::protobuf::Message* ToMessage(void* content_ptr) {
    return static_cast<google::protobuf::Message*>(content_ptr);
  }

  static const google::protobuf::Message* ToMessage(const void* content_ptr) {
    return static_cast<const google::protobuf::Message*>(content_ptr);
  }

  static MessageDescriptor* ToMessageDescriptor(Descriptor* desc) {
    return dynamic_cast<MessageDescriptor*>(desc);
  }
};

class SmartPtrDescriptor : public Descriptor {
 public:
  SmartPtrDescriptor(const std::string& name, size_t size, Descriptor* desc)
      : Descriptor(name, size), desc_(desc) {}
  virtual ~SmartPtrDescriptor() {}

  virtual std::string GetTypeName() const = 0;

  virtual bool IsSmartPtr() const override { return true; }

  virtual std::string GetSmartPtrTypeName() const { return type_name_; }

  virtual std::string GetContentTypeName() const = 0;

  virtual void* GetMutableRawPtr(void* obj) = 0;

  virtual const void* GetRawPtr(const void* obj) const = 0;

  virtual Descriptor* GetContentDescriptor() { return desc_; }

  static SmartPtrDescriptor* ToSmartPtrDescriptor(Descriptor* desc) {
    return dynamic_cast<SmartPtrDescriptor*>(desc);
  }

  Descriptor* desc_;
};

class UniquePtrDescriptor final : public SmartPtrDescriptor {
 public:
  template <typename Type>
  UniquePtrDescriptor(Type* type)
      : SmartPtrDescriptor("std::unique_ptr<>", sizeof(std::unique_ptr<Type>),
                           reflection::DescriptorAccessor<Type>::Get()) {
    get_val_ = [](const void* ptr) -> const void* {
      const auto* smart_ptr = static_cast<const std::unique_ptr<Type>*>(ptr);
      if (smart_ptr == nullptr) return nullptr;
      return smart_ptr->get();
    };
    mut_val_ = [](void* ptr) -> void* {
      auto* smart_ptr = static_cast<std::unique_ptr<Type>*>(ptr);
      if (smart_ptr == nullptr) return nullptr;
      return smart_ptr->get();
    };
  }

  ~UniquePtrDescriptor() {}

  virtual std::string GetTypeName() const override;

  virtual std::string GetContentTypeName() const override;

  virtual void* GetMutableRawPtr(void* obj) override;

  virtual const void* GetRawPtr(const void* obj) const override;

 protected:
  std::function<const void*(const void*)> get_val_;
  std::function<void*(void*)> mut_val_;
};

template <typename Value>
class DescriptorAccessor<std::unique_ptr<Value>> {
 public:
  static Descriptor* Get() {
    static UniquePtrDescriptor desc(static_cast<Value*>(nullptr));
    return &desc;
  }
};

class SharedPtrDescriptor final : public SmartPtrDescriptor {
 public:
  template <typename Type>
  SharedPtrDescriptor(Type* type)
      : SmartPtrDescriptor("std::shared_ptr<>", sizeof(std::shared_ptr<Type>),
                           reflection::DescriptorAccessor<Type>::Get()) {
    get_val_ = [](const void* ptr) -> const void* {
      const auto* smart_ptr = static_cast<const std::shared_ptr<Type>*>(ptr);
      if (smart_ptr == nullptr) return nullptr;
      return smart_ptr->get();
    };
    mut_val_ = [](void* ptr) -> void* {
      auto* smart_ptr = static_cast<std::shared_ptr<Type>*>(ptr);
      if (smart_ptr == nullptr) return nullptr;
      return smart_ptr->get();
    };
  }

  ~SharedPtrDescriptor() {}

  virtual std::string GetTypeName() const override;

  virtual std::string GetContentTypeName() const override;

  virtual void* GetMutableRawPtr(void* obj) override;

  virtual const void* GetRawPtr(const void* obj) const override;

 protected:
  std::function<const void*(const void*)> get_val_;
  std::function<void*(void*)> mut_val_;
};

template <typename Value>
class DescriptorAccessor<std::shared_ptr<Value>> {
 public:
  static Descriptor* Get() {
    static SharedPtrDescriptor desc(static_cast<Value*>(nullptr));
    return &desc;
  }
};

// Base class of containers. Provide robust interface for different containers
// to get and modify their values.
class ContainerDescriptor : public Descriptor {
 public:
  template <typename... Args>
  ContainerDescriptor(const std::string& name, size_t size, Args&&... args)
      : Descriptor(name, size), desc_(std::forward<Args>(args)...) {}
  virtual ~ContainerDescriptor() {}

  // Get type name of current described target. For STL containers, template
  // typename will be included too, e.g. std::vector<std::string>.
  virtual std::string GetTypeName() const = 0;

  virtual bool IsContainer() const override { return true; }

  // Get container name of current described target, without template typename,
  // e.g. std::vector<>.
  virtual std::string GetContainerTypeName() const = 0;

  virtual size_t GetContainerSize(const void* obj) const = 0;

  // Get const pointer to certain value in container by index. Return nullptr
  // for invalid containers.
  virtual const void* GetValueByIndex(const void* obj, int32_t index) const {
    return nullptr;
  }

  // Get non-const pointer to certain value in container by index. Return
  // nullptr for invalid containers.
  virtual void* MutableValueByIndex(void* obj, int32_t index) {
    return nullptr;
  }

  // Get key list of container. For containers without key (vector, set, ...),
  // return an empty vector.
  virtual std::vector<const void*> GetContainerKeys(const void* obj) const {
    return std::vector<const void*>();
  }

  // Get const pointer to certain value in container by key. Return nullptr for
  // invalid containers.
  virtual const void* GetValueByKey(const void* obj, const void* key) const {
    return nullptr;
  }

  // Get non-const pointer to certain value in container by key. Return nullptr
  // for invalid containers.
  virtual void* MutableValueByKey(void* obj, const void* key) {
    return nullptr;
  }

  // Add a value to container. Actual type of second parameter is determined by
  // container type. Return a bool to indicate add status.
  virtual bool AddValue(void* obj, const void* val) = 0;

  // Check whether a key exists (for map-liked containers) or a value exists
  // (for set-liked containers) for search-targetted containers. Return a bool
  // to indicate check status. Time compexity may vary for different containers.
  virtual bool HasKeyOrValue(const void* obj, const void* key_or_value) {
    return false;
  }

  // Get descriptor of key type. Return nullptr if key doesn't exist.
  virtual Descriptor* GetKeyDescriptor() { return desc_.key; }

  // Get descriptor of value type. Works for all containers (we consider items
  // in vector, set, ... as value).
  virtual Descriptor* GetValueDescriptor() { return desc_.value; }

  // Cast a Descriptor* to ContainerDescriptor*. Return nullptr if failed.
  static ContainerDescriptor* ToContainerDescriptor(Descriptor* desc) {
    return dynamic_cast<ContainerDescriptor*>(desc);
  }

  DescPair desc_;
};

class VectorDescriptor : public ContainerDescriptor {
 public:
  template <typename Type>
  VectorDescriptor(Type* type)
      : ContainerDescriptor("std::vector<>", sizeof(std::vector<Type>),
                            reflection::DescriptorAccessor<Type>::Get()) {
    get_size_ = [](const void* ctn) -> size_t {
      const auto* ctn_ptr = static_cast<const std::vector<Type>*>(ctn);
      if (ctn_ptr == nullptr) return -1;
      return ctn_ptr->size();
    };
    get_val_ = [](const void* ctn, int32_t index) -> const void* {
      const auto* ctn_ptr = static_cast<const std::vector<Type>*>(ctn);
      if (ctn_ptr == nullptr) return nullptr;
      int32_t size = ctn_ptr->size();
      if (size <= index) return nullptr;
      return &ctn_ptr->at(index);
    };
    mut_val_ = [](void* ctn, int32_t index) -> void* {
      auto* ctn_ptr = static_cast<std::vector<Type>*>(ctn);
      if (ctn_ptr == nullptr) return nullptr;
      int32_t size = ctn_ptr->size();
      if (size <= index) return nullptr;
      return &ctn_ptr->at(index);
    };
    add_val_ = [](void* ctn, const void* val) -> bool {
      auto* ctn_ptr = static_cast<std::vector<Type>*>(ctn);
      const auto* val_ptr = static_cast<const Type*>(val);
      if (ctn_ptr == nullptr || val_ptr == nullptr) return false;
      // TODO(yuchen): requires further optimization for vector<unique_ptr>.
      // ctn_ptr->push_back(std::move(*val_ptr));
      return true;
    };
  }
  virtual ~VectorDescriptor() {}

  virtual std::string GetTypeName() const override;

  virtual std::string GetContainerTypeName() const override;

  virtual size_t GetContainerSize(const void* obj) const override;

  virtual const void* GetValueByIndex(const void* obj,
                                      int32_t index) const override;

  virtual void* MutableValueByIndex(void* obj, int32_t index) override;

  virtual bool AddValue(void* obj, const void* val) override;

 protected:
  std::function<size_t(const void*)> get_size_;
  std::function<const void*(const void*, int32_t)> get_val_;
  std::function<void*(void*, int32_t)> mut_val_;
  std::function<bool(void*, const void*)> add_val_;
};

template <typename Value>
class DescriptorAccessor<std::vector<Value>> {
 public:
  static Descriptor* Get() {
    static VectorDescriptor desc(static_cast<Value*>(nullptr));
    return &desc;
  }
};

class MapDescriptor : public ContainerDescriptor {
 public:
  template <typename Key, typename Value>
  MapDescriptor(Key* key_type, Value* value_type)
      : ContainerDescriptor("std::map<>", sizeof(std::map<Key, Value>),
                            reflection::DescriptorAccessor<Key>::Get(),
                            reflection::DescriptorAccessor<Value>::Get()) {
    get_size_ = [](const void* ctn) -> size_t {
      const auto* ctn_ptr = static_cast<const std::map<Key, Value>*>(ctn);
      if (ctn_ptr == nullptr) return -1;
      return ctn_ptr->size();
    };
    get_keys_ = [](const void* ctn) -> std::vector<const void*> {
      const auto* ctn_ptr = static_cast<const std::map<Key, Value>*>(ctn);
      if (ctn_ptr == nullptr) return std::vector<const void*>();
      std::vector<const void*> keys;
      for (auto iter = ctn_ptr->begin(); iter != ctn_ptr->end(); ++iter) {
        keys.emplace_back(&iter->first);
      }
      return keys;
    };
    get_val_ = [](const void* ctn, const void* key) -> const void* {
      const auto* ctn_ptr = static_cast<const std::map<Key, Value>*>(ctn);
      const auto* key_ptr = static_cast<const Key*>(key);
      if (ctn_ptr == nullptr || key_ptr == nullptr) return nullptr;
      if (ctn_ptr->find(*key_ptr) == ctn_ptr->end()) return nullptr;
      return &ctn_ptr->at(*key_ptr);
    };
    has_key_ = [](const void* ctn, const void* key) -> bool {
      const auto* ctn_ptr = static_cast<const std::map<Key, Value>*>(ctn);
      const auto* key_ptr = static_cast<const Key*>(key);
      if (ctn_ptr == nullptr || key_ptr == nullptr) return false;
      if (ctn_ptr->find(*key_ptr) == ctn_ptr->end()) return false;
      return true;
    };
    mut_val_ = [](void* ctn, const void* key) -> void* {
      auto* ctn_ptr = static_cast<std::map<Key, Value>*>(ctn);
      const auto* key_ptr = static_cast<const Key*>(key);
      if (ctn_ptr == nullptr || key_ptr == nullptr) return nullptr;
      if (ctn_ptr->find(*key_ptr) == ctn_ptr->end()) return nullptr;
      return &ctn_ptr->at(*key_ptr);
    };
    add_val_ = [](void* ctn, const void* val) -> bool {
      auto* ctn_ptr = static_cast<std::map<Key, Value>*>(ctn);
      const auto* val_ptr = static_cast<const std::pair<Key, Value>*>(val);
      if (ctn_ptr == nullptr || val_ptr == nullptr) return false;
      ctn_ptr->insert(*val_ptr);
      return true;
    };
  }
  virtual ~MapDescriptor() {}

  virtual std::string GetTypeName() const override;

  virtual std::string GetContainerTypeName() const override;

  virtual size_t GetContainerSize(const void* obj) const override;

  virtual std::vector<const void*> GetContainerKeys(
      const void* obj) const override;

  virtual const void* GetValueByKey(const void* obj,
                                    const void* key) const override;

  virtual void* MutableValueByKey(void* obj, const void* key) override;

  virtual bool AddValue(void* obj, const void* val) override;

  virtual bool HasKeyOrValue(const void* obj,
                             const void* key_or_value) override;

 protected:
  std::function<size_t(const void*)> get_size_;
  std::function<std::vector<const void*>(const void*)> get_keys_;
  std::function<const void*(const void*, const void*)> get_val_;
  std::function<bool(const void*, const void*)> has_key_;
  std::function<void*(void*, const void*)> mut_val_;
  std::function<bool(void*, const void*)> add_val_;
};

template <typename Key, typename Value>
class DescriptorAccessor<std::map<Key, Value>> {
 public:
  static Descriptor* Get() {
    static MapDescriptor desc(static_cast<Key*>(nullptr),
                              static_cast<Value*>(nullptr));
    return &desc;
  }
};

class UnorderedMapDescriptor : public ContainerDescriptor {
 public:
  template <typename Key, typename Value>
  UnorderedMapDescriptor(Key* key_type, Value* value_type)
      : ContainerDescriptor("std::unordered_map<>",
                            sizeof(std::unordered_map<Key, Value>),
                            reflection::DescriptorAccessor<Key>::Get(),
                            reflection::DescriptorAccessor<Value>::Get()) {
    get_size_ = [](const void* ctn) -> size_t {
      const auto* ctn_ptr =
          static_cast<const std::unordered_map<Key, Value>*>(ctn);
      if (ctn_ptr == nullptr) return -1;
      return ctn_ptr->size();
    };
    get_keys_ = [](const void* ctn) -> std::vector<const void*> {
      const auto* ctn_ptr =
          static_cast<const std::unordered_map<Key, Value>*>(ctn);
      if (ctn_ptr == nullptr) return std::vector<const void*>();
      std::vector<const void*> keys;
      for (auto iter = ctn_ptr->begin(); iter != ctn_ptr->end(); ++iter) {
        keys.emplace_back(&iter->first);
      }
      return keys;
    };
    get_val_ = [](const void* ctn, const void* key) -> const void* {
      const auto* ctn_ptr =
          static_cast<const std::unordered_map<Key, Value>*>(ctn);
      const auto* key_ptr = static_cast<const Key*>(key);
      if (ctn_ptr == nullptr || key_ptr == nullptr) return nullptr;
      if (ctn_ptr->find(*key_ptr) == ctn_ptr->end()) return nullptr;
      return &ctn_ptr->at(*key_ptr);
    };
    has_key_ = [](const void* ctn, const void* key) -> bool {
      const auto* ctn_ptr =
          static_cast<const std::unordered_map<Key, Value>*>(ctn);
      const auto* key_ptr = static_cast<const Key*>(key);
      if (ctn_ptr == nullptr || key_ptr == nullptr) return false;
      if (ctn_ptr->find(*key_ptr) == ctn_ptr->end()) return false;
      return true;
    };
    mut_val_ = [](void* ctn, const void* key) -> void* {
      auto* ctn_ptr = static_cast<std::unordered_map<Key, Value>*>(ctn);
      const auto* key_ptr = static_cast<const Key*>(key);
      if (ctn_ptr == nullptr || key_ptr == nullptr) return nullptr;
      if (ctn_ptr->find(*key_ptr) == ctn_ptr->end()) return nullptr;
      return &ctn_ptr->at(*key_ptr);
    };
    add_val_ = [](void* ctn, const void* val) -> bool {
      auto* ctn_ptr = static_cast<std::unordered_map<Key, Value>*>(ctn);
      const auto* val_ptr = static_cast<const std::pair<Key, Value>*>(val);
      if (ctn_ptr == nullptr || val_ptr == nullptr) return false;
      ctn_ptr->insert(*val_ptr);
      return true;
    };
  }
  virtual ~UnorderedMapDescriptor() {}

  virtual std::string GetTypeName() const override;

  virtual std::string GetContainerTypeName() const override;

  virtual size_t GetContainerSize(const void* obj) const override;

  virtual std::vector<const void*> GetContainerKeys(
      const void* obj) const override;

  virtual const void* GetValueByKey(const void* obj,
                                    const void* key) const override;

  virtual void* MutableValueByKey(void* obj, const void* key) override;

  virtual bool AddValue(void* obj, const void* val) override;

  virtual bool HasKeyOrValue(const void* obj,
                             const void* key_or_value) override;

 protected:
  std::function<size_t(const void*)> get_size_;
  std::function<std::vector<const void*>(const void*)> get_keys_;
  std::function<const void*(const void*, const void*)> get_val_;
  std::function<bool(const void*, const void*)> has_key_;
  std::function<void*(void*, const void*)> mut_val_;
  std::function<bool(void*, const void*)> add_val_;
};

template <typename Key, typename Value>
class DescriptorAccessor<std::unordered_map<Key, Value>> {
 public:
  static Descriptor* Get() {
    static UnorderedMapDescriptor desc(static_cast<Key*>(nullptr),
                                       static_cast<Value*>(nullptr));
    return &desc;
  }
};

class SetDescriptor : public ContainerDescriptor {
 public:
  template <typename Type>
  SetDescriptor(Type* type)
      : ContainerDescriptor("std::set<>", sizeof(std::set<Type>),
                            reflection::DescriptorAccessor<Type>::Get()) {
    get_size_ = [](const void* ctn) -> size_t {
      const auto* ctn_ptr = static_cast<const std::set<Type>*>(ctn);
      if (ctn_ptr == nullptr) return -1;
      return ctn_ptr->size();
    };
    get_val_ = [](const void* ctn, int32_t index) -> const void* {
      const auto* ctn_ptr = static_cast<const std::set<Type>*>(ctn);
      if (ctn_ptr == nullptr) return nullptr;
      int32_t size = ctn_ptr->size();
      if (size <= index) return nullptr;
      int i = 0;
      for (auto iter = ctn_ptr->begin(); iter != ctn_ptr->end(); ++iter) {
        if (i == index) return &(*iter);
        ++i;
      }
      return nullptr;
    };
    has_val_ = [](const void* ctn, const void* val) -> bool {
      const auto* ctn_ptr = static_cast<const std::set<Type>*>(ctn);
      const auto* val_ptr = static_cast<const Type*>(val);
      if (ctn_ptr == nullptr || val_ptr == nullptr) return false;
      if (ctn_ptr->find(*val_ptr) == ctn_ptr->end()) return false;
      return true;
    };
    add_val_ = [](void* ctn, const void* val) -> bool {
      auto* ctn_ptr = static_cast<std::set<Type>*>(ctn);
      const auto* val_ptr = static_cast<const Type*>(val);
      if (ctn_ptr == nullptr || val_ptr == nullptr) return false;
      // TODO(yuchen): requires further optimization for vector<unique_ptr>.
      // ctn_ptr->push_back(*val_ptr);
      return true;
    };
  }
  virtual ~SetDescriptor() {}

  virtual std::string GetTypeName() const override;

  virtual std::string GetContainerTypeName() const override;

  virtual size_t GetContainerSize(const void* obj) const override;

  virtual const void* GetValueByIndex(const void* obj,
                                      int32_t index) const override;

  virtual void* MutableValueByIndex(void* obj, int32_t index) override;

  virtual bool AddValue(void* obj, const void* val) override;

  virtual bool HasKeyOrValue(const void* obj,
                             const void* key_or_value) override;

 protected:
  std::function<size_t(const void*)> get_size_;
  std::function<const void*(const void*, int32_t)> get_val_;
  std::function<bool(const void*, const void*)> has_val_;
  std::function<bool(void*, const void*)> add_val_;
};

template <typename Value>
class DescriptorAccessor<std::set<Value>> {
 public:
  static Descriptor* Get() {
    static SetDescriptor desc(static_cast<Value*>(nullptr));
    return &desc;
  }
};

class UnorderedSetDescriptor : public ContainerDescriptor {
 public:
  template <typename Type>
  UnorderedSetDescriptor(Type* type)
      : ContainerDescriptor("std::unordered_set<>",
                            sizeof(std::unordered_set<Type>),
                            reflection::DescriptorAccessor<Type>::Get()) {
    get_size_ = [](const void* ctn) -> size_t {
      const auto* ctn_ptr = static_cast<const std::unordered_set<Type>*>(ctn);
      if (ctn_ptr == nullptr) return -1;
      return ctn_ptr->size();
    };
    has_val_ = [](const void* ctn, const void* val) -> bool {
      const auto* ctn_ptr = static_cast<const std::unordered_set<Type>*>(ctn);
      const auto* val_ptr = static_cast<const Type*>(val);
      if (ctn_ptr == nullptr || val_ptr == nullptr) return false;
      if (ctn_ptr->find(*val_ptr) == ctn_ptr->end()) return false;
      return true;
    };
    add_val_ = [](void* ctn, const void* val) -> bool {
      auto* ctn_ptr = static_cast<std::unordered_set<Type>*>(ctn);
      const auto* val_ptr = static_cast<const Type*>(val);
      if (ctn_ptr == nullptr || val_ptr == nullptr) return false;
      // TODO(yuchen): requires further optimization for vector<unique_ptr>.
      // ctn_ptr->push_back(*val_ptr);
      return true;
    };
  }
  virtual ~UnorderedSetDescriptor() {}

  virtual std::string GetTypeName() const override;

  virtual std::string GetContainerTypeName() const override;

  virtual size_t GetContainerSize(const void* obj) const override;

  virtual const void* GetValueByIndex(const void* obj,
                                      int32_t index) const override;

  virtual void* MutableValueByIndex(void* obj, int32_t index) override;

  virtual bool AddValue(void* obj, const void* val) override;

  virtual bool HasKeyOrValue(const void* obj,
                             const void* key_or_value) override;

 protected:
  std::function<size_t(const void*)> get_size_;
  std::function<bool(const void*, const void*)> has_val_;
  std::function<bool(void*, const void*)> add_val_;
};

template <typename Value>
class DescriptorAccessor<std::unordered_set<Value>> {
 public:
  static Descriptor* Get() {
    static UnorderedSetDescriptor desc(static_cast<Value*>(nullptr));
    return &desc;
  }
};

struct Member {
 public:
  std::string field_name_;
  size_t offset_;
  Descriptor* desc_;
};

// Class descriptor definitions. All classes marked as 'using reflection' will
// instantiate its own class descriptor instance with recorded members.
class ClassDescriptor : public Descriptor {
 public:
  ClassDescriptor(std::function<void(ClassDescriptor*)> init) : Descriptor() {
    init(this);
  }
  virtual ~ClassDescriptor() {}

  virtual bool IsTypePreDefined() const override;

  // Get size of fields marked as using reflection in class.
  virtual int32_t GetFieldSize() const;

  // Get variable name of a certain reflected field.
  virtual std::string GetFieldName(int32_t id) const;

  // Get descriptor of a certain reflected field by id.
  virtual Descriptor* GetDescriptorById(int32_t id);

  // Get descriptor of a certain reflected field by variable name.
  virtual Descriptor* GetDescriptorByName(const std::string& name);

  // Get non-const pointer to certain value in class by id.
  virtual void* MutableFieldValueById(void* obj, int32_t id);

  // Get non-const pointer to certain value in class by variable name.
  virtual void* MutableFieldValueByName(void* obj, const std::string& name);

  // **FOR INTERNAL USE ONLY**
  void InternalSetMembers(std::vector<Member>&& members);

 public:
  // Cast a Descriptor* to ClassDescriptor*. Return nullptr if failed.
  static ClassDescriptor* ToClassDescriptor(Descriptor* desc) {
    return dynamic_cast<ClassDescriptor*>(desc);
  }

 private:
  std::vector<Member> members_;
};

}  // namespace reflection
