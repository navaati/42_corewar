#ifndef CLI_H
# define CLI_H

# include <corewar.h>

void		cli_aff(t_proc *proc, char c);
void		debug_cycles(t_vm *vm);
extern const t_frontend	debug_frontend;
extern const t_frontend	zaz_frontend;

#endif
