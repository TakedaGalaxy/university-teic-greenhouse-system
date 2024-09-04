#include <iostream>
#include <cstdint>

struct MemoryProtocol
{
  uint8_t *data = nullptr;
  uint32_t size;
};

MemoryProtocol alocMemoryProtocol(uint32_t size)
{
  MemoryProtocol memory;

  memory.data = new uint8_t[size];
  memory.size = size;

  return memory;
}

template <typename T>
void writeMemoryProtocol(MemoryProtocol memory, uint32_t address, T value)
{ 
  memcpy(memory.data + address, &value, sizeof(T));
}

template <typename T>
T readMemoryProtocol(MemoryProtocol memory, uint32_t address)
{
  T value;

  memcpy(&value, memory.data + address, sizeof(T));

  return value;
}

int main()
{

  uint32_t teste = 69;

  auto memory = alocMemoryProtocol(50);

  writeMemoryProtocol(memory, 0, teste);

  auto readTeste = readMemoryProtocol<uint32_t>(memory, 0);

  std::cout << "V: " << teste << std::endl;
  std::cout << "R: " << readTeste << std::endl;

  return 0;
}
