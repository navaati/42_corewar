#include <libft.h>

#include <corewar.h>

uint8_t	niner_code[] = {
	LD, DIR_CODE << 6 | REG_CODE << 4, 0, 0, 5, 57, 7,
	NOP,
	LD, IND_CODE << 6 | REG_CODE << 4, 0, 19, 13,
	AFF, REG_CODE << 6, 7,
	LIVE, 0, 0, 0, 0,
	AFF, REG_CODE << 6, 13,
	ZJMP, 255, 256 - 24,
	/* data */0, 0, 0, 10};

uint8_t noper_code[10] = { NOP };

int		main(int argc, char **argv)
{
	t_vm	vm;
	uint8_t	noper[sizeof(header_t) + sizeof(noper_code)];
	uint8_t	niner[sizeof(header_t) + sizeof(niner_code)];
	void	*files[2];
	size_t	sizes[2];

	(void)argc;
	(void)argv;

	*((header_t *)noper) = (header_t){
		.prog_name = "NOPer",
		.prog_size = sizeof(noper_code),
		.comment = "starts in NOP land"
	};
	ft_memcpy(noper + sizeof(header_t), noper_code, sizeof(noper_code));
	files[0] = noper;
	sizes[0] = sizeof(noper);

	*((header_t *)niner) = (header_t){
		.prog_name = "9er",
		.prog_size = sizeof(niner_code),
		.comment = "loop on printing nines and \\n's"
	};
	ft_memcpy(niner + sizeof(header_t), niner_code, sizeof(niner_code));
	files[1] = niner;
	sizes[1] = sizeof(niner);

	init_vm(&vm, files, sizes, 2);
	while (cycle(&vm))
		;
	ft_putstr("\nHalt: ");
	if (vm.winner == NO_CHAMPION)
		ft_putendl("nobody won\n");
	else
	{
		ft_putstr("winner is ");
		ft_putendl(vm.champions[vm.winner].name);
	}
	return (0);
}
