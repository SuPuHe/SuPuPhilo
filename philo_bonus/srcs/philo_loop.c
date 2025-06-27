/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_loop.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 16:56:03 by omizin            #+#    #+#             */
/*   Updated: 2025/06/27 17:09:14 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

static void	handle_one_philo(t_philo *philo)
{
	print_status(philo, "is thinking");
	sem_wait(philo->input->sem_forks);
	print_status(philo, "has taken a fork");
	smart_sleep(philo->input->die_time);
	print_status(philo, "died");
	sem_post(philo->input->sem_stop);
	exit(1);
}

static int	philo_thinking_sleeping(t_philo *philo, int helper)
{
	if (helper == 0)
		print_status(philo, "is thinking");
	else if (helper == 1)
	{
		print_status(philo, "is sleeping");
		smart_sleep(philo->input->sleep_time);
	}
	return (1);
}

static void	update_meals(t_philo *philo)
{
	sem_wait(philo->input->sem_meal_check);
	philo->last_meal_time = get_current_ms(philo->input);
	philo->meals_eaten++;
	sem_post(philo->input->sem_meal_check);
}

static int	philo_eating(t_philo *philo)
{
	sem_wait(philo->input->sem_forks);
	print_status(philo, "has taken a fork");
	sem_wait(philo->input->sem_forks);
	print_status(philo, "has taken a fork");
	print_status(philo, "is eating");
	update_meals(philo);
	smart_sleep(philo->input->eat_time);
	sem_post(philo->input->sem_forks);
	sem_post(philo->input->sem_forks);
	if (philo->input->meal_num != -1
		&& philo->meals_eaten >= philo->input->meal_num)
	{
		sem_post(philo->input->sem_meal_done);
		return (0);
	}
	return (1);
}

void	philo_loop(t_philo *philo)
{
	pthread_t	death_thread;

	if (philo->id % 2 == 0)
		usleep(500);
	pthread_create(&death_thread, NULL, death_monitoring, philo);
	pthread_detach(death_thread);
	if (philo->input->philo_num == 1)
		handle_one_philo(philo);
	while (1)
	{
		philo_thinking_sleeping(philo, 0);
		if (!philo_eating(philo))
			break ;
		philo_thinking_sleeping(philo, 1);
	}
	exit(0);
}
