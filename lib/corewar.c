#include <libft.h>
#include <stdlib.h>

#include "corewar_priv.h"
#include "hooks.h"

uint8_t assignate(uint8_t c, t_proc *proc, t_address addr)
{
	return (proc->vm->memory[(proc->pc + addr) % MEM_SIZE] = c);
}

t_word	assignate_word(t_word w, t_proc *proc, t_address addr)
{
	(void)w;
	(void)proc;
	(void)addr;

	proc->vm->memory[(proc->pc + addr) % MEM_SIZE] = (w >> 24) & 0xFF;
	proc->vm->memory[(proc->pc + addr + 1) % MEM_SIZE] = (w >> 16) & 0xFF;
	proc->vm->memory[(proc->pc + addr + 2) % MEM_SIZE] = (w >> 8) & 0xFF;
	proc->vm->memory[(proc->pc + addr + 3) % MEM_SIZE] = (w >> 0) & 0xFF;

	return (w);
}

// nonito

uint8_t	deref(t_proc *proc, t_address addr)
{
	return (proc->vm->memory[(proc->pc + addr) % MEM_SIZE]);
}

t_word		deref_word(t_proc *proc, t_address addr)
{
	t_word	word;

	word = deref(proc, addr) << 24;
	word |= deref(proc, addr + 1) << 16;
	word |= deref(proc, addr + 2) << 8;
	word |= deref(proc, addr + 3);
	return (word);
}

t_address	deref_short(t_proc *proc, t_address addr)
{
	t_address	ind;

	ind = deref(proc, addr) << 8;
	ind |= deref(proc, addr + 1);
	return (ind);
}

t_word		deref_ind(t_proc *proc, t_address addr)
{
	return(deref_word(proc, deref_short(proc, addr)));
}

t_proc		*allocate_proc_node(t_vm *vm)
{
	t_proc_node	*node;

	if (!(node = malloc(sizeof(*node))))
		return (NULL);
	LIST_INSERT_HEAD(&vm->procs, node, entries);
	return (&node->proc);
}

t_op	get_curr_op(t_proc *proc)
{
	t_opcode	opcode;
	t_op		op;

	opcode = deref(proc, 0);
	if (opcode > AFF)
		op = op_tab[0];
	else
	{
		op = op_tab[opcode];
#ifndef NDEBUG
		if (!op.exec)
		{
			DBG("Opcode not implemented: %s\n", op.name);
			op = op_tab[0];
		}
#endif
	}
	return (op);
}

static void	step(t_proc *proc, t_flags *flags)
{
	uint32_t	old_pos;
	t_op		curr_op;
	t_offset	length;

	step_proc_hook(proc);
	if (proc->wait)
		proc->wait--;
	else
	{
		curr_op = get_curr_op(proc);
		old_pos = proc->pc;
		length = curr_op.exec(proc) % MEM_SIZE;
		proc->pc = (proc->pc + length) % MEM_SIZE;
		if (flags->v & 0x10 && length != 1) {
			t_address min = old_pos > proc->pc ? proc->pc : old_pos;
			t_address max = old_pos < proc->pc ? proc->pc : old_pos;
				
			DBG("ADV %d (0x%04x -> 0x%04x) ", max - min, old_pos, proc->pc);
						
			if (ft_strcmp(curr_op.name, "zjmp"))
			{
				while (min != max) {
					DBG("%02x ", proc->vm->memory[min]);
					min += 1;
				}
			}

			DBG("\n");
		}
		curr_op = get_curr_op(proc);
		proc->wait = curr_op.delay - 1;
	}
}

static void	massacre(t_vm *vm)
{
	t_proc_node	*node;
	t_proc_node	*node_temp;

	massacre_hook(vm);
	assert(!LIST_EMPTY(&vm->procs));
	if (vm->nbr_live >= NBR_LIVE ||
		vm->last_cycles_decr >= MAX_CHECKS)
	{
		assert(vm->cycle_to_die >= CYCLE_DELTA);
		vm->cycle_to_die -= CYCLE_DELTA;
		decr_ctd_hook(vm);
		vm->last_cycles_decr = 0;
	}
	else
		vm->last_cycles_decr++;
	vm->nbr_live = 0;
	LIST_FOREACH_SAFE(node, &vm->procs, entries, node_temp)
	{
		if (!node->proc.live)
		{
			kill_proc_hook(&node->proc);
			LIST_REMOVE(node, entries);
			free(node);
		}
		else
			node->proc.live = false;
	}
	vm->next_massacre += vm->cycle_to_die;
}

bool	cycle(t_vm *vm, t_flags *flags)
{
	t_proc_node	*node;

	LIST_FOREACH(node, &vm->procs, entries)
		step(&node->proc, flags);

	if (flags->d != 0 && vm->cycles == flags->d)
	{
		int i = 0;
		while (i < MEM_SIZE) {
			if (i % 64 == 0)
				DBG("0x%04x : ", i);
			DBG("%02x ", vm->memory[i]);
			i += 1;
			if (i % 64 == 0)
				DBG("\n");
		}
		exit(0);
	}
	if (vm->cycles == vm->next_massacre)
		massacre(vm);
	vm->cycles++;
	return (!LIST_EMPTY(&vm->procs));
}

t_err	init_vm(t_vm *vm, const t_frontend *frontend,
				void *files[], size_t sizes[], size_t nb_champs)
{
	t_offset	pc_distance;
	size_t		i;

	*vm = (t_vm){
		.frontend = *frontend,
		.memory = {},
		.procs = LIST_HEAD_INITIALIZER(&vm->procs),
		.champions = {},
		.nb_champions = 0,
		.winner = NO_CHAMPION,
		.cycle_to_die = CYCLE_TO_DIE,
		.next_massacre = CYCLE_TO_DIE,
	};
	pc_distance = MEM_SIZE / nb_champs;
	i = 0;
	DBG("VM INIT\n");
	while (i < nb_champs)
	{
		if (!load_champion_from_file(vm, i * pc_distance, files[i], sizes[i]))
			return (ERR);
		DBG("A new challenger is comin'\n");
		i++;
	}
	return (OK);
}
