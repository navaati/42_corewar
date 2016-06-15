#include <libft.h>

#include "corewar_priv.h"

static void		copy_to_memory(t_vm *vm, t_address pc, void *buf, t_offset size)
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

static t_err	load_champion(t_vm *vm, t_address pc, const t_champ_desc *desc)
{
	static uint32_t nbr;
	t_champion	*champ;
	t_proc		*proc;

	assert(vm->nb_champions <= MAX_PLAYERS);
	if (vm->nb_champions >= MAX_PLAYERS)
		return (ERR);
	champ = &vm->champions[vm->nb_champions];
	ft_memcpy(champ->name, desc->name, PROG_NAME_LENGTH);
	champ->name[PROG_NAME_LENGTH] = '\0';
	ft_memcpy(champ->comment, desc->comment, COMMENT_LENGTH);
	champ->comment[COMMENT_LENGTH] = '\0';
	copy_to_memory(vm, pc, desc->buf, desc->size);
	proc = allocate_proc_node(vm);
	vm->nb_champions--;
	*proc = (t_proc){
		.vm = vm,
		.nbr = ++nbr,
		.pc = pc,
		.carry = false,
		.regs = { vm->nb_champions, 0 },
		.live = false
	};
	proc->wait = get_curr_op(proc).src->delay;
	return (OK);
}

t_err			load_champion_from_file(t_vm *vm, t_address pc, void *file, size_t size)
{
	header_t		*header;
	t_champ_desc	desc;

	if (size <= sizeof(header_t))
		return (ERR);
	header = (header_t *)file;
	if (be32toh(header->magic) != COREWAR_EXEC_MAGIC)
		return (ERR);
	desc.name = header->prog_name;
	desc.comment = header->comment;
	desc.size = be32toh(header->prog_size);
	if (desc.size > CHAMP_MAX_SIZE ||
		size - sizeof(header_t) < desc.size)
		return (ERR);
	desc.buf = file + sizeof(header_t);
	return (load_champion(vm, pc, &desc));
}
