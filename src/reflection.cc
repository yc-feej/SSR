// Copyright 2020 Deeproute.ai. All Rights Reserved.
// Author: Yuchen Liu (yuchenliu@deeproute.ai)
#include "src/reflection.h"

#include <boost/core/demangle.hpp>

namespace reflection {

void* ClassReflFactory::GetClassByName(const std::string& name) {
  if (util_.find(name) == util_.end()) return nullptr;

  return util_.at(name).func();
}

void* ClassReflFactory::FindClassByTypeInfo(const std::type_info& type_info) {
  std::string typeid_name = boost::core::demangle(type_info.name());
  return GetClassByName(typeid_name);
}

Descriptor* ClassReflFactory::GetDescriptorByName(const std::string& name) {
  if (util_.find(name) == util_.end()) return nullptr;

  return util_.at(name).desc;
}

Descriptor* ClassReflFactory::FindDescriptorByTypeInfo(
    const std::type_info& type_info) {
  std::string typeid_name = boost::core::demangle(type_info.name());
  return GetDescriptorByName(typeid_name);
}

void ClassReflFactory::RegisterClassFunc(const std::string& name,
                                         instance_ptr_creator func) {
  if (util_.find(name) == util_.end()) util_.insert({name, {}});
  util_.at(name).func = func;
}

void ClassReflFactory::RegisterClassDescriptor(const std::string& name,
                                               Descriptor* desc) {
  if (util_.find(name) == util_.end()) util_.insert({name, {}});
  util_.at(name).desc = desc;
}

/* Pre-defined descriptors */

ADD_PRE_DEFINED_DESC(int32_t, Int32);
ADD_PRE_DEFINED_DESC(int64_t, Int64);
ADD_PRE_DEFINED_DESC(float, Float);
ADD_PRE_DEFINED_DESC(double, Double);
ADD_PRE_DEFINED_DESC(bool, Bool);
ADD_PRE_DEFINED_DESC(std::string, String);
ADD_PRE_DEFINED_DESC(char, Char);
ADD_PRE_DEFINED_DESC(glm::vec3, Vec3);
ADD_PRE_DEFINED_DESC(glm::vec4, Vec4);
ADD_PRE_DEFINED_DESC(glm::mat3, Mat3);
ADD_PRE_DEFINED_DESC(glm::mat4, Mat4);
ADD_PRE_DEFINED_DESC(glm::quat, Quaternion);

/* Message descriptor */

template <>
Descriptor* GetDescriptor<google::protobuf::Message>() {
  static MessageDescriptor desc;
  return &desc;
}

/* UniquePtrDescriptor methods */

std::string UniquePtrDescriptor::GetTypeName() const {
  if (desc_ == nullptr) return "";
  return StringUtil::Concat("std::unique_ptr<", desc_->GetTypeName(), ">");
}

std::string UniquePtrDescriptor::GetContentTypeName() const {
  if (desc_ == nullptr) return "";
  return desc_->GetTypeName();
}

void* UniquePtrDescriptor::GetMutableRawPtr(void* obj) { return mut_val_(obj); }

const void* UniquePtrDescriptor::GetRawPtr(const void* obj) const {
  return get_val_(obj);
}

/* SharedPtrDescriptor methods */

std::string SharedPtrDescriptor::GetTypeName() const {
  if (desc_ == nullptr) return "";
  return StringUtil::Concat("std::shared_ptr<", desc_->GetTypeName(), ">");
}

std::string SharedPtrDescriptor::GetContentTypeName() const {
  if (desc_ == nullptr) return "";
  return desc_->GetTypeName();
}

void* SharedPtrDescriptor::GetMutableRawPtr(void* obj) { return mut_val_(obj); }

const void* SharedPtrDescriptor::GetRawPtr(const void* obj) const {
  return get_val_(obj);
}

/* VectorDescriptor methods */

std::string VectorDescriptor::GetTypeName() const {
  return StringUtil::Concat("std::vector<", desc_.value->GetTypeName(), ">");
}

std::string VectorDescriptor::GetContainerTypeName() const {
  return type_name_;
}

size_t VectorDescriptor::GetContainerSize(const void* obj) const {
  return get_size_(obj);
}

const void* VectorDescriptor::GetValueByIndex(const void* obj,
                                              int32_t index) const {
  return get_val_(obj, index);
}

void* VectorDescriptor::MutableValueByIndex(void* obj, int32_t index) {
  return mut_val_(obj, index);
}

bool VectorDescriptor::AddValue(void* obj, const void* val) {
  return add_val_(obj, val);
}

/* MapDescriptor methods */

std::string MapDescriptor::GetTypeName() const {
  return StringUtil::Concat("std::map<", desc_.key->GetTypeName(), ", ",
                            desc_.value->GetTypeName(), ">");
}

std::string MapDescriptor::GetContainerTypeName() const { return type_name_; }

size_t MapDescriptor::GetContainerSize(const void* obj) const {
  return get_size_(obj);
}

std::vector<const void*> MapDescriptor::GetContainerKeys(
    const void* obj) const {
  return get_keys_(obj);
}

const void* MapDescriptor::GetValueByKey(const void* obj,
                                         const void* key) const {
  return get_val_(obj, key);
}

void* MapDescriptor::MutableValueByKey(void* obj, const void* key) {
  return mut_val_(obj, key);
}

bool MapDescriptor::AddValue(void* obj, const void* val) {
  return add_val_(obj, val);
}

bool MapDescriptor::HasKeyOrValue(const void* obj, const void* key_or_value) {
  return has_key_(obj, key_or_value);
}

/* UnorderedMapDescriptor methods */

std::string UnorderedMapDescriptor::GetTypeName() const {
  return StringUtil::Concat("std::unordered_map<", desc_.key->GetTypeName(),
                            ", ", desc_.value->GetTypeName(), ">");
}

std::string UnorderedMapDescriptor::GetContainerTypeName() const {
  return type_name_;
}

size_t UnorderedMapDescriptor::GetContainerSize(const void* obj) const {
  return get_size_(obj);
}

std::vector<const void*> UnorderedMapDescriptor::GetContainerKeys(
    const void* obj) const {
  return get_keys_(obj);
}

const void* UnorderedMapDescriptor::GetValueByKey(const void* obj,
                                                  const void* key) const {
  return get_val_(obj, key);
}

void* UnorderedMapDescriptor::MutableValueByKey(void* obj, const void* key) {
  return mut_val_(obj, key);
}

bool UnorderedMapDescriptor::AddValue(void* obj, const void* val) {
  return add_val_(obj, val);
}

bool UnorderedMapDescriptor::HasKeyOrValue(const void* obj,
                                           const void* key_or_value) {
  return has_key_(obj, key_or_value);
}

/* SetDescriptor methods */
std::string SetDescriptor::GetTypeName() const {
  return StringUtil::Concat("std::set<", desc_.value->GetTypeName(), ">");
}

std::string SetDescriptor::GetContainerTypeName() const { return type_name_; }

size_t SetDescriptor::GetContainerSize(const void* obj) const {
  return get_size_(obj);
}

const void* SetDescriptor::GetValueByIndex(const void* obj,
                                           int32_t index) const {
  return get_val_(obj, index);
}

void* SetDescriptor::MutableValueByIndex(void* obj, int32_t index) {
  return nullptr;
}

bool SetDescriptor::AddValue(void* obj, const void* val) {
  return add_val_(obj, val);
}

bool SetDescriptor::HasKeyOrValue(const void* obj, const void* key_or_value) {
  return has_val_(obj, key_or_value);
}

/* UnorderedSetDescriptor methods */
std::string UnorderedSetDescriptor::GetTypeName() const {
  return StringUtil::Concat("std::unordered_set<", desc_.value->GetTypeName(),
                            ">");
}

std::string UnorderedSetDescriptor::GetContainerTypeName() const {
  return type_name_;
}

size_t UnorderedSetDescriptor::GetContainerSize(const void* obj) const {
  return get_size_(obj);
}

const void* UnorderedSetDescriptor::GetValueByIndex(const void* obj,
                                                    int32_t index) const {
  return nullptr;
}

void* UnorderedSetDescriptor::MutableValueByIndex(void* obj, int32_t index) {
  return nullptr;
}

bool UnorderedSetDescriptor::AddValue(void* obj, const void* val) {
  return add_val_(obj, val);
}

bool UnorderedSetDescriptor::HasKeyOrValue(const void* obj,
                                           const void* key_or_value) {
  return has_val_(obj, key_or_value);
}

/* ClassDescriptor methods */

bool ClassDescriptor::IsTypePreDefined() const { return false; }

int32_t ClassDescriptor::GetFieldSize() const {
  return static_cast<int32_t>(members_.size());
}

std::string ClassDescriptor::GetFieldName(int32_t id) const {
  int32_t size = members_.size();
  if (id >= size) return "";
  return members_.at(id).field_name_;
}

Descriptor* ClassDescriptor::GetDescriptorById(int32_t id) {
  int32_t size = members_.size();
  if (id >= size) return nullptr;
  return members_.at(id).desc_;
}

Descriptor* ClassDescriptor::GetDescriptorByName(const std::string& name) {
  for (auto& member : members_) {
    if (member.field_name_ == name) {
      return member.desc_;
    }
  }
  return nullptr;
}

void* ClassDescriptor::MutableFieldValueById(void* obj, int32_t id) {
  int32_t size = members_.size();
  if (id >= size) return nullptr;

  return members_.at(id).desc_->MutableVal(obj + members_.at(id).offset_);
}

void* ClassDescriptor::MutableFieldValueByName(void* obj,
                                               const std::string& name) {
  for (auto& member : members_) {
    if (member.field_name_ == name) {
      return member.desc_->MutableVal(obj + member.offset_);
    }
  }
  return nullptr;
}

void ClassDescriptor::InternalSetMembers(std::vector<Member>&& members) {
  members_.assign(members.begin(), members.end());
}

}  // namespace reflection