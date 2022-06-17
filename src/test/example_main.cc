#include <iostream>

#include "basic/reflection/test/test.h"

REGISTER(prophet::ReflectionTestBase, ReflectionTestBase)
REGISTER(prophet::ReflectionTestDerive, ReflectionTestDerive)
REGISTER(prophet::ReflectionTestPtr, ReflectionTestPtr)

void DumpItem(const void* val, prophet::reflection::Descriptor* field_desc) {
  // Dump base types.
  if (field_desc->GetTypeName() == "int32_t") {
    std::cout << "Field val: " << *field_desc->ToTypePtr<int32_t>(val)
              << std::endl;
  } else if (field_desc->GetTypeName() == "float") {
    std::cout << "Field val: " << *field_desc->ToTypePtr<float>(val)
              << std::endl;
  } else if (field_desc->GetTypeName() == "int64_t") {
    std::cout << "Field val: " << *field_desc->ToTypePtr<int64_t>(val)
              << std::endl;
  } else if (field_desc->GetTypeName() == "double") {
    std::cout << "Field val: " << *field_desc->ToTypePtr<double>(val)
              << std::endl;
  } else if (field_desc->GetTypeName() == "std::string") {
    std::cout << "Field val: " << *field_desc->ToTypePtr<std::string>(val)
              << std::endl;
  } else if (field_desc->GetTypeName() == "char") {
    std::cout << "Field val: " << *field_desc->ToTypePtr<char>(val)
              << std::endl;
  } else if (field_desc->GetTypeName() == "glm::vec3") {
    const auto& val_ref = *field_desc->ToTypePtr<glm::vec3>(val);
    std::cout << "Field val: " << val_ref.x << " " << val_ref.y << " "
              << val_ref.z << std::endl;
  } else if (field_desc->IsProtobufMessage()) {
    auto* proto_field_desc =
        prophet::reflection::MessageDescriptor::ToMessageDescriptor(field_desc);
    auto* msg_ptr = proto_field_desc->ToMessage(val);
    std::cout << "Field size: " << msg_ptr->GetDescriptor()->field_count()
              << std::endl;
  }
}

void DumpContainer(void* val, prophet::reflection::Descriptor* field_desc) {
  auto* container_desc =
      prophet::reflection::ContainerDescriptor::ToContainerDescriptor(
          field_desc);
  std::string container_type = container_desc->GetContainerTypeName();
  int32_t container_size = container_desc->GetContainerSize(val);
  std::cout << "Container type: " << container_type << std::endl;
  std::cout << "Container size: " << container_size << std::endl;
  if (container_type == "std::vector<>") {
    std::cout << "Container value: " << std::endl;
    for (int j = 0; j < container_size; ++j) {
      const void* val_ptr = container_desc->GetValueByIndex(val, j);
      DumpItem(val_ptr, container_desc->GetValueDescriptor());
    }
    std::cout << "Test: add a value" << std::endl;
    std::string value = "added";
    container_desc->AddValue(val, &value);
    std::cout << "Test: modify a value" << std::endl;
    auto* val_ptr =
        static_cast<std::string*>(container_desc->MutableValueByIndex(val, 0));
    *val_ptr = "modified";
  } else if (container_type == "std::map<>") {
    std::cout << "Container value: " << std::endl;
    auto keys = container_desc->GetContainerKeys(val);
    auto* key_desc = container_desc->GetKeyDescriptor();
    auto* value_desc = container_desc->GetValueDescriptor();
    for (auto key : keys) {
      const void* value = container_desc->GetValueByKey(val, key);
      std::cout << "Key: ";
      DumpItem(key, key_desc);
      std::cout << "Value: ";
      DumpItem(value, value_desc);
    }
    std::cout << "Test: add a value here (check the difference with next call!)"
              << std::endl;
    std::pair<int32_t, std::string> add_value{3, "testtset"};
    container_desc->AddValue(val, &add_value);
  } else if (container_type == "std::set<>") {
    std::cout << "Container value: " << std::endl;
    for (int j = 0; j < container_size; ++j) {
      const void* val_ptr = container_desc->GetValueByIndex(val, j);
      DumpItem(val_ptr, container_desc->GetValueDescriptor());
    }
    std::string check_val = "def";
    std::cout << "Has value def :"
              << container_desc->HasKeyOrValue(val, &check_val) << std::endl;
  } else if (container_type == "std::unordered_map<>") {
    std::cout << "Container value: " << std::endl;
    auto keys = container_desc->GetContainerKeys(val);
    auto* key_desc = container_desc->GetKeyDescriptor();
    auto* value_desc = container_desc->GetValueDescriptor();
    for (auto key : keys) {
      const void* value = container_desc->GetValueByKey(val, key);
      std::cout << "Key: ";
      DumpItem(key, key_desc);
      std::cout << "Value: ";
      DumpItem(value, value_desc);
    }
  } else if (container_type == "std::unordered_set<>") {
    std::cout << "Add value aaa" << std::endl;
    std::string add_value = "aaa";
    container_desc->AddValue(val, &add_value);
    std::cout << "Has value aaa: "
              << container_desc->HasKeyOrValue(val, &add_value) << std::endl;
  }
}

template <typename T>
void DumpClass(void* object, T* desc) {
  if (object == nullptr) return;

  if (desc->IsTypePreDefined()) return;
  auto* class_desc =
      prophet::reflection::ClassDescriptor::ToClassDescriptor(desc);
  std::cout << "class type:" << class_desc->GetTypeName() << std::endl;
  int32_t size = class_desc->GetFieldSize();
  std::cout << "class field size:" << size << std::endl;

  for (int i = 0; i < size; i++) {
    auto* field_desc = class_desc->GetDescriptorById(i);
    std::cout << "Field index: " << i << std::endl;
    std::cout << "Field name: " << class_desc->GetFieldName(i) << std::endl;
    std::cout << "Field type: " << field_desc->GetTypeName() << std::endl;

    auto* content_ptr = class_desc->MutableFieldValueById(object, i);

    if (field_desc->IsSmartPtr()) {
      std::cout << "Is smart pointer! We parse content instead" << std::endl;
      auto* ptr_desc =
          prophet::reflection::SmartPtrDescriptor::ToSmartPtrDescriptor(
              field_desc);
      content_ptr = ptr_desc->GetMutableRawPtr(content_ptr);
      field_desc = ptr_desc->GetContentDescriptor();
      std::cout << "Content type: " << field_desc->GetTypeName() << std::endl;
    }

    if (!field_desc->IsTypePreDefined()) {
      DumpClass(content_ptr, field_desc);
      continue;
    }

    if (field_desc->IsContainer()) {
      DumpContainer(content_ptr, field_desc);
      continue;
    }

    DumpItem(content_ptr, field_desc);
  }
}

int main(int argc, char** argv) {
  // Base class test
  std::cout << "BASE CLASS EXAMPLE" << std::endl;
  prophet::ReflectionTestBase test1(10, 1000000000, 5.0f, 8.0f, "test1", 'a');
  auto* desc1 = prophet::reflection::DescriptorAccessor<
      prophet::ReflectionTestBase>::Get();
  DumpClass(&test1, desc1);

  std::cout << "-------------" << std::endl;

  // Derived class test
  std::cout << "DERIVED CLASS EXAMPLE" << std::endl;
  prophet::ReflectionTestDerive test2("test2");
  auto* desc2 = prophet::reflection::DescriptorAccessor<
      prophet::ReflectionTestDerive>::Get();
  DumpClass(&test2, desc2);

  std::cout << "-------------" << std::endl;

  // Multiple class instance test
  std::cout << "MULTIPLE CLASS INSTANCE EXAMPLE" << std::endl;
  prophet::ReflectionTestBase test3(20, 2000000000, 10.0f, 16.0f, "test3", 'b',
                                    {2.0f, 4.0f, 6.0f});
  DumpClass(&test3, desc1);

  std::cout << "-----------------" << std::endl;

  // Class add-value test
  std::cout << "CLASS ADD-VALUE EXAMPLE" << std::endl;
  DumpClass(&test2, desc2);

  std::cout << "----------------" << std::endl;

  // Instancing from name test
  std::cout << "CLASS INSTANCING FROM NAME EXAMPLE" << std::endl;
  void* test_base_obj = NEW_CLASS_PTR("prophet::ReflectionTestBase");
  std::cout << "create success" << std::endl;
  prophet::reflection::Descriptor* test_base_desc =
      DESC("prophet::ReflectionTestBase");
  DumpClass(test_base_obj, test_base_desc);

  std::cout << "----------------" << std::endl;

  // Instancing a derived class
  std::cout << "CLASS GET DESCRIPTOR FROM NAME EXAMPLE" << std::endl;
  prophet::ReflectionTestBase* test_derive_obj =
      new prophet::ReflectionTestDerive("test4");
  std::cout << "create success" << std::endl;
  prophet::reflection::Descriptor* test_derive_desc =
      DESC(test_derive_obj->GetClassName());
  DumpClass(test_derive_obj, test_derive_desc);

  std::cout << "----------------" << std::endl;

  // Instancing of plain type
  std::cout << "PLAIN TYPE INSTANCING FROM NAME EXAMPLE" << std::endl;
  void* test_plain_obj = NEW_CLASS_PTR("int32_t");
  int32_t* test_plain_int = static_cast<int32_t*>(test_plain_obj);
  *test_plain_int = 5;
  prophet::reflection::Descriptor* test_plain_desc = DESC("int32_t");
  DumpItem(test_plain_obj, test_plain_desc);

  std::cout << "----------------" << std::endl;

  // Smart pointer class example
  std::cout << "SMART PTR CLASS EXAMPLE" << std::endl;
  prophet::ReflectionTestPtr test_ptr_obj;
  prophet::reflection::Descriptor* test_ptr_desc =
      prophet::reflection::DescriptorAccessor<
          prophet::ReflectionTestPtr>::Get();
  std::cout << "original value: " << test_ptr_obj.get_val3()->at(0) << " "
            << test_ptr_obj.get_val3()->at(1) << std::endl;
  DumpClass(&test_ptr_obj, test_ptr_desc);

  return 0;
}
