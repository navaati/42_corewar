/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tools.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adebray <adebray@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2013/12/22 14:16:14 by adebray           #+#    #+#             */
/*   Updated: 2013/12/24 21:16:29 by adebray          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>
#include <stdlib.h>

t_flags			*flags_init(t_flags *flags)
{
	flags = malloc(sizeof(t_flags));
	flags->minus = 0;
	flags->plus = 0;
	flags->zero = 0;
	flags->space = 0;
	flags->width = 0;
	return (flags);
}

void			get_flags(t_flags *flags, char *str)
{
	if (*str == '-')
		flags->minus = 1;
	else if (*str == '0')
		flags->zero = 1;
	else if (*str == '+')
		flags->plus = 1;
	else if (*str == ' ')
		flags->space = 1;
}

int				ft_isflags(char str)
{
	if (str == '-' || str == '0' || str == '+' || str == ' ')
		return (1);
	else
		return (0);
}

void			get_width(t_flags *flags, char str)
{
	flags->width *= 10;
	flags->width += str - '0';
}
