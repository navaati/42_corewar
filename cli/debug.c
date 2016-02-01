#include <libft.h>

#include <corewar.h>

#include "cli.h"

static void	debug_step_proc(t_proc *proc)
{
	(void)proc;
	DBG("pc: %u, curr_opcode: 0x%02x, wait: %u\n",
		proc->pc, deref(proc, 0), proc->wait);
}

static void	debug_exec_op(t_proc *proc, char *op_name, t_offset op_length)
{
	(void)proc;
	(void)op_name;
	(void)op_length;
	DBG("Do %s\n", op_name);
}

static void	debug_massacre(t_vm *vm)
{
	(void)vm;
	DBG("%u cycles elapsed: massacre !\n", vm->cycle_to_die);
	DBG("Nbr lives: %u\n", vm->nbr_live);
	DBG("Massacres since last cycle_to_die decrementation: %u\n",
		vm->last_cycles_decr);
}

static void	debug_decr_ctd(t_vm *vm)
{
	(void)vm;
	DBG("Decrementing cycle_to_die of %u cycles to %u\n",
		CYCLE_DELTA, vm->cycle_to_die);
}

static void	debug_kill_proc(t_proc *proc)
{
	(void)proc;
	DBG("Batte en mousse at 0x%02x\n", proc->pc);
}

void		debug_cycles(t_vm *vm)
{
	char	progress[40];
	int		progress_size;
	int		cycles_done;
	int		progress_filled;

	progress_size = sizeof(progress);
	ft_memset(progress, ' ', progress_size);
	cycles_done = vm->cycle_to_die - (vm->next_massacre - vm->cycles);
	progress_filled = progress_size * cycles_done / vm->cycle_to_die;
	ft_memset(progress, '#', progress_filled);
	DBG("Cycle %u [%.*s]\n", vm->cycles, progress_size, progress);
}

const t_frontend	debug_frontend = {
	.step_proc = debug_step_proc,
	.exec_op = debug_exec_op,
	.massacre = debug_massacre,
	.decr_ctd = debug_decr_ctd,
	.new_proc = NULL,
	.kill_proc = debug_kill_proc,
	.aff = cli_aff,
};
