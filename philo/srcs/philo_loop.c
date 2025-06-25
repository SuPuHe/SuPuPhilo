/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_loop.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/25 11:21:43 by omizin            #+#    #+#             */
/*   Updated: 2025/06/25 12:23:59 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static void	*handle_one_philo(t_philo *philo)
{
	print_status(philo, "is thinking");
	pthread_mutex_lock(philo->fork_right);
	print_status(philo, "has taken a fork");
	smart_sleep(philo->input->die_time, philo);
	pthread_mutex_unlock(philo->fork_right);
	return (NULL);
}

static int	philo_thinking_sleeping(t_philo *philo, int helper)
{
	if (helper == 0)
		print_status(philo, "is thinking");
	else if (helper == 1)
	{
		print_status(philo, "is sleeping");
		smart_sleep(philo->input->sleep_time, philo);
	}
	return (!is_dead(philo));
}

static void	update_meals(t_philo *philo)
{
	pthread_mutex_lock(&philo->meal_mutex);
	philo->last_meal_time = get_current_ms(philo->input);
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_mutex);
}

static int	philo_eating(t_philo *philo)
{
	if (philo->id % 2 == 0)
	{
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(philo->fork_right);
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(&philo->fork_left);
	}
	else
	{
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(&philo->fork_left);
		print_status(philo, "has taken a fork");
		pthread_mutex_lock(philo->fork_right);
	}
	update_meals(philo);
	print_status(philo, "is eating");
	smart_sleep(philo->input->eat_time, philo);
	pthread_mutex_unlock(&philo->fork_left);
	pthread_mutex_unlock(philo->fork_right);
	if (philo->input->meal_num != -1
		&& philo->meals_eaten >= philo->input->meal_num)
		return (0);
	return (!is_dead(philo));
}

void	*philo_loop(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->id % 2 != 0)
		usleep(500);
	if (philo->input->philo_num == 1)
		return (handle_one_philo(philo));
	while (!is_dead(philo))
	{
		if (!philo_thinking_sleeping(philo, 0))
			break ;
		if (!philo_eating(philo))
			break ;
		if (!philo_thinking_sleeping(philo, 1))
			break ;
	}
	return (NULL);
}
