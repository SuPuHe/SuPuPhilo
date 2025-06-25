/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 11:30:17 by omizin            #+#    #+#             */
/*   Updated: 2025/06/25 12:22:37 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	check_if_died(t_philo *p, t_input *input,
pthread_mutex_t *print_mutex)
{
	long	time_since_meal;

	pthread_mutex_lock(&p->meal_mutex);
	time_since_meal = get_current_ms(input) - p->last_meal_time;
	pthread_mutex_unlock(&p->meal_mutex);
	if (time_since_meal >= input->die_time + input->delay)
	{
		pthread_mutex_lock(print_mutex);
		printf("time: %ld, id: %d died\n", get_current_ms(input), p->id);
		pthread_mutex_unlock(print_mutex);
		pthread_mutex_lock(&input->death_mutex);
		input->death_flag = 1;
		pthread_mutex_unlock(&input->death_mutex);
		return (1);
	}
	return (0);
}

static int	check_all_full(t_philo *philos, t_input *input,
pthread_mutex_t *print_mutex)
{
	int	i;
	int	full_count;

	i = 0;
	full_count = 0;
	while (i < input->philo_num)
	{
		pthread_mutex_lock(&philos[i].meal_mutex);
		if (input->meal_num != -1 && philos[i].meals_eaten >= input->meal_num)
			full_count++;
		pthread_mutex_unlock(&philos[i].meal_mutex);
		i++;
	}
	if (full_count == input->philo_num)
	{
		pthread_mutex_lock(print_mutex);
		printf("All philosophers have eaten %d times.\n", input->meal_num);
		pthread_mutex_unlock(print_mutex);
		pthread_mutex_lock(&input->death_mutex);
		input->death_flag = 1;
		pthread_mutex_unlock(&input->death_mutex);
		return (1);
	}
	return (0);
}

void	*death_monitoring(void *arg)
{
	t_monitoring	*mon;
	t_input			*input;
	t_philo			*philos;
	int				i;

	mon = (t_monitoring *)arg;
	input = mon->input;
	philos = mon->philos;
	while (1)
	{
		i = 0;
		while (i < input->philo_num)
		{
			if (check_if_died(&philos[i], input, mon->print_mutex))
				return (NULL);
			i++;
		}
		if (input->meal_num != -1
			&& check_all_full(philos, input, mon->print_mutex))
			return (NULL);
		usleep(100);
	}
	return (NULL);
}
