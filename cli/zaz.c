#include <libft.h>

#include <corewar.h>

#include "cli.h"

t_flags g_flags = {
	.v = 0
};

static char *op_table[16] = {
	"live",
	"ld",
	"st",
	"add",
	"sub",
	"and",
	"or",
	"xor",
	"zjmp",
	"ldi",
	"sti",
	"fork",
	"lld",
	"lldi",
	"lfork",
	"aff",
};

typedef void(*t_debug_function)(t_args *p, t_proc *proc, char *op_name, t_offset op_length);

static void debug_std(t_args *args, t_proc *proc, char *op_name, t_offset op_length)
{
	(void)proc;
	(void)op_name;
	(void)op_length;

	int i = 0;
	DBG("P    %d | %s", proc->nbr, op_name);
	while (i < args->nbr)
	{
		if (args->fields[i].code == REG_CODE)
			DBG(" r%d", args->fields[i].param);
		else
			DBG(" %d", args->fields[i].param);
		i += 1;
	}
	DBG("\n");
}

static void debug_zjmp(t_args *p, t_proc *proc, char *op_name, t_offset op_length)
{
	(void)p;
	(void)proc;
	(void)op_name;
	(void)op_length;
	if (g_flags.v & 0x04)
	{
		if (proc->carry)
			DBG("P    %d | zjmp %d OK\n", proc->nbr, (signed short)p->fields[0].param);
		else
			DBG("P    %d | zjmp %d FAILED\n", proc->nbr, (signed short)p->fields[0].param);
	}
}

static void debug_live(t_args *p, t_proc *proc, char *op_name, t_offset op_length)
{
	(void)p;
	(void)proc;
	(void)op_name;
	(void)op_length;
	if (g_flags.v & 0x04)
	{
		if (proc->carry)
			DBG("P    %d | live %d\n", proc->nbr, (signed short)p->fields[0].param);
		else
			DBG("P    %d | live %d\n", proc->nbr, (signed short)p->fields[0].param);
	}
}

static t_debug_function debug_array[16] = {
	debug_live,
	debug_std,
	debug_std,
	debug_std,
	debug_std,
	debug_std,
	debug_std,
	debug_std,
	debug_zjmp,
	debug_std,
	debug_std,
	debug_std,
	debug_std,
	debug_std,
	debug_std,
	debug_std,
};

static void zaz_exec_op(t_args *p, t_proc *proc, char *op_name, t_offset op_length)
{
	int i = 0;

	while (i < 16)
	{
		if (op_table[i] == op_name)
			return debug_array[i](p, proc, op_name, op_length);
		i += 1;
	}
}

void		debug_cycles(t_vm *vm)
{
	(void)vm;
	if (g_flags.v & 0x02)
		DBG("It is now cycle %d\n", vm->cycles);
}

static void zaz_decr_ctd(t_vm *vm) {
	DBG("Cycle to die is now %d\n", vm->cycle_to_die);
}

const t_frontend	zaz_frontend = {
	.step_proc = NULL,
	.exec_op = zaz_exec_op,
	.massacre = NULL,
	.decr_ctd = zaz_decr_ctd,
	.new_proc = NULL,
	.kill_proc = NULL,
	.aff = NULL
};
