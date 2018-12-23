#include "shop.h"
#include "product.h"

void ShopBase::AddProduct(IProductPtr prod) {
  std::lock_guard<std::mutex> g(mutex_);
  products_.insert(prod);
}

void ShopBase::RemoveProduct(IProductPtr prod) {
  std::lock_guard<std::mutex> g(mutex_);
  products_.erase(products_.find(prod));
}

double ShopBase::Sell(IProductWeakPtr w_prod) {
  IProductPtr prod;

  {
    std::lock_guard<std::mutex> g(mutex_);
    auto f = products_.find(w_prod);
    if (f != products_.end()) {
      prod = w_prod.lock();
    } else {
      return -1.0;
    }
  }

  return prod->GetPrice();
}
