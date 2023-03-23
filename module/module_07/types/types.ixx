module;
#include <string>
#include <vector>
export module types;

export
struct Product {
  std::string value;
};

export
struct ShoppingBag {
  std::vector<Product> products;
};