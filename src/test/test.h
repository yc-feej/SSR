// Copyright 2022 Yuchen Liu. All Rights Reserved.
// Author: Yuchen Liu (yuchenliu@deeproute.ai)
#pragma once

#include "src/reflection.h"
#include "src/test/proto/refl_test.pb.h"

class ReflectionTestBase {
 public:
  USE_REFLECTION_CLASS();

  ReflectionTestBase() = default;
  ReflectionTestBase(int32_t val1, int64_t val2 = 10000000000,
                     float val3 = 5.0f, double val4 = 8.0f,
                     const std::string& val5 = "base", char val6 = 'a',
                     glm::vec3 val7 = glm::vec3(1.0f, 2.0f, 3.0f))
      : val1_(val1),
        val2_(val2),
        val3_(val3),
        val4_(val4),
        val5_(val5),
        val6_(val6),
        val7_(val7) {}
  virtual ~ReflectionTestBase() {}

  virtual std::string GetClassName() { return class_name_; }

 private:
  int32_t val1_{50};
  int64_t val2_;
  float val3_;
  double val4_;
  std::string val5_;
  char val6_;
  glm::vec3 val7_;

  std::string class_name_{"ReflectionTestBase"};
};

class ReflectionTestDerive : public ReflectionTestBase {
 public:
  USE_REFLECTION_CLASS();

  ReflectionTestDerive() = default;
  ReflectionTestDerive(const std::string& val1) : val1_(val1) {}
  virtual ~ReflectionTestDerive() {}

  virtual std::string GetClassName() override { return class_name_; }

 private:
  std::string val1_;
  std::vector<std::string> val2_{"ttt", "fff"};
  std::map<int32_t, std::string> val3_{{1, "a"}, {2, "b"}};
  reflection::test::ReflTestProtoA val4_;
  reflection::test::ReflTestProtoB val5_;

  std::set<std::string> val6_{"abc", "def", "ggg"};
  std::unordered_map<int32_t, std::string> val7_{{3, "a"}, {4, "b"}};
  std::unordered_set<std::string> val8_{"xyz", "zxy"};

  std::string class_name_{"ReflectionTestDerive"};
};

class ReflectionTestPtr {
 public:
  USE_REFLECTION_CLASS();

  ReflectionTestPtr()
      : val1_(new int32_t(5)),
        val2_(new std::string("abc")),
        val3_(new std::vector<std::string>({"abc", "bcd"})) {}

  std::vector<std::string>* get_val3() { return val3_.get(); }

  std::unique_ptr<int32_t> val1_;
  std::unique_ptr<std::string> val2_;
  std::unique_ptr<std::vector<std::string>> val3_;
};
