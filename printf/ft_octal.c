/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_octal.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adebray <adebray@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2013/12/20 12:38:16 by adebray           #+#    #+#             */
/*   Updated: 2013/12/22 14:29:38 by adebray          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <ft_printf.h>

int							ft_putoctal(unsigned int decimal)
{
	static int				cmp;

	cmp = 0;
	if (decimal < 8)
		ft_printf("%d", decimal);
	else
	{
		ft_putoctal(decimal / 8);
		ft_printf("%d", decimal % 8);
	}
	cmp += 1;
	return (cmp);
}
