#include <libft.h>

#include "corewar_priv.h"
#include "corewar.h"
#include "hooks.h"

static uint8_t index_gather_dir(t_proc *proc, const t_op *op, t_args *args, uint8_t i)
{
	args->len += 2;
	proc->pc += 2;
	if (op->params[i] & T_DIR) {
		args->fields[i].param = deref_short(proc, -2);
		args->nbr += 1;
	}
	else
		return (0);
	return (1);
}

static gather_f gather_table[] = {
	NULL,
	gather_reg,
	index_gather_dir,
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
	proc->pc += args.len;

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

void ldi_exec(t_proc *proc, const t_op *op)
{
	t_args *args = gather_args(proc, op);

	(void)op;
	if (args->nbr != op->nbr_params) {
		return (show_pc_movement(args, proc, "ldi", args->len));
	}

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

	register_set(proc, args->fields[2].param, deref_word(proc, (args->fields[0].param + args->fields[1].param) % IDX_MOD - args->len));

	exec_op_hook(args, proc, "ldi", args->len);
	show_pc_movement(args, proc, "ldi", args->len);
}

void sti_exec(t_proc *proc, const t_op *op)
{
	t_args *args = gather_args(proc, op);

	(void)op;
	if (args->nbr != op->nbr_params) {
		return (show_pc_movement(args, proc, "sti", args->len));
	}

	if (args->fields[1].code == REG_CODE) {
		args->fields[1].param = register_get(proc, args->fields[1].param);
	}
	else if (args->fields[1].code == IND_CODE) {
		args->fields[1].param = deref_word(proc, args->fields[1].param - args->len);
	}

	if (args->fields[2].code == REG_CODE) {
		args->fields[2].param = register_get(proc, args->fields[2].param);
	}
	else if (args->fields[2].code == IND_CODE) {
		args->fields[2].param = deref_word(proc, args->fields[2].param - args->len);
	}

	assignate_word( register_get(proc, args->fields[0].param), proc, (args->fields[1].param + args->fields[2].param) % IDX_MOD - args->len);

	exec_op_hook(args, proc, "sti", args->len);
	show_pc_movement(args, proc, "sti", args->len);
}

void lldi_exec(t_proc *proc, const t_op *op)
{
	t_args *args = gather_args(proc, op);

	(void)op;
	if (args->nbr != op->nbr_params) {
		return (show_pc_movement(args, proc, "lldi", args->len));
	}

	if (args->fields[0].code == REG_CODE) {
		args->fields[0].param = register_get(proc, args->fields[0].param);
	}
	else if (args->fields[0].code == IND_CODE) {
		args->fields[0].param = deref_word(proc, args->fields[0].param - args->len);
		args->fields[0].param = deref_word(proc, args->fields[0].param - args->len);
	}

	if (args->fields[1].code == REG_CODE) {
		args->fields[1].param = register_get(proc, args->fields[1].param);
	}
	else if (args->fields[1].code == IND_CODE) {
		args->fields[1].param = deref_word(proc, args->fields[1].param - args->len);
	}

	register_set(proc, args->fields[2].param, deref_word(proc, args->fields[0].param + args->fields[1].param));

	exec_op_hook(args, proc, "lldi", args->len);
	show_pc_movement(args, proc, "lldi", args->len);
}
