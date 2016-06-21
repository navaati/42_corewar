#include <libft.h>

#include "corewar_priv.h"
#include "corewar.h"
#include "hooks.h"

static gather_f gather_table[] = {
	NULL,
	gather_reg,
	gather_dir,
	gather_ind
};

static uint8_t	gather_match(t_proc *proc, const t_op *op, t_args *args, uint8_t index)
{
	uint8_t		code;

	code = args->fields[index].code;
	if (gather_table[code]) {
		if (!gather_table[code](proc, op, args, index))
			return (0);
	}
	return (1);
}

static t_args	*gather_args(t_proc *proc, const t_op *op)
{
	uint8_t			coding_byte;
	static t_args	args;

	ft_bzero(&args, sizeof(t_args));
	args.len += 1;
	coding_byte = deref(proc, 1);
	args.len += 1;
	pc_incr(proc, args.len);

	int i = 0;
	while (i < op->nbr_params)
	{
		uint8_t code = (coding_byte >> 6) & 0xFF;

		args.fields[i].code = code;
		if (!gather_match(proc, op, &args, i))
			return (&args);

		i += 1;
		coding_byte = coding_byte << 2;
	}

	// DBG("%d\n", args.len);
	return (&args);
}

void	nop_exec(t_proc *proc, const t_op *op)
{
	(void)proc;
	(void)op;
	pc_incr(proc, 1);
}

void	live_exec(t_proc *proc, const t_op *op)
{
	t_word	champion;
	t_vm	*vm;

	(void)op;
	vm = proc->vm;
	champion = deref_word(proc, 1);

	proc->live = true;
	pc_incr(proc, 5);

	t_args args;
	args = (t_args){ .nbr = 1, .len = 5, .fields = { { .param = champion } } };
	exec_op_hook(&args, proc, "live", args.len);

	champion = UINT_MAX + 1 - champion;
	if (champion <= vm->nb_champions && champion != 0) {

		if (g_flags.v & 0x01)
			DBG("Player %d (%s) is said to be alive\n", (int)champion, vm->champions[champion - 1].name);

		vm->winner = champion;
		vm->nbr_live++;
	}

	show_pc_movement(&args, proc, "live", args.len);
}

void	ld_exec(t_proc *proc, const t_op *op) // no reg0 protection
{
	t_args *args = gather_args(proc, op);

	if (args->nbr != op->nbr_params || args->fields[1].param == 0)
		return (show_pc_movement(args, proc, "ld", args->len));

	if (args->fields[0].code == IND_CODE) {
		register_set(proc, args->fields[1].param, deref_word(proc, args->fields[0].param % IDX_MOD - args->len));
		args->fields[0].param = register_get(proc, args->fields[1].param);
	}
	else {
		register_set(proc, args->fields[1].param, args->fields[0].param);
	}

	if (register_get(proc, args->fields[1].param) == 0)
		proc->carry = true;
	else
		proc->carry = false;

	exec_op_hook(args, proc, "ld", args->len);
	show_pc_movement(args, proc, "ld", args->len);
};

void st_exec(t_proc *proc, const t_op *op) // no reg0 protection
{
	t_args *args = gather_args(proc, op);

	if (args->nbr != op->nbr_params)
		return (show_pc_movement(args, proc, "st", args->len));

	if (args->fields[1].code == IND_CODE) {
		assignate_word(register_get(proc, args->fields[0].param), proc, args->fields[1].param % IDX_MOD - args->len);
	}
	else if (args->fields[1].code == REG_CODE) {
		register_set(proc, args->fields[1].param, register_get(proc, args->fields[0].param));
	}

	exec_op_hook(args, proc, "st", args->len);
	show_pc_movement(args, proc, "st", args->len);
}

void add_exec(t_proc *proc, const t_op *op)
{
	t_args *args = gather_args(proc, op);

	(void)op;
	if (args->nbr != op->nbr_params)
		return (show_pc_movement(args, proc, "add", args->len));

	register_set(proc, args->fields[2].param, register_get(proc, args->fields[0].param) + register_get(proc, args->fields[1].param));

	if (register_get(proc, args->fields[2].param) == 0)
		proc->carry = true;
	else
		proc->carry = false;

	exec_op_hook(args, proc, "add", args->len);
	show_pc_movement(args, proc, "add", args->len);
}

void sub_exec(t_proc *proc, const t_op *op)
{
	t_args *args = gather_args(proc, op);

	(void)op;
	if (args->nbr != op->nbr_params)
		return (show_pc_movement(args, proc, "sub", args->len));

	register_set(proc, args->fields[2].param, register_get(proc, args->fields[0].param) - register_get(proc, args->fields[1].param));

	if (register_get(proc, args->fields[2].param) == 0)
		proc->carry = true;
	else
		proc->carry = false;

	exec_op_hook(args, proc, "sub", args->len);
	show_pc_movement(args, proc, "sub", args->len);
}

void and_exec(t_proc *proc, const t_op *op)
{
	t_args *args = gather_args(proc, op);

	(void)op;
	if (args->nbr != op->nbr_params)
		return (show_pc_movement(args, proc, "and", args->len)) ;

	if (args->fields[0].code == REG_CODE) {
		args->fields[0].param = register_get(proc, args->fields[0].param);
	}
	else if (args->fields[0].code == IND_CODE) {
		args->fields[0].param = deref_word(proc, args->fields[0].param - args->len);
	}

	if (args->fields[1].code == REG_CODE) {
		args->fields[1].param = register_get(proc, args->fields[1].param);
	}
	else if (args->fields[1].code == IND_CODE) {
		args->fields[1].param = deref_word(proc, args->fields[1].param - args->len);
	}

	if (args->fields[2].param > 0) {
		register_set(proc, args->fields[2].param, args->fields[0].param & args->fields[1].param );
		exec_op_hook(args, proc, "and", args->len);
	}

	show_pc_movement(args, proc, "and", args->len);
}

void or_exec(t_proc *proc, const t_op *op)
{
	t_args *args = gather_args(proc, op);

	(void)op;
	if (args->nbr != op->nbr_params)
		return (show_pc_movement(args, proc, "or", args->len));

	if (args->fields[0].code == REG_CODE) {
		args->fields[0].param = register_get(proc, args->fields[0].param);
	}
	else if (args->fields[0].code == IND_CODE) {
		args->fields[0].param = deref_word(proc, args->fields[0].param - args->len);
	}

	if (args->fields[1].code == REG_CODE) {
		args->fields[1].param = register_get(proc, args->fields[1].param);
	}
	else if (args->fields[1].code == IND_CODE) {
		args->fields[1].param = deref_word(proc, args->fields[1].param - args->len);
	}

	if (args->fields[2].param > 0) {
		register_set(proc, args->fields[2].param, args->fields[0].param | args->fields[1].param );
		exec_op_hook(args, proc, "or", args->len);
	}

	show_pc_movement(args, proc, "or", args->len);
}

void xor_exec(t_proc *proc, const t_op *op)
{
	t_args *args = gather_args(proc, op);

	(void)op;
	if (args->nbr != op->nbr_params)
		return (show_pc_movement(args, proc, "xor", args->len));

	if (args->fields[0].code == REG_CODE) {
		args->fields[0].param = register_get(proc, args->fields[0].param);
	}
	else if (args->fields[0].code == IND_CODE) {
		args->fields[0].param = deref_word(proc, args->fields[0].param - args->len);
	}

	if (args->fields[1].code == REG_CODE) {
		args->fields[1].param = register_get(proc, args->fields[1].param);
	}
	else if (args->fields[1].code == IND_CODE) {
		args->fields[1].param = deref_word(proc, args->fields[1].param - args->len);
	}

	if (args->fields[2].param > 0) {
		register_set(proc, args->fields[2].param, args->fields[0].param ^ args->fields[1].param );
		exec_op_hook(args, proc, "xor", args->len);
	}

	show_pc_movement(args, proc, "xor", args->len);
}

void	zjmp_exec(t_proc *proc, const t_op *op)
{
	t_args args;

	(void)op;
	if (proc->carry){
		args = (t_args){ .len = deref_short(proc, 1), .fields = { { .param = deref_short(proc, 1) } } };
		pc_incr(proc, deref_short(proc, 1));
		exec_op_hook(&args, proc, "zjmp", args.len);
		// show_pc_movement(&args, proc, "zjmp", args.len);
	}
	else {
		args = (t_args){ .len = 3, .fields = { { .param = deref_short(proc, 1) } } };
		pc_incr(proc, 3);
		exec_op_hook(&args, proc, "zjmp", args.len);
		show_pc_movement(&args, proc, "zjmp", args.len);
	}
}

void	fork_exec(t_proc *proc, const t_op *op)
{
	t_proc	*new_proc;
	t_word arg;
	t_args args;


	(void)op;
	new_proc = allocate_proc_node(proc->vm);
	*new_proc = *proc;
	new_proc->nbr = (++proc->vm->nb_proc);
	arg = deref_short(proc, 1);
	forward_pc(new_proc, arg % IDX_MOD);
	new_proc->wait = get_curr_op(new_proc).src->delay - 1;
	// new_proc->wait = 0;
	new_proc->live = false;

	args = (t_args){ .len = 3, .fields = { { .param = arg }, { .param = new_proc->pc } } };
	pc_incr(proc, 3);
	exec_op_hook(&args, proc, "fork", args.len);
	show_pc_movement(&args, proc, "fork", args.len);
}

void	lld_exec(t_proc *proc, const t_op *op)
{
	t_args *args = gather_args(proc, op);

	if (args->nbr != op->nbr_params || args->fields[1].param == 0)
		return (show_pc_movement(args, proc, "lld", args->len));

	if (args->fields[0].code == IND_CODE) {
		register_set(proc, args->fields[1].param, deref_short(proc, args->fields[0].param - args->len));
		args->fields[0].param = register_get(proc, args->fields[1].param);
	}
	else {
		register_set(proc, args->fields[1].param, args->fields[0].param);
	}

	if (register_get(proc, args->fields[1].param) == 0)
		proc->carry = true;
	else
		proc->carry = false;

	exec_op_hook(args, proc, "lld", args->len);
	show_pc_movement(args, proc, "lld", args->len);
}


void	lfork_exec(t_proc *proc, const t_op *op)
{
	t_proc	*new_proc;
	t_args args;
	t_word arg;

	(void)op;
	new_proc = allocate_proc_node(proc->vm);
	*new_proc = *proc;
	new_proc->nbr = (++proc->vm->nb_proc);
	arg = deref_short(proc, 1);
	new_proc->pc = arg % MEM_SIZE;
	new_proc->wait = get_curr_op(new_proc).src->delay - 1;
	new_proc->live = false;

	args = (t_args){ .len = 3, .fields = { { .param = arg }, { .param = proc->pc + arg } } };
	pc_incr(proc, 3);
	exec_op_hook(&args, proc, "lfork", args.len);
	show_pc_movement(&args, proc, "lfork", args.len);
}

void	aff_exec(t_proc *proc, const t_op *op)
{
	uint8_t	coding_byte;
	uint8_t	reg_num;

	t_args *args = gather_args(proc, op);

	coding_byte = deref(proc, 1);
	if (coding_byte != REG_CODE << 6) {
		return show_pc_movement(args, proc, "aff", args->len);
	}
	reg_num = deref(proc, 2);

	aff_hook(proc, register_get(proc, reg_num));
	show_pc_movement(args, proc, "aff", args->len);
}
