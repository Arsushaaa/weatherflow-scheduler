#pragma once

#include <string>

namespace ml {

class Exception {
private:
   std::string message_;

public:
   explicit Exception(const std::string& message)
      : message_(message) 
   {}

   const char* what() const noexcept {
      return message_.c_str();
   }
};

} // namespace ml