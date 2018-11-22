#include <iostream>
#include <vector>
#include <memory>
#include <random>
#include <type_traits>

static const unsigned kNumIters = 10000;
static const double kValEps = 1e-10;  // to check double value == 0 in normal conditions

// -------------------------------------------------------------------------------------------------

class TRandomNumberGenerator {
 public:
  virtual ~TRandomNumberGenerator() {}
  virtual double Generate() = 0;
};

using TRandomNumberGeneratorPtr = std::unique_ptr<TRandomNumberGenerator>;

class TPoissonRandomNumberGenerator : public TRandomNumberGenerator {
 public:
  TPoissonRandomNumberGenerator(double lambda) : d(lambda) {

  }
  double Generate() override {
    return double(d(gen));
  }
 private:
  std::poisson_distribution<int> d;
  std::default_random_engine gen;
};

class TBernoulliRandomNumberGenerator : public TRandomNumberGenerator {
 public:
  TBernoulliRandomNumberGenerator(double p) : d(p) {}
  double Generate() override {
    if (d(gen)) {
      return 1.0;
    } else {
      return 0.0;
    }
  }
 private:
  std::bernoulli_distribution d;
  std::default_random_engine gen;
};

class TGeometricRandomNumberGenerator : public TRandomNumberGenerator {
 public:
  TGeometricRandomNumberGenerator(double p) : d(p) {}
  double Generate() override {
    return d(gen);
  }
 private:
  std::geometric_distribution<int> d;
  std::default_random_engine gen;
};

class TFiniteRandomNumberGenerator : public TRandomNumberGenerator {
 public:
  template<class ValueIterator, class ProbIterator>
  TFiniteRandomNumberGenerator(ValueIterator v_begin, ValueIterator v_end, ProbIterator p_begin, ProbIterator p_end)
    : d(p_begin, p_end), vals(v_begin, v_end) {}
  double Generate() override {
    return vals[d(gen)];
  }
 private:
  std::discrete_distribution<int> d;
  std::vector<double> vals;
  std::default_random_engine gen;
};

// -------------------------------------------------------------------------------------------------

template<class TConcreteRng, typename ...Targs>
TRandomNumberGeneratorPtr MakeConcrete(const Targs&...) {
  std::cerr << "Unknown constructor" << std::endl;
  return nullptr;
}

template<>
TRandomNumberGeneratorPtr MakeConcrete<TPoissonRandomNumberGenerator>(const double& lambda) {
  if (lambda <= 0) {
    return nullptr;
  }

  return std::make_unique<TPoissonRandomNumberGenerator>(lambda);
}

template<>
TRandomNumberGeneratorPtr MakeConcrete<TBernoulliRandomNumberGenerator>(const double& p) {
  if (p < 0 || p > 1.0) {
    return nullptr;
  }

  return std::make_unique<TBernoulliRandomNumberGenerator>(p);
}

template<>
TRandomNumberGeneratorPtr MakeConcrete<TGeometricRandomNumberGenerator>(const double& p) {
  if (p < 0 || p > 1.0) {
    return nullptr;
  }

  return std::make_unique<TGeometricRandomNumberGenerator>(p);
}



template<>
TRandomNumberGeneratorPtr MakeConcrete<TFiniteRandomNumberGenerator>
                          (const std::vector<double>& vs, const std::vector<double>& ps) {
  double psum = 0;
  for (auto p: ps) {
    psum += p;
  }

  if (std::abs(1.0 - psum) >= kValEps) {
    return nullptr;
  }

  if (vs.size() != ps.size()) {
    return nullptr;
  }

  return std::make_unique<TFiniteRandomNumberGenerator>(vs.cbegin(), vs.cend(), ps.cbegin(), ps.cend());
}

template<typename ...TArgs>
TRandomNumberGeneratorPtr MakeRandomNumberGenerator(const std::string& type, TArgs ...args) {
  if (type == "poisson") {
    return MakeConcrete<TPoissonRandomNumberGenerator>(args...);
  } else if (type == "bernoulli") {
    return MakeConcrete<TBernoulliRandomNumberGenerator>(args...);
  } else if (type == "geometric") {
    return MakeConcrete<TGeometricRandomNumberGenerator>(args...);
  } else if (type == "finite") {
    return MakeConcrete<TFiniteRandomNumberGenerator>(args...);
  } else {
    std::cout << "unknown type: " << type << std::endl;
    return nullptr;
  }
}

// -------------------------------------------------------------------------------------------------

bool CheckPoisson(double l, double max_difference = 1e-1, unsigned num_iters = kNumIters) {
  TRandomNumberGeneratorPtr p = MakeRandomNumberGenerator("poisson", l);
  if (!p) {
    std::cerr << "Error creating Poisson generator with l=" << l << std::endl;
    return false;
  }

  // get poisson mean value from STL
  std::poisson_distribution<int> distr(l);
  double mean = distr.mean();
  double exp_mean = 0;

  for (unsigned i = 0; i < num_iters; i++) {
    exp_mean += p->Generate();
  }

  exp_mean /= num_iters;

  std::cout << "Poisson(l=" << l << ") analytic mean: " << mean << ", experimental " << exp_mean << std::endl;

  return std::abs(mean - exp_mean) < max_difference;
}

bool CheckBernoulli(double pp, double max_difference = 1e-1, unsigned num_iters = kNumIters) {
  TRandomNumberGeneratorPtr p = MakeRandomNumberGenerator("bernoulli", pp);
  if (!p) {
    std::cerr << "Error creating Bernoulli generator with p=" << pp << std::endl;
    return false;
  }

  unsigned num_t = 0, num_f = 0;

  for (unsigned i = 0; i < num_iters; i++) {
    double val = p->Generate();
    if (val > 0.5) {
      num_t++;
    } else {
      num_f++;
    }
  }

  double result = double(num_t) / num_iters;

  std::cout << "Bernoulli(p=" << pp << ") experimental " << result << std::endl;

  return std::abs(pp - result) < max_difference;
}

bool CheckGeometric(double pp, double max_difference = 1e-1, unsigned num_iters = kNumIters) {
  TRandomNumberGeneratorPtr p = MakeRandomNumberGenerator("geometric", pp);
  if (!p) {
    std::cerr << "Error creating Geometric generator with p=" << pp << std::endl;
    return false;
  }

  double mean = -1.0;
  if (pp > kValEps) { // pp != 0
    mean = (1 - pp) / pp;
  }

  double exp = 0;

  for (unsigned i = 0; i < num_iters; i++) {
    exp += p->Generate();
  }

  exp /= num_iters;

  if (mean < 0) {
    std::cout << "Geometric (p=1.0): value is expected to be large (TODO: check it): "<< exp;
    return true;
  } else {
    std::cout << "Geometric(p=" << pp << ") mean " << mean << ", experimental " << exp << std::endl;
    return std::abs(exp - mean) < max_difference;
  }
}

bool CheckFinite(const std::vector<double>& vals, const std::vector<double>& probs,
                 double max_difference = 1e-1, unsigned num_iters=kNumIters) {
  TRandomNumberGeneratorPtr p = MakeRandomNumberGenerator("finite", vals, probs);
  if (!p) {
    std::cerr << "Error creating finite generator" << std::endl;
    return false;
  }

  double mean = 0;
  for (unsigned i = 0; i < vals.size(); i++) {
    mean += vals[i] * probs[i];
  }

  double exp = 0;
  for (unsigned i = 0; i < num_iters; i++) {
    exp += p->Generate();
  }

  exp /= num_iters;

  std::cout << "Finite(...) mean " << mean << ", experimental " << exp << std::endl;
  return std::abs(exp - mean) < max_difference;
}

// -------------------------------------------------------------------------------------------------

int main(int argc, char** argv) {
  (void) argc;
  (void) argv;

  CheckPoisson(0.5);
  CheckBernoulli(0.5);
  CheckGeometric(0.5);
  CheckGeometric(0.3);
  CheckFinite({1.0, 2.0, 3.0, 4.0}, {0.4, 0.3, 0.2, 0.1});

  return 0;
}
