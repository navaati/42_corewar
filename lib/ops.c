#include <libft.h>

#include "corewar_priv.h"
#include "hooks.h"

static t_args	*gather_args(t_proc *proc, uint8_t max)
{
	uint8_t			coding_byte;
	static t_args	args;

	ft_bzero(&args, sizeof(t_args));
	// ft_bzero(args.fields, sizeof(t_field) * 4);
	args.len = 1;
	coding_byte = deref(proc, args.len);

	args.len += 1;

	int i = 0;
	while (i < max)
	{
		uint8_t code = (coding_byte >> 6) & 0xFF;

		args.fields[i].code = code;
		if (code == DIR_CODE) {
			args.fields[i].param = deref_word(proc, args.len);
			args.len += 4;
		}
		else if (code == IND_CODE) {
			args.fields[i].param = deref_short(proc, args.len);
			args.len += 2;
		}
		else if (code == REG_CODE) {
			args.fields[i].param = deref(proc, args.len);
			args.len += 1;
		}

		i += 1;
		args.nbr += 1;
		coding_byte = coding_byte << 2;
	}
	return (&args);
}

static t_offset	nop_exec(t_proc *proc)
{
	(void)proc;
	// t_word params[5] = { 0 };
	// exec_op_hook(params, proc, "nop", 1);
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

	t_args args;
	args = (t_args){ .len = 5, .fields = { { .param = champion } } };
	exec_op_hook(&args, proc, "live", args.len);

	return (5);
}


static t_offset	ld_exec(t_proc *proc) // no reg0 protection
{
	t_args *args = gather_args(proc, 2);

	if (args->fields[0].code != DIR_CODE && args->fields[0].code != IND_CODE )
		return args->len;
	if (args->fields[1].code != REG_CODE)
		return args->len;

	if (args->fields[0].code == IND_CODE) {
		proc->regs[args->fields[1].param - 1] = deref_word(proc, args->fields[0].param);
		args->fields[0].param = proc->regs[args->fields[1].param - 1];
	}
	else
		proc->regs[args->fields[1].param - 1] = args->fields[0].param;

	if (proc->regs[args->fields[1].param - 1] == 0)
		proc->carry = true;
	else
		proc->carry = false;

	exec_op_hook(args, proc, "ld", args->len);

	return (args->len);
};

static t_offset st_exec(t_proc *proc) // no reg0 protection
{
	t_args *args = gather_args(proc, 2);

	if (args->fields[0].code != REG_CODE)
		return args->len;
	if (args->fields[1].code != IND_CODE && args->fields[1].code != REG_CODE )
		return args->len;

	if (args->fields[1].code == IND_CODE)
		assignate_word(proc->regs[args->fields[0].param - 1], proc, args->fields[1].param);
	else if (args->fields[1].code == REG_CODE) {
		if (args->fields[1].param <= 0)
			return args->len;
		if (args->fields[0].param <= 0)
			return args->len;
		proc->regs[args->fields[1].param - 1] = proc->regs[args->fields[0].param - 1];
	}

	exec_op_hook(args, proc, "st", args->len);

	return (args->len);
}

static t_offset add_exec(t_proc *proc)
{
	t_args *args = gather_args(proc, 3);

	if (args->fields[0].code != REG_CODE)
		return (args->len);
	if (args->fields[1].code != REG_CODE)
		return (args->len);
	if (args->fields[2].code != REG_CODE)
		return (args->len);

	if (args->fields[0].param < 1 || args->fields[0].param > REG_NUMBER)
		return (args->len);
	if (args->fields[1].param < 1 || args->fields[1].param > REG_NUMBER)
		return (args->len);
	if (args->fields[2].param < 1 || args->fields[2].param > REG_NUMBER)
		return (args->len);

	proc->regs[args->fields[2].param - 1] = proc->regs[args->fields[0].param - 1] + proc->regs[args->fields[1].param - 1];

	if (proc->regs[args->fields[2].param - 1] == 0)
		proc->carry = true;
	else
		proc->carry = false;

	exec_op_hook(args, proc, "add", args->len);

	return (args->len);

}

static t_offset sub_exec(t_proc *proc)
{
	t_args *args = gather_args(proc, 3);

	if (args->fields[0].code != REG_CODE)
		return (args->len);
	if (args->fields[1].code != REG_CODE)
		return (args->len);
	if (args->fields[2].code != REG_CODE)
		return (args->len);

	if (args->fields[0].param < 1 || args->fields[0].param > REG_NUMBER)
		return (args->len);
	if (args->fields[1].param < 1 || args->fields[1].param > REG_NUMBER)
		return (args->len);
	if (args->fields[2].param < 1 || args->fields[2].param > REG_NUMBER)
		return (args->len);

	proc->regs[args->fields[2].param - 1] = proc->regs[args->fields[0].param - 1] - proc->regs[args->fields[1].param - 1];

	if (proc->regs[args->fields[2].param - 1] == 0)
		proc->carry = true;
	else
		proc->carry = false;

	exec_op_hook(args, proc, "sub", args->len);

	return (args->len);

}

static t_offset	zjmp_exec(t_proc *proc)
{
	t_args args;

	if (proc->carry){
		args = (t_args){ .len = deref_short(proc, 1), .fields = { { .param = deref_short(proc, 1) } } };
		exec_op_hook(&args, proc, "zjmp", args.len);
		return (deref_short(proc, 1));
	}
	else {
		args = (t_args){ .len = deref_short(proc, 1), .fields = { { .param = deref_short(proc, 1) } } };
		exec_op_hook(&args, proc, "zjmp", args.len);
		return (3);
	}
}

static t_offset	aff_exec(t_proc *proc)
{
	uint8_t	coding_byte;
	uint8_t	reg_num;

	coding_byte = deref(proc, 1);
	if (coding_byte != REG_CODE << 6)
		return (2);
	reg_num = deref(proc, 2);
	aff_hook(proc, proc->regs[reg_num]);
	return (3);
}

const t_op	op_tab[] =
{
	{"nop",	1, nop_exec},
	{"live", 10, live_exec},
	{"ld", 5, ld_exec},
	{"st", 5, st_exec},
	{"add", 10, add_exec},
	{"sub", 10, sub_exec},
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
