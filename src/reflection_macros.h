// Copyright 2022 Yuchen Liu. All Rights Reserved.
// Author: Yuchen Liu (yuchenliu@deeproute.ai)
#pragma once

#include <string>

namespace reflection {

#define USE_REFLECTION_CLASS()                       \
  friend class reflection::DescriptorAccessorHelper; \
  static reflection::ClassDescriptor reflection;     \
  static void InitReflection(reflection::ClassDescriptor*);

#define REGISTER(class_name, string_name)                    \
  void* CreateClassInstance##string_name() {                 \
    return static_cast<void*>(new class_name);               \
  }                                                          \
  reflection::ClassReflRegister register_func_##string_name( \
      #class_name, CreateClassInstance##string_name);        \
  reflection::ClassReflRegister register_desc_##string_name( \
      #class_name, reflection::DescriptorAccessor<class_name>::Get());

#define ADD_REFLECTION_CLASS_MEMBER(class_name, ...)                   \
  reflection::ClassDescriptor class_name::reflection{                  \
      class_name::InitReflection};                                     \
  void class_name::InitReflection(reflection::ClassDescriptor* desc) { \
    desc->type_name_ = #class_name;                                    \
    desc->size_ = sizeof(class_name);                                  \
    desc->InternalSetMembers({ADD_MEMBER(class_name, ##__VA_ARGS__)}); \
  }

#define ADD_PRE_DEFINED_DESC(type_name, class_name) \
  template <>                                       \
  Descriptor* GetDescriptor<type_name>() {          \
    static class_name##Descriptor desc;             \
    return &desc;                                   \
  }                                                 \
  REGISTER(type_name, class_name);

#define NEW_CLASS_PTR(class_type) \
  reflection::ClassReflFactory::Get().GetClassByName(class_type)

#define DESC(class_type) \
  reflection::ClassReflFactory::Get().GetDescriptorByName(class_type)

// Given a pointer to base class. Return actual descriptor of derived class
// using RTTI.
#define DESC_BY_BASE_PTR(base_class_ptr)                           \
  reflection::ClassReflFactory::Get().GetDescriptorByBaseClassPtr( \
      base_class_ptr)

#define DESC_BY_THIS(this_ptr)    \
  reflection::DescriptorAccessor< \
      typename std::remove_reference<decltype(*this_ptr)>::type>::Get()

#define TO_CLASS_DESC(desc) reflection::ClassDescriptor::ToClassDescriptor(desc)

#define ADD_MEMBER_LINE(class_name, a) \
  {#a, offsetof(class_name, a),        \
   reflection::DescriptorAccessor<decltype(class_name::a)>::Get()},

#define ADD_MEMBER_1(class_name)

#define ADD_MEMBER_2(class_name, a) ADD_MEMBER_LINE(class_name, a)

#define ADD_MEMBER_3(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)         \
  ADD_MEMBER_2(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_4(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)         \
  ADD_MEMBER_3(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_5(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)         \
  ADD_MEMBER_4(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_6(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)         \
  ADD_MEMBER_5(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_7(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)         \
  ADD_MEMBER_6(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_8(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)         \
  ADD_MEMBER_7(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_9(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)         \
  ADD_MEMBER_8(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_10(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)          \
  ADD_MEMBER_9(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_11(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)          \
  ADD_MEMBER_10(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_12(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)          \
  ADD_MEMBER_11(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_13(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)          \
  ADD_MEMBER_12(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_14(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)          \
  ADD_MEMBER_13(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_15(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)          \
  ADD_MEMBER_14(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_16(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)          \
  ADD_MEMBER_15(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_17(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)          \
  ADD_MEMBER_16(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_18(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)          \
  ADD_MEMBER_17(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_19(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)          \
  ADD_MEMBER_18(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_20(class_name, a, ...) \
  ADD_MEMBER_LINE(class_name, a)          \
  ADD_MEMBER_19(class_name, ##__VA_ARGS__)

#define ADD_MEMBER_SELECTOR(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, _11, _12, \
                            _13, _14, _15, _16, _17, _18, _19, _20, ADD_FUNC,  \
                            ...)                                               \
  ADD_FUNC

#define ADD_MEMBER(...)                                                        \
  ADD_MEMBER_SELECTOR(                                                         \
      __VA_ARGS__, ADD_MEMBER_20, ADD_MEMBER_19, ADD_MEMBER_18, ADD_MEMBER_17, \
      ADD_MEMBER_16, ADD_MEMBER_15, ADD_MEMBER_14, ADD_MEMBER_13,              \
      ADD_MEMBER_12, ADD_MEMBER_11, ADD_MEMBER_10, ADD_MEMBER_9, ADD_MEMBER_8, \
      ADD_MEMBER_7, ADD_MEMBER_6, ADD_MEMBER_5, ADD_MEMBER_4, ADD_MEMBER_3,    \
      ADD_MEMBER_2, ADD_MEMBER_1)                                              \
  (__VA_ARGS__)

}  // namespace reflection