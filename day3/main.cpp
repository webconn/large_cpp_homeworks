#include <iostream>
#include <vector>
#include <ctime>
#include <functional>
#include <cassert>

struct TItem {
  int value;
  time_t timestamp;

  TItem(int v)
    : value(v)
    , timestamp(std::time(0)) {}

  bool operator==(const TItem& t) {
    return t.value == value && t.timestamp == timestamp;
  }
};

using Items = std::vector<TItem>;

// -----------------------------------------------------------------------------
template<int V, int... Args>
Items MakeItemsSimple() {
  Items v({TItem(V)});
  if constexpr (sizeof...(Args) != 0) {
    Items app = MakeItemsSimple<Args...>();
    v.insert(v.end(), app.begin(), app.end());
  }
  return v;
}


Items items = MakeItemsSimple<0, 1, 4, 5, 6>();
Items newItems = MakeItemsSimple<7, 15, 1>();

// -----------------------------------------------------------------------------
std::function<bool(int)> MakePredicate(Items its) {
  return [its](int it) mutable -> bool {
    if (its.size() == 0) {
      return false;
    } else if (TItem(it) == its.back()) {
      return true;
    } else {
      its.pop_back();
      return MakePredicate(its)(it);
    }
  };
}

// -----------------------------------------------------------------------------


int main(int argc, char **argv) {
  auto isFound = MakePredicate(items);
  auto isFoundNew = MakePredicate(newItems);

  assert(isFound(0) == true);
  assert(isFound(7) == false);
  assert(isFoundNew(7) == true);
  assert(isFoundNew(8) == false);

  return 0;
}
