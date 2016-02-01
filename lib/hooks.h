#ifndef HOOKS_H
# define HOOKS_H

# include <corewar.h>

void	step_proc_hook(t_proc *proc);
void	exec_op_hook(t_proc *proc, char *op_name, t_offset op_length);
void	massacre_hook(t_vm *vm);
void	decr_ctd_hook(t_vm *vm);
void	new_proc_hook(t_proc *proc);
void	kill_proc_hook(t_proc *proc);
void	aff_hook(t_proc *proc, char c);

#endif
