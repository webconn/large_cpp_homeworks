#pragma once

#include <memory>
#include <set>
#include <mutex>

class IProduct;
using IProductPtr = std::shared_ptr<IProduct>;
using IProductWeakPtr = std::weak_ptr<IProduct>;

class IShop {
 public:
  virtual ~IShop() {}

  virtual void AddProduct(IProductPtr prod) = 0;
  virtual void RemoveProduct(IProductPtr prod) = 0;
  virtual double Sell(IProductWeakPtr prod) = 0;
};

class weak_ptr_comp {
 public:
  bool operator() (const IProductWeakPtr& p1, const IProductWeakPtr& p2) const {
    auto lp1 = p1.lock(), lp2 = p2.lock();

    if (!lp2) return false;
    if (!lp1) return true;
    return lp1 < lp2;
  }
};

class ShopBase : public IShop, public std::enable_shared_from_this<ShopBase> {
 public:
  ShopBase() {}

  void AddProduct(IProductPtr prod) override;
  void RemoveProduct(IProductPtr prod) override;
  double Sell(IProductWeakPtr prod) override;

 private:
  std::mutex mutex_;
  std::set<IProductWeakPtr, weak_ptr_comp> products_;
};

using ShopBasePtr = std::shared_ptr<ShopBase>;
