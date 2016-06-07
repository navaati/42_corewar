#include <libft.h>

#include "corewar_priv.h"
#include "hooks.h"

static t_offset	nop_exec(t_proc *proc)
{
	(void)proc;
	return (1);
}

static t_offset	live_exec(t_proc *proc)
{
	t_word	champion;
	t_vm	*vm;

	vm = proc->vm;
	champion = deref_word(proc, 1);
	if (champion < vm->nb_champions)
		vm->winner = champion;
	proc->live = true;
	vm->nbr_live++;
	return (5);
}

static t_offset	ld_exec(t_proc *proc)
{
	uint8_t		coding_byte;
	uint8_t		coding_first;
	t_word		word;
	t_offset	length;
	uint8_t		reg_num;

	length = 1;
	coding_byte = deref(proc, length);
	length++;
	coding_first = (coding_byte >> 6) & 0xFF;
	if (coding_first == DIR_CODE)
	{
		word = deref_word(proc, length);
		length += 4;
	}
	else if (coding_first == IND_CODE)
	{
		word = deref_ind(proc, length);
		length += 2;
	}
	else
		return(0);
	reg_num = deref(proc, length);
	length++;
	proc->regs[reg_num] = word;

	if (proc->regs[reg_num] == 0)
		proc->carry = true;
	else
		proc->carry = false;
	return (length);
};

static t_offset	zjmp_exec(t_proc *proc)
{
	if (proc->carry)
		return (deref_short(proc, 1));
	else
		return (3);
}

static t_offset	aff_exec(t_proc *proc)
{
	uint8_t	coding_byte;
	uint8_t	reg_num;

	coding_byte = deref(proc, 1);
	if (coding_byte != REG_CODE << 6)
		return (0);
	reg_num = deref(proc, 2);
	aff_hook(proc, proc->regs[reg_num]);
	return (3);
}

const t_op	op_tab[] =
{
	{"nop",	0, nop_exec},
	{"live", 10, live_exec},
	{"ld", 5, ld_exec},
	{"st", 5, NULL},
	{"add", 10, NULL},
	{"sub", 10, NULL},
	{"and", 6, NULL},
	{"or", 6, NULL},
	{"xor", 6, NULL},
	{"zjmp", 20, zjmp_exec},
	{"ldi", 25, NULL},
	{"sti", 25, NULL},
	{"fork", 800, NULL},
	{"lld", 10, NULL},
	{"lldi", 50, NULL},
	{"lfork", 1000, NULL},
	{"aff", 2, aff_exec},
};
