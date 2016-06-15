#include <libft.h>

#include "corewar_priv.h"
#include "hooks.h"

#define INCR_PC_LEN(x) {args.len += x; proc->pc += x;}

static t_args	*gather_args(t_proc *proc, const t_op *op)
{
	uint8_t			coding_byte;
	static t_args	args;

	ft_bzero(&args, sizeof(t_args));
	INCR_PC_LEN(1);
	coding_byte = deref(proc, 0);
	INCR_PC_LEN(1);

	int i = 0;
	while (i < op->nbr_params)
	{
		uint8_t code = (coding_byte >> 6) & 0xFF;

		args.fields[i].code = code;
		if (code == DIR_CODE) {
			INCR_PC_LEN(4);
			if (op->params[i] & T_DIR)
				args.fields[i].param = deref_word(proc, -4);
			else
				return (&args);
		}
		else if (code == IND_CODE) {
			INCR_PC_LEN(2);
			if (op->params[i] & T_IND)
				args.fields[i].param = deref_short(proc, -2);
			else
				return (&args);
		}
		else if (code == REG_CODE) {
			INCR_PC_LEN(1);
			if (op->params[i] & T_REG)
				args.fields[i].param = deref(proc, -1);
			else
				return (&args);
		}

		i += 1;
		args.nbr += 1;
		coding_byte = coding_byte << 2;
	}

	// DBG("%d\n", args.len);
	return (&args);
}

static void	nop_exec(t_proc *proc, const t_op *op)
{
	(void)proc;
	(void)op;
	proc->pc += 1;
}

static void	live_exec(t_proc *proc, const t_op *op)
{
	t_word	champion;
	t_vm	*vm;

	(void)op;
	vm = proc->vm;
	champion = deref_word(proc, 1);
	if (champion < vm->nb_champions)
		vm->winner = champion;
	proc->live = true;
	vm->nbr_live++;

	t_args args;
	args = (t_args){ .len = 5, .fields = { { .param = champion } } };
	exec_op_hook(&args, proc, "live", args.len);
}

static void	ld_exec(t_proc *proc, const t_op *op) // no reg0 protection
{
	t_args *args = gather_args(proc, op);

	if (args->nbr != op->nbr_params)
		return exec_op_hook(args, proc, "ld", args->len);

	if (args->fields[0].code == IND_CODE) {
		register_set(proc, args->fields[1].param, deref_word(proc, args->fields[0].param - args->len));
		args->fields[0].param = register_get(proc, args->fields[1].param);
	}
	else {
		register_set(proc, args->fields[1].param, args->fields[0].param);
	}

	if (register_get(proc, args->fields[1].param) == 0)
		proc->carry = true;
	else
		proc->carry = false;

	return exec_op_hook(args, proc, "ld", args->len);
};

static void st_exec(t_proc *proc, const t_op *op) // no reg0 protection
{
	t_args *args = gather_args(proc, op);

	if (args->nbr != op->nbr_params)
		return (exec_op_hook(args, proc, "st", args->len));

	if (args->fields[1].code == IND_CODE) {
		assignate_word(register_get(proc, args->fields[0].param), proc, args->fields[1].param % IDX_MOD - args->len);
	}
	else if (args->fields[1].code == REG_CODE) {
		register_set(proc, args->fields[1].param, register_get(proc, args->fields[0].param));
	}

	return (exec_op_hook(args, proc, "st", args->len));
}

static void add_exec(t_proc *proc, const t_op *op)
{
	t_args *args = gather_args(proc, op);

	(void)op;
	if (args->nbr != op->nbr_params)
		return exec_op_hook(args, proc, "add", args->len) ;

	register_set(proc, args->fields[2].param, register_get(proc, args->fields[0].param) + register_get(proc, args->fields[1].param));

	if (register_get(proc, args->fields[2].param) == 0)
		proc->carry = true;
	else
		proc->carry = false;

	return exec_op_hook(args, proc, "add", args->len) ;
}

static void sub_exec(t_proc *proc, const t_op *op)
{
	t_args *args = gather_args(proc, op);

	(void)op;
	if (args->nbr != op->nbr_params)
		return exec_op_hook(args, proc, "sub", args->len) ;

	register_set(proc, args->fields[2].param, register_get(proc, args->fields[0].param) - register_get(proc, args->fields[1].param));

	if (register_get(proc, args->fields[2].param) == 0)
		proc->carry = true;
	else
		proc->carry = false;

	return exec_op_hook(args, proc, "sub", args->len) ;
}

static void	zjmp_exec(t_proc *proc, const t_op *op)
{
	t_args args;

	(void)op;
	if (proc->carry){
		args = (t_args){ .len = deref_short(proc, 1), .fields = { { .param = deref_short(proc, 1) } } };
		exec_op_hook(&args, proc, "zjmp", args.len);
		proc->pc += deref_short(proc, 1);
	}
	else {
		args = (t_args){ .len = deref_short(proc, 1), .fields = { { .param = deref_short(proc, 1) } } };
		exec_op_hook(&args, proc, "zjmp", args.len);
		proc->pc += 3;
	}
}

static void	aff_exec(t_proc *proc, const t_op *op)
{
	uint8_t	coding_byte;
	uint8_t	reg_num;

	(void)op;
	coding_byte = deref(proc, 1);
	if (coding_byte != REG_CODE << 6)
		proc->pc += 2;
	reg_num = deref(proc, 2);
	aff_hook(proc, register_get(proc, reg_num));
	proc->pc += 3;
}

const t_myop	myop_tab[] =
{
	{ &((t_op){"nop", 1, {0}, 0, 1, "nop", 0, 0}),  nop_exec},
	{&op_tab[0],  live_exec},
	{&op_tab[1],  ld_exec},
	{&op_tab[2],  st_exec},
	{&op_tab[3],  add_exec},
	{&op_tab[4],  sub_exec},
	{&op_tab[5],  NULL},
	{&op_tab[6],  NULL},
	{&op_tab[7],  NULL},
	{&op_tab[8],  zjmp_exec},
	{&op_tab[9],  NULL},
	{&op_tab[10], NULL},
	{&op_tab[11], NULL},
	{&op_tab[12], NULL},
	{&op_tab[13], NULL},
	{&op_tab[14], NULL},
	{&op_tab[15], aff_exec},
};
