/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_sheum_hexa.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adebray <adebray@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2013/12/21 16:06:37 by adebray           #+#    #+#             */
/*   Updated: 2013/12/22 14:26:43 by adebray          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>

static int			ft_print_x(unsigned int decimal)
{
	int				cmp;

	cmp = 0;
	if (decimal == 10)
		cmp += ft_printf("%c", 'A');
	else if (decimal == 11)
		cmp += ft_printf("%c", 'B');
	else if (decimal == 12)
		cmp += ft_printf("%c", 'C');
	else if (decimal == 13)
		cmp += ft_printf("%c", 'D');
	else if (decimal == 14)
		cmp += ft_printf("%c", 'E');
	else if (decimal == 15)
		cmp += ft_printf("%c", 'F');
	return (cmp);
}

int					ft_put_sheum_hexa(unsigned int decimal)
{
	int				cmp;

	cmp = 0;
	if (decimal < 16)
	{
		if (decimal >= 10)
			cmp += ft_print_x(decimal);
		else
			cmp += ft_printf("%x", decimal);
	}
	else
	{
		cmp += ft_put_sheum_hexa(decimal / 16);
		cmp += ft_printf("%x", decimal % 16);
	}
	return (cmp);
}
