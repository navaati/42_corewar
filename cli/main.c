#include <libft.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include <corewar.h>

#include "cli.h"

static t_err	read_file(const char *filename, void **file, size_t *size)
{
	int			fd;
	header_t	header;
	size_t		l_size;
	void		*l_file;

	if((fd = open(filename, O_RDONLY)) < 0)
	{
		ft_putstr_fd("Cannot open ", 2);
		ft_putendl_fd(filename, 2);
		return (ERR);
	}
	if (!ft_fread(fd, &header, sizeof(header_t)))
	{
		ft_putendl_fd("Read failed", 2);
		GOTO(close);
	}
	l_size = sizeof(header_t) + be32toh(header.prog_size);
	if (!(l_file = malloc(l_size)))
	{
		ft_putstr_fd("Cannot allocate memory", 2);
		GOTO(close);
	}
	*((header_t *)l_file) = header;
	if (!ft_fread(fd, l_file + sizeof(header_t), be32toh(header.prog_size)))
	{
		ft_putendl_fd("Read failed", 2);
		GOTO(free);
	}
	close(fd);
	*size = l_size;
	*file = l_file;
	return (OK);
free:
	free(l_file);
close:
	close(fd);
	return (ERR);
}

void	cli_aff(t_proc *proc, char c)
{
	(void)proc;
	ft_putchar(c);
}

static t_err	init(t_vm *vm, int argc, char **argv)
{
	t_err	ret;
	void	*files[MAX_PLAYERS];
	size_t	sizes[MAX_PLAYERS];
	int		i;

	ret = ERR;
	i = 0;
	while (i < argc)
	{
		if (!read_file(argv[i], &files[i], &sizes[i]))
			GOTO(free);
		i++;
	}
	ret = init_vm(vm, &zaz_frontend, files, sizes, i);
free:
	while (i)
	{
		free(files[i - 1]);
		i--;
	}
	return (ret);
}

void	usage(int argc, char **argv)
{
	(void)argc;
	ft_putstr("Usage: ");
	ft_putendl(argv[0]);
	ft_putendl("\t-d X");
	ft_putendl("\t-v X");
	ft_putendl("\tchampion1 [champion2] [champion3] [champion4]");
	exit(-1);
}

extern t_flags g_flags;

int		get_opt(int argc, char **argv)
{
	int ch;

	while ((ch = getopt(argc, argv, "d:v:")) != -1)
	{
		if (ch == 'd')
			g_flags.d = atoi(optarg);
		else if (ch == 'v')
			g_flags.v = (unsigned char)atoi(optarg);
		else
			usage(argc, argv);
	}

	if (argc < 2 || argc - optind > MAX_PLAYERS)
		usage(argc, argv);
	return (optind);
}

int		main(int argc, char **argv)
{
	t_vm	vm;

	int optind = get_opt(argc, argv);

	if (!init(&vm, argc - optind, argv + optind))
		return (1);
	while (cycle(&vm, &g_flags))
		debug_cycles(&vm);
	// ft_putstr("\nHalt: ");
	if (vm.winner == NO_CHAMPION)
		ft_putendl("nobody won\n");
	else
	{
		ft_putstr("winner is ");
		ft_putendl(vm.champions[vm.winner - 1].name);
	}
	return (0);
}
