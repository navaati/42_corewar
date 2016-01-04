/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   corewar.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lgillot- <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2015/10/11 22:27:37 by lgillot-          #+#    #+#             */
/*   Updated: 2015/10/12 17:15:10 by lgillot-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

t_offset	nop_exec(t_proc *proc)
{
	(void)proc;
	return (1);
}

t_offset	live_exec(t_proc *proc)
{
	t_word	champion;

	champion = deref_word(proc, 1);
	(void)champion; // TODO: live des champions
	proc->live = true;
	proc->vm->nbr_live++;
	return (5);
}

t_offset	ld_exec(t_proc *proc)
{
	uint8_t		coding_byte;
	uint8_t		coding_first;
	t_word		word;
	t_offset	length;
	uint8_t		reg_num;

	length = 1;
	coding_byte = deref(proc, length);
	length++;
	coding_first = (coding_byte >> 6) & 0xFF;
	if (coding_first == DIR_CODE)
	{
		word = deref_word(proc, length);
		length += 4;
	}
	else if (coding_first == IND_CODE)
	{
		word = deref_ind(proc, length);
		length += 2;
	}
	else
		return(0);
	reg_num = deref(proc, length);
	length++;
	proc->regs[reg_num] = word;
	return (length);
};

t_offset	zjmp_exec(t_proc *proc)
{
	if (proc->carry)
		return (deref_short(proc, 1));
	else
		return (3);
}

t_offset	aff_exec(t_proc *proc)
{
	uint8_t	coding_byte;
	uint8_t	reg_num;

	coding_byte = deref(proc, 1);
	if (coding_byte != REG_CODE << 6)
		return (0);
	reg_num = deref(proc, 2);
	ft_putchar(proc->regs[reg_num]);
	return (3);
}

const t_op	op_tab[] =
{
	{"nop",	0, nop_exec},
	{"live", 10, live_exec},
	{"ld", 5, ld_exec},
	{"st", 5, NULL},
	{"add", 10, NULL},
	{"sub", 10, NULL},
	{"and", 6, NULL},
	{"or", 6, NULL},
	{"xor", 6, NULL},
	{"zjmp", 20, zjmp_exec},
	{"ldi", 25, NULL},
	{"sti", 25, NULL},
	{"fork", 800, NULL},
	{"lld", 10, NULL},
	{"lldi", 50, NULL},
	{"lfork", 1000, NULL},
	{"aff", 2, aff_exec},
};

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
#ifdef DEBUG
		if (!op.exec)
		{
			DBG("Opcode not implemented: %s\n", op.name);
			op = op_tab[0];
		}
#endif
	}
	return (op);
}

void	step(t_proc *proc)
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

t_proc	init_proc(t_vm *vm, t_address pc)
{
	t_proc	proc;

	proc  = (t_proc){
		.vm = vm,
		.pc = pc,
	};
	proc.wait = get_curr_op(&proc).delay;
	return (proc);
}

void	massacre(t_vm *vm)
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
		}
	}
	vm->next_massacre += vm->cycle_to_die;
}

void	debug_cycles(t_vm *vm)
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

void	debug_proc(t_proc *proc)
{
	t_op op;

	op = get_curr_op(proc);
	DBG("pc: %u, curr_opcode: 0x%02x = %s, wait: %u\n",
		proc->pc, deref(proc, 0), op.name, proc->wait);
}

void	cycle(t_vm *vm)
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
}

int		main(int argc, char **argv)
{
	t_vm	vm;
	t_proc	*proc1;
	t_proc	*proc2;

	(void)argc;
	(void)argv;
	vm = (t_vm){
		.memory = {LD, DIR_CODE << 6 | REG_CODE << 4, 0, 0, 5, 57, 7,
				   NOP,
				   LD, IND_CODE << 6 | REG_CODE << 4, 0, 19, 13,
				   AFF, REG_CODE << 6, 7,
				   LIVE, 0, 0, 0, 0,
				   AFF, REG_CODE << 6, 13,
				   ZJMP, 255, 255 - 24,
				   /* data */0, 0, 0, 10},
		.procs = LIST_HEAD_INITIALIZER(&vm.procs),
		.cycle_to_die = CYCLE_TO_DIE,
		.next_massacre = CYCLE_TO_DIE,
	};
	proc1 = allocate_proc_node(&vm);
	*proc1 = init_proc(&vm, 0);
	proc1->carry = true;
	proc2 = allocate_proc_node(&vm);
	*proc2 = init_proc(&vm, 50);
	while (!LIST_EMPTY(&vm.procs))
		cycle(&vm);
	DBG("\nHalt\n");
	return (0);
}
