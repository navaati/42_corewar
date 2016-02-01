#include "hooks.h"

void	step_proc_hook(t_proc *proc)
{
	if (proc->vm->frontend.step_proc)
		proc->vm->frontend.step_proc(proc);
}

void	exec_op_hook(t_proc *proc, char *op_name, t_offset op_length)
{
	if (proc->vm->frontend.exec_op)
		proc->vm->frontend.exec_op(proc, op_name, op_length);
}

void	massacre_hook(t_vm *vm)
{
	if (vm->frontend.massacre)
		vm->frontend.massacre(vm);
}

void	decr_ctd_hook(t_vm *vm)
{
	if (vm->frontend.decr_ctd)
		vm->frontend.decr_ctd(vm);
}

void	new_proc_hook(t_proc *proc)
{
	if (proc->vm->frontend.new_proc)
		proc->vm->frontend.new_proc(proc);
}

void	kill_proc_hook(t_proc *proc)
{
	if (proc->vm->frontend.kill_proc)
		proc->vm->frontend.kill_proc(proc);
}

void	aff_hook(t_proc *proc, char c)
{
	if (proc->vm->frontend.aff)
		proc->vm->frontend.aff(proc, c);
}
