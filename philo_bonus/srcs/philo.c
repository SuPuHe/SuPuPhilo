/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 11:26:00 by omizin            #+#    #+#             */
/*   Updated: 2025/06/27 18:49:41 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

int	main(int argc, char **argv)
{
	t_input	input;
	t_philo	*philos;

	if (!get_input(argc, argv, &input))
		return (1);
	if (!init_semaphores(&input))
		return (1);
	if (!init_info(&input, &philos))
		return (1);
	sem_wait(input.sem_stop);
	kill_process(philos, &input);
	destroy_all_semaphores(&input);
	free(philos);
	return (0);
}
