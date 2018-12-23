#include "product.h"
#include "shop.h"

ProductBase::ProductBase(double price)
  : price_(price)
  , started_(false) {}

ProductBase::~ProductBase() {}

void ProductBase::Attach(IShopWeakPtr w_shop) {
  auto shop = w_shop.lock();

  if (shop) {
    shops_.push_back(shop);
    shop->AddProduct(shared_from_this());
  }
}

void ProductBase::Detach(IShopWeakPtr w_shop) {
  auto shop = w_shop.lock();

  if (shop) {
    shops_.remove_if([&w_shop](const IShopWeakPtr& w) {
      auto lp1 = w.lock();
      auto lp2 = w_shop.lock();
      if (!lp1) return true;
      if (!lp2) return false;
      return lp1 == lp2;
    });
    shop->RemoveProduct(shared_from_this());
  }
}

void ProductBase::ChangePrice(double price) {
  std::lock_guard<std::mutex> g(mutex_);
  price_ = price;
}

void ProductBase::StartSales() {
  started_ = true;
  for (IShopWeakPtr& w_shop : shops_) {
    auto shop = w_shop.lock();
    if (shop) {
      shop->AddProduct(shared_from_this());
    }
  }
}

void ProductBase::StopSales() {
  started_ = false;
  for (IShopWeakPtr& w_shop : shops_) {
    auto shop = w_shop.lock();
    if (shop) {
      shop->RemoveProduct(shared_from_this());
    }
  }
}

double ProductBase::GetPrice() const {
  if (!started_) {
    return -1.0;
  } else {
    std::lock_guard<std::mutex> g(mutex_);
    return price_;
  }
}
