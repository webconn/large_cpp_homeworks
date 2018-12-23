#include "product.h"
#include "shop.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace std::chrono_literals;

class Shop1 : public ShopBase {};
class Shop2 : public ShopBase {};

class Prod1 : public ProductBase {
 public:
  Prod1(double price) : ProductBase(price) {}
};

class Prod2 : public ProductBase {
 public:
  Prod2(double price) : ProductBase(price) {}
};

int main(int argc, char** argv) {
  ShopBasePtr shop1(new Shop1()), shop2(new Shop2());
  ProductBasePtr prod2(new Prod2(15.0));
  ProductBasePtr prod1(new Prod1(10.0));

  thread t1([&]() {
    prod1->Attach(shop1);
    prod1->Attach(shop2);
    cout << "Prod1 is about to start selling!" << endl;
    prod1->StartSales();

    prod2->Attach(shop2);
    prod2->StartSales();
    cout << "Prod2 is about to start selling!" << endl;

    prod1->Detach(shop1);
    cout << "Prod1 is now discontinued in shop2" << endl;

    cout << "Prod2 price change!" << endl;
    prod2->ChangePrice(5.99);

    cout << "Prod1 is about to return to shop1" << endl;
    prod1->Attach(shop1);

    this_thread::sleep_for(100ms);
  });

  cout << "Try to buy Prod1 in shop1" << endl;
  double p = shop1->Sell(prod1);
  if (p > 0) {
    cout << "Price: " << p << endl;
  } else {
    cout << "Not available :(" << endl;
  }

  cout << "Close shop1" << endl;
  shop1.reset();

  cout << "Buy prod2 from shop2" << endl;
  p = shop2->Sell(prod2);

  if (p > 0) {
    cout << "Price: " << p << endl;
  } else {
    cout << "Not available :(" << endl;
  }

  t1.join();

  return 0;
}
