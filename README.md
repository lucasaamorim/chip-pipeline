# Implementação de Pipeline
Implementação em SystemC de um modelo de organização (modelo PC/PO) para uma ISA RISC.

## Instruções Suportadas
O modelo é capaz de realizar as seguintes instruções em pipeline:
**Tipo R**
- AND: Operação booleana and
- OR: Operação booleana or
- XOR: Operação booleana xor
- CMP: Comparação
- ADD: Soma
- SUB: Subtração
**Tipo I**
- NOT: Operação booleana not
- LD: Leitura em Memória
- ST: Armazenamento em Memória
**Tipo J**
- J: Salto incondicional
- JN: Salto condicional (caso negativo)
- JZ: Salto condicional (caso zero)

## Modos de Endereçamento
Os modos de endereçamento possíveis para cada tipo de instrução são:
1. **Tipo R**: Registrador 
2. **Tipo I**: Base, Imediato e Registrador
3. **Tipo J**: Absoluto

## Formato da palavra de instrução
A palavra de instrução são um capo de tamanho fixo. São 32 bits e pode ter os seguintes formatos:
### Tipo R
1. 6 bits de opcode
2. 12 bits para registradores ()
3. 14 bits livres
### Tipo I
1. 6 bits de opcode
2. 8 bits para registradores
3. 14 bits livres
### Tipo J
1. 6 bits de opcode
2. 26 bits de endereço (absoluto)

## Pipeline

### Estágios do Pipeline
O pipeline implementado contém 5 estágios:
1. IF - Instruction Fetch: Busca por instruções na memória de instrução através do PC (Contador de Programa)
2. ID - Instruction Decode: Decodificação e Obtenção dos operandos
3. EX - Execution: Executa operações na ULA
4. MEM - Memory Access: Acessa a memória para leitura ou escrita
5. WB - Write Back: 

### Tratamento de hazards (conflitos) de pipeline
W.I.P...

