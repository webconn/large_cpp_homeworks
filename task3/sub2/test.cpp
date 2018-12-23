#include "lest.hpp"

#include "product.h"
#include "shop.h"

const lest::test specification[] = {
  CASE("can sell existing product") {
    ShopBasePtr shop(new ShopBase());
    ProductBasePtr prod1(new ProductBase(10.0));

    EXPECT(shop->Sell(prod1) < 0);

    prod1->StartSales();
    prod1->Attach(shop);

    EXPECT(shop->Sell(prod1) == 10.0);

    prod1->StopSales();

    EXPECT(shop->Sell(prod1) < 0);
  },

  CASE("product can't be sold if destructed") {
    ShopBasePtr shop(new ShopBase());

    ProductBasePtr prod1(new ProductBase(10.0));
    prod1->Attach(shop);
    prod1->StartSales();

    EXPECT(shop->Sell(prod1) == 10.0);

    prod1.reset();

    EXPECT(shop->Sell(prod1) < 0);
  },

  CASE("product can change price") {
    ShopBasePtr shop(new ShopBase());

    ProductBasePtr prod1(new ProductBase(10.0));

    prod1->Attach(shop);
    prod1->StartSales();

    EXPECT(shop->Sell(prod1) == 10.0);

    prod1->ChangePrice(20.0);

    EXPECT(shop->Sell(prod1) == 20.0);
  },

  CASE("shop can be destroyed and it's OK") {
    ShopBasePtr shop1(new ShopBase()), shop2(new ShopBase());

    ProductBasePtr prod(new ProductBase(10.0));

    prod->StartSales();
    prod->Attach(shop1);
    prod->Attach(shop2);

    EXPECT(shop1->Sell(prod) == 10.0);
    EXPECT(shop2->Sell(prod) == 10.0);

    shop1.reset();

    EXPECT(shop2->Sell(prod) == 10.0);
  }
};

int main(int argc, char **argv) {
  return lest::run(specification, argc, argv);
}
