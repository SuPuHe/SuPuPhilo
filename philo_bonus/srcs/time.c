/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 16:38:30 by omizin            #+#    #+#             */
/*   Updated: 2025/06/27 16:38:38 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

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

void	smart_sleep(long ms)
{
	long	start;

	start = get_time();
	while (get_time() - start < ms)
		usleep(500);
}
