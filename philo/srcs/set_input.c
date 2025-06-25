/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   set_input.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 11:36:27 by omizin            #+#    #+#             */
/*   Updated: 2025/06/25 12:24:48 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	set_input_info(t_input *input, int argc, char **argv)
{
	input->philo_num = ft_long_atoi(argv[1]);
	if (input->philo_num == 0)
		return (0);
	else if (input->philo_num < 10)
		input->delay = 0;
	else
		input->delay = 1;
	input->fork_num = ft_long_atoi(argv[1]);
	input->die_time = ft_long_atoi(argv[2]);
	input->eat_time = ft_long_atoi(argv[3]);
	input->sleep_time = ft_long_atoi(argv[4]);
	input->start_time = get_time();
	if (argc == 6)
		input->meal_num = ft_long_atoi(argv[5]);
	else
		input->meal_num = -1;
	return (1);
}

int	get_input(int argc, char **argv, t_input *input)
{
	int		i;

	if (argc != 5 && argc != 6)
	{
		printf("Error input\n");
		return (0);
	}
	i = 1;
	while (i < argc)
	{
		if (ft_num_check(argv[i]) == 0)
			return (printf("Error input\n"), 0);
		i++;
	}
	if (!set_input_info(input, argc, argv))
	{
		printf("Error input\n");
		return (0);
	}
	return (1);
}
