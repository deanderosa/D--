#ifndef UTILS_H_
#define UTILS_H_

#include <memory>
#include <vector>

template<typename T, typename... Ptrs>
std::vector<std::unique_ptr<T>> make_vector(Ptrs&&... ptrs) {
  std::vector<std::unique_ptr<T>> vec;
  (vec.emplace_back(std::forward<Ptrs>(ptrs)), ...);
  return vec;
}

#endif
