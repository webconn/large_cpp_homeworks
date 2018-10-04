#include <algorithm>
#include <functional>
#include <string>
#include <vector>
#include <iostream>
#include <cstdlib>

const char* f2(const std::string& str) {
  return str.c_str();
}

template<typename TF1>
auto Compose(TF1&& f) {
  return std::bind(std::forward<TF1>(f), std::placeholders::_1);
}

template<typename TF1, typename... TFuncs>
auto Compose(TF1&& f1, TFuncs... funcs) {
  return std::bind(std::forward<TF1>(f1), Compose(std::forward<TFuncs>(funcs)...));
}

int main() {
  std::string s[] = { "1.2", "2.343", "3.2" };
  std::vector<double> d(3);

  auto f1 = atof;

  std::transform(s, s + 3, d.begin(), Compose(f1, f2));

  std::cout << d[0] << d[1] << d[2] << std::endl;
}
