#ifndef COREWAR_PRIV_H
# define COREWAR_PRIV_H

# include <sys/queue.h>

# include "op.h"

typedef uint8_t		t_opcode;
typedef uint16_t	t_address;
typedef t_address	t_offset;
typedef uint32_t	t_word;

_Static_assert(sizeof(t_address) == IND_SIZE, "IND_SIZE != sizeof(t_address)");

# define NOP 0x00
# define LIVE 0x01
# define LD 0x02
# define ST 0x03
# define ADD 0x04
# define SUB 0x05
# define AND 0x06
# define OR 0x07
# define XOR 0x08
# define ZJMP 0x09
# define LDI 0x0A
# define STI 0x0B
# define FORK 0x0C
# define LLD 0x0D
# define LLDI 0x0E
# define LFORK 0x0F
# define AFF 0x10

typedef struct s_proc_node	t_proc_node;

typedef struct	s_champion
{
	char	name[PROG_NAME_LENGTH + 1];
	char	comment[COMMENT_LENGTH + 1];
}				t_champion;

# define NO_CHAMPION (-1)

typedef struct	s_vm
{
	uint8_t		memory[MEM_SIZE];
	LIST_HEAD(, s_proc_node)	procs;
	t_champion	champions[MAX_PLAYERS];
	size_t		nb_champions;
	ssize_t		winner;
	uint32_t	cycles;
	uint32_t	next_massacre;
	uint16_t	cycle_to_die;
	uint16_t	last_cycles_decr;
	uint16_t	nbr_live;
}				t_vm;

typedef struct	s_proc
{
	t_vm		*vm;
	t_address	pc;
	bool		carry;
	t_word		regs[REG_NUMBER];
	uint16_t	wait;
	bool		live;
}				t_proc;

typedef t_offset(*t_op_exec)(t_proc *proc);

typedef struct	s_op
{
	char		*name;
	uint16_t	delay;
	t_op_exec	exec;
}				t_op;

struct			s_proc_node
{
	t_proc					proc;
	LIST_ENTRY(s_proc_node)	entries;
};

#endif
