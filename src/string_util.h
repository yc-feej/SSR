// Copyright 2022 Yuchen Liu. All Rights Reserved.
// Author: Yuchen Liu (yuchenliu@deeproute.ai)
#pragma once

#include <sstream>
#include <string>

namespace reflection {

class StringUtil {
 private:
  // String concatenate helper function.
  static inline void AddToStream(std::ostringstream&) {}

  // String concatenate helper function.
  template <typename Str, typename... Args>
  static void AddToStream(std::ostringstream& stream, Str&& value,
                          Args&&... args) {
    stream << std::forward<Str>(value);
    AddToStream(stream, std::forward<Args>(args)...);
  }

 public:
  // Concatenate multiple string(or char*)s into one string. Use AddToStream(&)
  // To add strings recursively.
  template <typename... Args>
  static std::string Concat(Args&&... args) {
    std::ostringstream s;
    AddToStream(s, std::forward<Args>(args)...);
    return s.str();
  }
};

}  // namespace reflection