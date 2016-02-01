/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adebray <adebray@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2013/12/03 00:16:53 by adebray           #+#    #+#             */
/*   Updated: 2013/12/22 14:28:10 by adebray          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

static void		reverse(char *str, int length)
{
	int			i;
	int			j;
	char		tmp;

	i = 0;
	j = length - 1;
	while (i < j)
	{
		tmp = str[i];
		str[i] = str[j];
		str[j] = tmp;
		i++;
		j--;
	}
}

char			*ft_itoa(int n)
{
	char		*result;
	int			sign;
	int			i;

	if (n == -2147483648)
		return ("-2147483648");
	sign = n < 0 ? 1 : 0;
	i = 0;
	result = malloc(sizeof(char *) * 12);
	if (n == 0)
		result[i++] = '0';
	else if (n < 0)
	{
		result[i++] = '-';
		n = -n;
	}
	while (n > 0)
	{
		result[i++] = '0' + n % 10;
		n /= 10;
	}
	reverse(result + sign, i - sign);
	result[i] = '\0';
	return (result);
}
