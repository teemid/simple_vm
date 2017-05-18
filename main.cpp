#include <cstdint>
#include <vector>

#define KB(b) (1024 * (b))
#define MB(b) (1024 * KB(b))
#define GB(b) (1024 * MB(b))

enum class OpCode : uint32_t
{
    Halt,
    Load,
    Store,
    Inc,
    Dec,
    Add,
    Sub,
    Mul,
    Div,
    And,
    Jump,
    JumpIfZero,
};

enum class Register : uint8_t
{
    PC,
    A,
    B,
    C,
    D,
    E,
    F,
    G,
};

struct Instruction
{
    OpCode op;
    Register operandA;
    union
    {
        struct
        {
            Register operandB;
            Register operandC;
        };
        uint16_t uvalue;
        int16_t value;
    };

    Instruction(OpCode op) : op(op) { }

    Instruction(OpCode op, Register operandA) : op(op), operandA(operandA) { }

    Instruction(OpCode op, Register operandA, uint16_t value)
    {
        this->op = op;
        this->operandA = operandA;
        this->value = value;
    }

    Instruction(OpCode op, Register operandA, Register operandB, Register operandC)
    {
        this->op = op;
        this->operandA = operandA;
        this->operandB = operandB;
        this->operandC = operandC;
    }

    Instruction(const Instruction & other) = default;
    ~Instruction() = default;
};

struct VirtualMachine
{
    std::vector<Instruction> instructions;
    std::vector<int32_t> stack;

    union
    {
        // NOTE: These have to be in the same order as enum class Register.
        struct
        {
            uint32_t pc; // program counter
            int32_t a; // accumulator
            int32_t b;
            int32_t c;
            int32_t d;
            int32_t e;
            int32_t f;
            int32_t g;
        };
        int32_t registers[8];
    };

    VirtualMachine() : pc(0), a(0), b(0), c(0), d(0), e(0), f(0), g(0) { }
    ~VirtualMachine() = default;
};

Instruction Fetch(VirtualMachine * vm)
{
    return vm->instructions[vm->pc++];
}

void Load(VirtualMachine * vm, Register operandA, int16_t value)
{
    vm->registers[(uint8_t)operandA] = value;
}

void Inc(VirtualMachine * vm, Register operandA)
{
    vm->registers[(uint8_t)operandA]++;
}

void Dec(VirtualMachine * vm, Register operandA)
{
    vm->registers[(uint8_t)operandA]--;
}

void Add(VirtualMachine * vm, Register operandA, Register operandB, Register operandC)
{
    vm->registers[(uint8_t)operandC] = vm->registers[(uint8_t)operandA] + vm->registers[(uint8_t)operandB];
}

void Sub(VirtualMachine * vm, Register operandA, Register operandB, Register operandC)
{
    vm->registers[(uint8_t)operandC] = vm->registers[(uint8_t)operandA] - vm->registers[(uint8_t)operandB];
}

void Mul(VirtualMachine * vm, Register operandA, Register operandB, Register operandC)
{
    vm->registers[(uint8_t)operandC] = vm->registers[(uint8_t)operandA] * vm->registers[(uint8_t)operandB];
}

void Div(VirtualMachine * vm, Register operandA, Register operandB, Register operandC)
{
    vm->registers[(uint8_t)operandC] = vm->registers[(uint8_t)operandA] / vm->registers[(uint8_t)operandB];
}

void And(VirtualMachine * vm, Register operandA, Register operandB, Register operandC)
{
    vm->registers[(uint8_t)operandC] = vm->registers[(uint8_t)operandA] && vm->registers[(uint8_t)operandB];
}

void Jump(VirtualMachine * vm, uint16_t uvalue)
{
    vm->pc = uvalue;
}

void JumpIfZero(VirtualMachine * vm, Register operandA, uint16_t uvalue)
{
    if (!vm->registers[(uint8_t)operandA])
    {
        vm->pc = uvalue;
    }
}

int main(int argc, char * argv)
{
    VirtualMachine vm;
    vm.instructions =
    {
        { OpCode::Load,       Register::A, 0                        },
        { OpCode::Load,       Register::B, 0                        },
        { OpCode::Inc,        Register::A                           },
        { OpCode::Load,       Register::D, 10                       },
        { OpCode::Sub,        Register::A, Register::D, Register::C },
        { OpCode::JumpIfZero, Register::C, 2                        },
        { OpCode::Halt                                              },
    };

    for ( ; ; )
    {
        Instruction instruction = Fetch(&vm);

        switch (instruction.op)
        {
            case OpCode::Load:
            {
                Load(&vm, instruction.operandA, instruction.value);
            } break;
            case OpCode::Inc:
            {
                Inc(&vm, instruction.operandA);
            } break;
            case OpCode::Dec:
            {
                Dec(&vm, instruction.operandA);
            } break;
            case OpCode::Add:
            {
                Add(&vm, instruction.operandA, instruction.operandB, instruction.operandC);
            } break;
            case OpCode::Sub:
            {
                Sub(&vm, instruction.operandA, instruction.operandB, instruction.operandC);
            } break;
            case OpCode::Mul:
            {
                Mul(&vm, instruction.operandA, instruction.operandB, instruction.operandC);
            } break;
            case OpCode::Div:
            {
                Div(&vm, instruction.operandA, instruction.operandB, instruction.operandC);
            } break;
            case OpCode::Jump:
            {
                Jump(&vm, instruction.uvalue);
            } break;
            case OpCode::JumpIfZero:
            {
                JumpIfZero(&vm, instruction.operandA, instruction.uvalue);
            } break;
            case OpCode::Halt:
            {
                return 0;
            } break;
        }
    }

    return 0;
}

