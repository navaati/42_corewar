#include "corewar_priv.h"

uint8_t gather_dir(t_proc *proc, const t_op *op, t_args *args, uint8_t i)
{
	args->len += 4;
	pc_incr(proc, 4);
	if (op->params[i] & T_DIR) {
		args->fields[i].param = deref_word(proc, -4);
		args->nbr += 1;
	}
	else
		return (0);
	return (1);
}

uint8_t gather_ind(t_proc *proc, const t_op *op, t_args *args, uint8_t i)
{
	args->len += 2;
	pc_incr(proc, 2);
	if (op->params[i] & T_IND) {
		args->fields[i].param = deref_short(proc, -2);
		args->nbr += 1;
	}
	else
		return (0);
	return (1);
}

uint8_t gather_reg(t_proc *proc, const t_op *op, t_args *args, uint8_t i)
{
	args->len += 1;
	pc_incr(proc, 1);
	if (op->params[i] & T_REG) {
		args->fields[i].param = deref(proc, -1);
		args->nbr += 1;
	}
	else
		return (0);
	return (1);
}
