#include "Register.hpp"

template <unsigned int BITS> void Register<BITS>::process() {
  /// Grava e depois lê
  if (reset.read()) {
    data = 0;
  } else if (write.read()) {
    data = input.read();
  }

  output.write(data);
}

// Instanciando os templates
template class Register<8>;
template class Register<16>;
template class Register<32>;
template class Register<64>;