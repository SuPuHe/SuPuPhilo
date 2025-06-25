/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 11:32:05 by omizin            #+#    #+#             */
/*   Updated: 2025/06/25 13:50:46 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long	get_current_ms(t_input *input)
{
	return (get_time() - input->start_time);
}

void	smart_sleep(long ms, t_philo *philo)
{
	long	start;

	start = get_time();
	while (!is_dead(philo) && get_time() - start < ms)
		usleep(500);
}
