#include <sentinel/arch/x86_64/cpu.hpp>
#include <sentinel/types.hpp>

namespace sentinel::arch::x86_64::cpu
{
    sentinel::u64 read_rflags()
    {
        sentinel::u64 flags;

        __asm__ volatile("pushfq\n\t" "popq %0" : "=r"(flags) : : "memory"
    );

        return flags;
    }

    void disable_interrupts()
    {
        __asm__ volatile("cli" ::: "memory");
    }

    void enable_interrupts()
    {
        __asm__ volatile("sti" ::: "memory");
    }

    void restore_rflags(sentinel::u64 flags)
    {
        __asm__ volatile (
            "pushq %0\n\t"
            "popfq"
            :
            : "r"(flags)
            : "memory", "cc"
        );
    }

    bool interrupts_enabled()
    {
        sentinel::u64 flags=read_rflags();

        return (flags & (1ULL << 9)) != 0;
    }
}