#ifndef COREWAR_H
# define COREWAR_H

# include <libft.h>
# ifdef USE_LIBBSD_QUEUE_H
#  include <bsd/sys/queue.h>
# else
#  include <sys/queue.h>
# endif

# include "op.h"

typedef uint8_t		t_opcode;
typedef uint16_t	t_address;
typedef t_address	t_offset;
typedef uint32_t	t_word;

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

typedef struct s_vm	t_vm;

typedef struct	s_proc
{
	t_vm		*vm;
	t_address	pc;
	bool		carry;
	t_word		regs[REG_NUMBER];
	uint16_t	wait;
	bool		live;
	uint32_t	nbr;
	void		*priv;
}				t_proc;

typedef struct	s_proc_node
{
	t_proc					proc;
	LIST_ENTRY(s_proc_node)	entries;
}				t_proc_node;

typedef struct	s_champion
{
	char	name[PROG_NAME_LENGTH + 1];
	char	comment[COMMENT_LENGTH + 1];
}				t_champion;

typedef struct s_args t_args;

typedef struct	s_frontend
{
	void(*step_proc)(t_proc *proc);
	void(*exec_op)(t_args *p, t_proc *proc, char *name, t_offset op_length);
	void(*massacre)(t_vm *vm);
	void(*decr_ctd)(t_vm *vm);
	void(*new_proc)(t_proc *proc);
	void(*kill_proc)(t_proc *proc);
	void(*aff)(t_proc *proc, char c);
	void	*priv;
}				t_frontend;

# define NO_CHAMPION (-1)

typedef struct	s_vm
{
	t_frontend	frontend;
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

t_err	init_vm(t_vm *vm, const t_frontend *frontend,
				void *files[], size_t sizes[], size_t nb_champs);
uint8_t		deref(t_proc *proc, t_address addr);
t_word		deref_word(t_proc *proc, t_address addr);
t_address	deref_short(t_proc *proc, t_address addr);


// nonito

typedef t_offset(*t_op_exec)(t_proc *proc);

typedef struct	s_op
{
	char		*name;
	uint16_t	delay;
	t_op_exec	exec;
}				t_op;

t_op		get_curr_op(t_proc *proc);

typedef struct		s_flags
{
	unsigned char	v;
	uint32_t		d;
}					t_flags;

bool	cycle(t_vm *vm, t_flags *flags);

uint8_t assignate(uint8_t c, t_proc *proc, t_address addr);
t_word	assignate_word(t_word w, t_proc *proc, t_address addr);

typedef struct	s_field
{
	uint8_t		code;
	t_word		param;	
}				t_field;

typedef struct	s_args
{
	uint8_t		nbr;
	uint8_t		len;
	t_field		fields[4];
}				t_args;


#endif
