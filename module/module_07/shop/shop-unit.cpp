module;
#include <memory>
module shop;
import util;

ShoppingBag shop() {
  ShoppingBag bag;
  bag.products.push_back(produce("fruit"));
  bag.products.push_back(produce("veggie"));
  return bag;
}