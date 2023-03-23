module;
#include <string>
export module util;

import types;

export
Product produce(const char* product) { return { .value = product }; }