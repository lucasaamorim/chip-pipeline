#include "RegisterBank.hpp"

template <unsigned int DATA_BITS, unsigned int ADDRESS_BITS>
void RegisterBank<DATA_BITS, ADDRESS_BITS>::process() {
  /// Percorre todos os registradores atualizando os seus sinais
  if (write.read()) {
    for (int i = 0; i < REGISTERS; i++) {
      if (i == write_address.read()) {
        register_writes[i].write(true);
        register_inputs[i].write(input.read());
      } else {
        register_writes[i].write(false);
      }
    }
  }

  /// Leitura dos registradores
  output_1.write(register_outputs[read_address_1.read()]);
  output_2.write(register_outputs[read_address_2.read()]);
}

// Instanciando os registradores
template class RegisterBank<8, 5>;
template class RegisterBank<16, 5>;
template class RegisterBank<32, 5>;
template class RegisterBank<64, 5>;