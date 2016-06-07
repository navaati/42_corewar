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

typedef void(*t_debug_function)(t_proc *proc, char *op_name, t_offset op_length);

static void debug_std(t_proc *proc, char *op_name, t_offset op_length)
{
	(void)proc;
	(void)op_name;
	(void)op_length;

	if (g_flags.v & 0x04)
		DBG("P\tX | %s \n", op_name);
}

static void debug_zjmp(t_proc *proc, char *op_name, t_offset op_length)
{
	(void)proc;
	(void)op_name;
	(void)op_length;
	if (!(g_flags.v & 0x04))
		return ;

	if (proc->carry)
		DBG("P\tX | zjmp OK\n");
	else
		DBG("P\tX | zjmp FAILED\n");
}

static t_debug_function debug_array[16] = {
	debug_std,
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

static void zaz_exec_op(t_proc *proc, char *op_name, t_offset op_length)
{
	int i = 0;
	while (i < 16)
	{
		if (op_table[i] == op_name)
			return debug_array[i](proc, op_name, op_length);
		i += 1;
	}
}

void		debug_cycles(t_vm *vm)
{
	(void)vm;
	if (g_flags.v & 0x02)
		DBG("It is now cycle %d\n", vm->cycles);
}

const t_frontend	zaz_frontend = {
	.step_proc = NULL,
	.exec_op = zaz_exec_op,
	.massacre = NULL,
	.decr_ctd = NULL,
	.new_proc = NULL,
	.kill_proc = NULL,
	.aff = NULL
};
