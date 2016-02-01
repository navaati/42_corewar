/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adebray <adebray@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2013/12/17 09:17:41 by adebray           #+#    #+#             */
/*   Updated: 2015/04/28 13:35:54 by adebray          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>
#include <stdlib.h>

void					use_a_d(char ar, t_flags *flags, va_list ap)
{
	int					d;
	char				*nbr;

	if (ar == 'd' || ar == 'i')
	{
		d = va_arg(ap, int);
		ft_putnbr(d);
		nbr = ft_itoa(d);
		flags->cmp += ft_strlen(nbr);
		free(nbr);
	}
	else if (ar == 'c')
	{
		d = va_arg(ap, int);
		ft_putchar(d);
		flags->cmp += 1;
	}
	return ;
}

void					use_a_u(char ar, t_flags *flags, va_list ap)
{
	unsigned int		u;

	if (ar == 'u')
	{
		u = va_arg(ap, unsigned int);
		if (u > 9)
			flags->cmp += ft_printf("%d", u / 10);
		flags->cmp += ft_printf("%d", u % 10);
	}
	else if (ar == 'o')
	{
		u = va_arg(ap, unsigned int);
		flags->cmp += ft_putoctal(u);
	}
	else if (ar == 'X')
	{
		u = va_arg(ap, unsigned int);
		flags->cmp += ft_put_sheum_hexa(u);
	}
	else if (ar == 'x')
	{
		u = va_arg(ap, unsigned int);
		flags->cmp += ft_puthexa(u);
	}
	return ;
}

void					print_arguments(char ar, t_flags *flags, va_list ap)
{
	char				*s;
	long unsigned int	ul;

	use_a_d(ar, flags, ap);
	use_a_u(ar, flags, ap);
	if (ar == 's')
	{
		s = va_arg(ap, char*);
		if (s == NULL)
			flags->cmp += ft_printf("(null)");
		else
			ft_putstr(s);
		flags->cmp += ft_strlen(s);
	}
	else if (ar == 'p')
	{
		ul = va_arg(ap, long unsigned int);
		flags->cmp += ft_printf("%s", "0x");
		flags->cmp += ft_puthexa(ul);
	}
	else if (ar == '%')
		flags->cmp += ft_printf("%c", '%');
	else if (ar == 'Z')
		flags->cmp += ft_printf("%c", 'Z');
}

t_flags					*parse(char *str, t_flags *flags, va_list ap)
{
	while (ft_isflags(*str))
	{
		get_flags(flags, str);
		str += 1;
	}
	while (ft_isdigit(*str))
	{
		get_width(flags, *str);
		str += 1;
	}
	print_arguments(*str, flags, ap);
	return (flags);
}

int						ft_printf(const char *format, ...)
{
	int					ret;
	va_list				ap;
	t_flags				*flags;

	flags = NULL;
	va_start(ap, format);
	flags = flags_init(flags);
	while (*format)
	{
		if (*format != '%')
		{
			ft_putchar(*format);
			flags->cmp += 1;
		}
		else
		{
			format += 1;
			flags = parse((char*)format, flags, ap);
		}
		format++;
	}
	va_end(ap);
	ret = flags->cmp;
	free(flags);
	return (ret);
}
