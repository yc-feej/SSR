# SSR - Super Simple Reflection

An easy-to-use reflection realized under C++14. Only minimum modification is needed for existed codes.

## Introduction

**What's good about SSR?**

1. Users can use a name to create an instance of a class without knowing its type.

2. Users can get "reflected" variables of a class, iterate over them, and get their names, types, and values.

3. You can bind other reflection systems with it - Protobuf reflection, for example.

## Usage

### Definition

Only one line of code is needed to mark the class as using reflection.

```
// in .h file
class ReflectA {
 public:
  USE_REFLECTION_CLASS();
  
  // What is desired to be caught by reflection
  int32_t a_;
  // What is NOT desired to be caught by reflection
  int32_t b_;
};
```

In the realization `.cc` file that included the previous `.h` file, one line of code is also needed to declare its descriptor as well as variables to be reflected.

```
// in .cc file
// First parameter is typename of class, followed by name of reflected variables
ADD_REFLECTION_CLASS_MEMBER(ReflectA, a_);
```

### Actual Use

As the class has been reflected, you can get all info you need through descriptor, or create new instances by type name.

```
// Scenario1: an instance existed
ReflectA a;
auto* desc_a = prophet::reflection::DescriptorAccessor<ReflectA>::Get();
desc_a->GetFieldName(0); // "a_"
desc_a->MutableFieldValueById(&a, 0); // a void* ptr to a_ in a
desc_a->MutableFieldValueByName(&a, "a_"); // a void* ptr to a_ in a

// Scenario2: no instance existed
auto* b = NEW_CLASS_PTR("ReflectA");
auto* desc_b = DESC("ReflectA"); // same as desc_a in scenario 1
desc_b->MutableFieldValueById(b, 0); // a void* ptr to a_ in b
```

### Find deriviation

For OOP, polymorphism is an important feature. If you need to find descriptor of derived class pointed by a base pointer, here's a simple example on how to do so.

```
class BaseClass {
 public:
  // DEFINE REFLECTIONS

  virtual std::string GetClassName() const { return class_name; }

 private:
  std::string class_name{"BaseClass"};
};

class DerivedClassA {
 public:
  // DEFINE REFLECTIONS

  virtual std::string GetClassName() override const { return class_name; }

 private:
  std::string class_name{"DerivedClassA"};
};

class DerivedClassB {
 public:
  // DEFINE REFLECTIONS

  virtual std::string GetClassName() override const { return class_name; }

 private:
  std::string class_name{"DerivedClassB"};
};

// When use
// Assume that we have a std::vector<BaseClass*> class_group
for (auto* class : class_group) {
  auto* class_desc = DESC(class->GetClassName());
}
```

## Supported Pre-defined types

Status | Pre-defined Types
---------|:------:
:heavy_check_mark: | int32_t
:heavy_check_mark: | int64_t
:heavy_check_mark: | float
:heavy_check_mark: | double
:heavy_check_mark: | std::string
:heavy_check_mark: | char
:heavy_check_mark: | glm::vec3
:heavy_check_mark: | google::protobuf::Message

##  Supported containers and template types

Status | Containers
---------|:------:
:heavy_check_mark: | std::vector
:heavy_check_mark: | std::map
:x:  | std::unordered_map
:x:  | std::set
:x:  | std::unordered_set
:x:  | std::list
:x:  | std::deque
:x:  | std::unique_ptr
:x:  | std::shared_ptr
:x:  | std::pair