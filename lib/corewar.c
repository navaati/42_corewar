#include <libft.h>
#include <stdlib.h>

#include "corewar_priv.h"

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

static void	step(t_proc *proc)
{
	t_op		curr_op;
	t_offset	length;

	if (proc->wait)
		proc->wait--;
	else
	{
		curr_op = get_curr_op(proc);
		DBG("Do %s\n", curr_op.name);
		length = curr_op.exec(proc) % MEM_SIZE;
		proc->pc = (proc->pc + length) % MEM_SIZE;
		curr_op = get_curr_op(proc);
		proc->wait = curr_op.delay;
	}
}

static void	massacre(t_vm *vm)
{
	t_proc_node	*node;
	t_proc_node	*node_temp;

	assert(!LIST_EMPTY(&vm->procs));
	DBG("Massacre !\n");
	DBG("Nbr lives: %u\n", vm->nbr_live);
	DBG("Massacres since last cycle_to_die decrementation: %u\n",
		vm->last_cycles_decr);
	if (vm->nbr_live >= NBR_LIVE ||
		vm->last_cycles_decr >= MAX_CHECKS)
	{
		assert(vm->cycle_to_die >= CYCLE_DELTA);
		vm->cycle_to_die -= CYCLE_DELTA;
		DBG("Decrementing cycle_to_die of %u cycles\n", CYCLE_DELTA);
		vm->last_cycles_decr = 0;
	}
	else
		vm->last_cycles_decr++;
	DBG("Cycles to die: %u\n", vm->cycle_to_die);
	vm->nbr_live = 0;
	LIST_FOREACH_SAFE(node, &vm->procs, entries, node_temp)
	{
		if (!node->proc.live)
		{
			DBG("Batte en mousse at 0x%02x\n", node->proc.pc);
			LIST_REMOVE(node, entries);
			free(node);
		}
		else
			node->proc.live = false;
	}
	vm->next_massacre += vm->cycle_to_die;
}

static void	debug_cycles(t_vm *vm)
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

static void	debug_proc(t_proc *proc)
{
	t_op op;

	op = get_curr_op(proc);
        (void)op;
	DBG("pc: %u, curr_opcode: 0x%02x = %s, wait: %u\n",
		proc->pc, deref(proc, 0), op.name, proc->wait);
}

bool	cycle(t_vm *vm)
{
	t_proc_node	*node;

	debug_cycles(vm);
	LIST_FOREACH(node, &vm->procs, entries)
	{
		debug_proc(&node->proc);
		step(&node->proc);
	}
	if (vm->cycles == vm->next_massacre)
		massacre(vm);
	vm->cycles++;
	return (!LIST_EMPTY(&vm->procs));
}

static void	copy_to_memory(t_vm *vm, t_address pc, void *buf, t_offset size)
{
	t_offset	room_until_end;

	room_until_end = MEM_SIZE - pc;
	if (size < room_until_end)
		ft_memcpy(vm->memory + pc, buf, size);
	else
	{
		ft_memcpy(vm->memory + pc, buf, room_until_end);
		ft_memcpy(vm->memory, buf + room_until_end, size - room_until_end);
	}
}

static t_err	load_champion(t_vm *vm, t_address pc, header_t *header, void *buf)
{
	t_champion	*champ;
	t_proc		*proc;

	assert(vm->nb_champions <= MAX_PLAYERS);
	if (vm->nb_champions >= MAX_PLAYERS)
		return (ERR);
	champ = &vm->champions[vm->nb_champions];
	ft_memcpy(champ->name, header->prog_name, PROG_NAME_LENGTH);
	champ->name[PROG_NAME_LENGTH] = '\0';
	ft_memcpy(champ->comment, header->comment, COMMENT_LENGTH);
	champ->comment[COMMENT_LENGTH] = '\0';
	copy_to_memory(vm, pc, buf, header->prog_size);
	proc = allocate_proc_node(vm);
	*proc = (t_proc){
		.vm = vm,
		.pc = pc,
		.carry = false,
		.regs = { vm->nb_champions, 0 },
		.live = false
	};
	proc->wait = get_curr_op(proc).delay;
	proc->carry = true; // temporary to allow jumping
	vm->nb_champions++;
	return (OK);
}

static t_err	load_champion_from_file(t_vm *vm, t_address pc, void *file, size_t size)
{
	header_t	*header;
	void		*buf;

	if (size <= sizeof(header_t))
		return (ERR);
	header = (header_t *)file;
	if (size - sizeof(header_t) < header->prog_size)
		return (ERR);
	buf = file + sizeof(header_t);
	return (load_champion(vm, pc, header, buf));
}

t_err	init_vm(t_vm *vm,
				void *files[], size_t sizes[], size_t nb_champs)
{
	t_offset	pc_distance;
	size_t		i;

	*vm = (t_vm){
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
	while (i < nb_champs)
	{
		if (!load_champion_from_file(vm, i * pc_distance, files[i], sizes[i]))
			return (ERR);
		i++;
	}
	return (OK);
}
