#include "cpu.h"
#include "assembler.h"

using namespace intel_8085;

int main(int, char**) 
{
    Memory      mem;
    CPU         cpu(mem);
    Assembler   assembler(mem);

    cpu.reset();

    assembler.assemble("../example_bubble_sort.asm");
}
