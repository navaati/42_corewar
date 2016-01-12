#ifndef COREWAR_PRIV_H
# define COREWAR_PRIV_H

# include <corewar.h>

_Static_assert(sizeof(t_address) == IND_SIZE, "IND_SIZE != sizeof(t_address)");

typedef t_offset(*t_op_exec)(t_proc *proc);

typedef struct	s_op
{
	char		*name;
	uint16_t	delay;
	t_op_exec	exec;
}				t_op;

uint8_t		deref(t_proc *proc, t_address addr);
t_word		deref_word(t_proc *proc, t_address addr);
t_address	deref_short(t_proc *proc, t_address addr);
t_word		deref_ind(t_proc *proc, t_address addr);
t_proc		*allocate_proc_node(t_vm *vm);

extern const t_op	op_tab[];

#endif
