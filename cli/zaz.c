#include <libft.h>

#include <corewar.h>

#include "cli.h"

t_flags g_flags = {
	.v = 0
};

typedef void(*t_debug_function)(t_args *p, t_proc *proc, char *op_name, t_offset op_length);

void show_pc_movement(t_args *args, t_proc *proc, char *op_name, t_offset op_length)
{
	(void)args;
	(void)proc;
	(void)op_name;
	(void)op_length;

	if (g_flags.v & 0x10) {
		t_address min = proc->pc - op_length > proc->pc ? proc->pc : proc->pc - op_length;
		t_address max = proc->pc - op_length < proc->pc ? proc->pc : proc->pc - op_length;

		DBG("ADV %d (0x%04x -> 0x%04x) ", max - min, proc->pc - op_length, proc->pc);

		while (min != max) {
			DBG("%02x ", proc->vm->memory[min]);
			min += 1;
		}

		DBG("\n");
	}
}

static void debug_std(t_args *args, t_proc *proc, char *op_name, t_offset op_length)
{
	int		i;

	i = 0;
	(void)op_length;
	if (g_flags.v & 0x04)
	{
		DBG("P %4d | %s", proc->nbr, op_name);
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
}

static void debug_ldi(t_args *args, t_proc *proc, char *op_name, t_offset op_length)
{
	int		i;

	i = 0;
	(void)op_length;
	if (g_flags.v & 0x04)
	{
		DBG("P %4d | %s", proc->nbr, op_name);
		while (i < args->nbr)
		{
			if (args->fields[i].code == REG_CODE && i == 2)
				DBG(" r%d", args->fields[i].param);
			else
				DBG(" %d", (signed short)args->fields[i].param);
			i += 1;
		}
		DBG("\n");
	}
}

static void debug_or(t_args *args, t_proc *proc, char *op_name, t_offset op_length)
{
	int		i;

	i = 0;
	(void)op_length;
	if (g_flags.v & 0x04)
	{
		DBG("P %4d | %s", proc->nbr, op_name);
		while (i < args->nbr)
		{
			if (args->fields[i].code == REG_CODE && i == 2)
				DBG(" r%d", args->fields[i].param);
			else
				DBG(" %d", args->fields[i].param);
			i += 1;
		}
		DBG("\n");
	}
}

static void debug_xor(t_args *args, t_proc *proc, char *op_name, t_offset op_length)
{
	int		i;

	i = 0;
	(void)op_length;
	if (g_flags.v & 0x04)
	{
		DBG("P %4d | %s", proc->nbr, op_name);
		while (i < args->nbr)
		{
			if (args->fields[i].code == REG_CODE && i == 2)
				DBG(" r%d", args->fields[i].param);
			else
				DBG(" %d", args->fields[i].param);
			i += 1;
		}
		DBG("\n");
	}
}

static void debug_st(t_args *args, t_proc *proc, char *op_name, t_offset op_length)
{
	int		i;

	i = 0;
	(void)op_length;
	if (g_flags.v & 0x04)
	{
		DBG("P %4d | %s", proc->nbr, op_name);
		while (i < args->nbr)
		{
			if (args->fields[i].code == REG_CODE && i == 0)
				DBG(" r%d", args->fields[i].param);
			else
				DBG(" %d", (signed short)args->fields[i].param);
			i += 1;
		}
		DBG("\n");
	}
}

static void debug_sti(t_args *args, t_proc *proc, char *op_name, t_offset op_length)
{
	int		i;

	i = 0;
	(void)op_length;
	if (g_flags.v & 0x04)
	{
		DBG("P %4d | %s", proc->nbr, op_name);
		while (i < args->nbr)
		{
			if (args->fields[i].code == REG_CODE && i == 0)
				DBG(" r%d", args->fields[i].param);
			else
				DBG(" %d", (short)args->fields[i].param);
			i += 1;
		}
		DBG("\n");
	}
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
			DBG("P %4d | zjmp %d OK\n", proc->nbr, (short)p->fields[0].param);
		else {
			DBG("P %4d | zjmp %d FAILED\n", proc->nbr, (short)p->fields[0].param);
		}
	}
}

static void debug_live(t_args *p, t_proc *proc, char *op_name, t_offset op_length)
{
	(void)p;
	(void)proc;
	(void)op_name;
	(void)op_length;
	// if (g_flags.v & 0x04)
		DBG("P %4d | live %d\n", proc->nbr, (int)p->fields[0].param);
}

static void debug_fork(t_args *p, t_proc *proc, char *op_name, t_offset op_length)
{
	(void)p;
	(void)proc;
	(void)op_name;
	(void)op_length;
	if (g_flags.v & 0x04)
		DBG("P %4d | fork %d (%d)\n", proc->nbr, p->fields[0].param, p->fields[1].param);
}

static void debug_lfork(t_args *p, t_proc *proc, char *op_name, t_offset op_length)
{
	(void)p;
	(void)proc;
	(void)op_name;
	(void)op_length;
	if (g_flags.v & 0x04)
		DBG("P %4d | lfork %d (%d)\n", proc->nbr, (int)p->fields[0].param, (int)p->fields[1].param);
}

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

static t_debug_function debug_array[16] = {
	/* live  */ debug_live,
	/* ld    */ debug_std,
	/* st    */ debug_st,
	/* add   */ debug_std,
	/* sub   */ debug_std,
	/* and   */ debug_std,
	/* or    */ debug_or,
	/* xor   */ debug_xor,
	/* zjmp  */ debug_zjmp,
	/* ldi   */ debug_ldi,
	/* sti   */ debug_sti,
	/* fork  */ debug_fork,
	/* lld   */ debug_std,
	/* lldi  */ debug_std,
	/* lfork */ debug_lfork,
	/* aff   */ debug_std,
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
