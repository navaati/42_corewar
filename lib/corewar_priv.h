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

typedef struct	s_champ_desc
{
	char		*name;
	char		*comment;
	uint16_t	size;
	void		*buf;
}				t_champ_desc;

uint8_t		deref(t_proc *proc, t_address addr);
t_word		deref_word(t_proc *proc, t_address addr);
t_address	deref_short(t_proc *proc, t_address addr);
t_word		deref_ind(t_proc *proc, t_address addr);
t_proc		*allocate_proc_node(t_vm *vm);
t_op		get_curr_op(t_proc *proc);
t_err		load_champion_from_file(t_vm *vm, t_address pc,
									void *file, size_t size);

extern const t_op	op_tab[];

#endif
