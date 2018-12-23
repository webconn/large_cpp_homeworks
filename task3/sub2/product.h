#pragma once

#include <list>
#include <memory>
#include <mutex>
#include <atomic>

class IShop;
using IShopWeakPtr = std::weak_ptr<IShop>;

class IProduct {
 public:
  virtual ~IProduct() {}

  virtual void Attach(IShopWeakPtr shop) = 0;
  virtual void Detach(IShopWeakPtr shop) = 0;
  virtual void StartSales() = 0;
  virtual void StopSales() = 0;
  virtual void ChangePrice(double price) = 0;
  virtual double GetPrice() const = 0;
};

class ProductBase : public IProduct, public std::enable_shared_from_this<ProductBase> {
 public:
  ProductBase(double price);
  ~ProductBase();

  void Attach(IShopWeakPtr shop) override;
  void Detach(IShopWeakPtr shop) override;
  void StartSales() override;
  void StopSales() override;
  void ChangePrice(double price) override;
  double GetPrice() const override;

 private:
  std::list<IShopWeakPtr> shops_;
  double price_;
  mutable std::mutex mutex_;
  std::atomic_bool started_;
};

using ProductBasePtr = std::shared_ptr<ProductBase>;
