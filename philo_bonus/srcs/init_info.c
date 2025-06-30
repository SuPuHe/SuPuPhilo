/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_info.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 17:24:40 by omizin            #+#    #+#             */
/*   Updated: 2025/06/30 13:34:11 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

void	*death_monitoring(void *arg)
{
	t_philo	*philo;
	long	time_since_meal;

	philo = (t_philo *)arg;
	while (1)
	{
		usleep(1000);
		sem_wait(philo->input->sem_meal_check);
		time_since_meal = get_current_ms(philo->input) - philo->last_meal_time;
		sem_post(philo->input->sem_meal_check);
		if (time_since_meal >= philo->input->die_time)
		{
			print_status(philo, "died");
			sem_post(philo->input->sem_stop);
			if (philo->input->meal_num != -1)
				sem_post(philo->input->sem_kill_detect);
			exit(1);
		}
	}
	return (NULL);
}

static void	init_philos(t_philo *philos, t_input *input)
{
	int	i;

	i = 0;
	while (i < input->philo_num)
	{
		philos[i].id = i + 1;
		philos[i].meals_eaten = 0;
		philos[i].input = input;
		philos[i].last_meal_time = get_current_ms(input);
		i++;
	}
}

static int	create_processes(t_input *input, t_philo *philos)
{
	int		i;
	pid_t	pid;

	i = 0;
	while (i < input->philo_num)
	{
		pid = fork();
		if (pid == -1)
			return (0);
		else if (pid == 0)
		{
			philo_loop(&philos[i]);
			exit(0);
		}
		else
			philos[i].pid = pid;
		i++;
	}
	return (1);
}

int	init_info(t_input *input, t_philo **philos_out)
{
	t_philo	*philos;

	philos = malloc(sizeof(t_philo) * input->philo_num);
	if (!philos)
		return (0);
	init_philos(philos, input);
	if (!create_processes(input, philos))
		return (0);
	if (input->meal_num != -1)
		wait_for_meals(input);
	*philos_out = philos;
	return (1);
}

int	init_semaphores(t_input *input)
{
	sem_unlink("/sem_forks");
	sem_unlink("/sem_print");
	sem_unlink("/sem_meal_check");
	sem_unlink("/sem_stop");
	sem_unlink("/sem_meal_done");
	sem_unlink("/sem_kill_detect");
	sem_unlink("/sem_dead");
	input->sem_forks = sem_open("/sem_forks", O_CREAT, 0644, input->philo_num);
	input->sem_print = sem_open("/sem_print", O_CREAT, 0644, 1);
	input->sem_meal_check = sem_open("/sem_meal_check", O_CREAT, 0644, 1);
	input->sem_stop = sem_open("/sem_stop", O_CREAT, 0644, 0);
	input->sem_meal_done = sem_open("/sem_meal_done", O_CREAT, 0644, 0);
	input->sem_kill_detect = sem_open("/sem_kill_detect", O_CREAT, 0644, 0);
	input->sem_dead = sem_open("/sem_dead", O_CREAT, 0644, 1);
	if (input->sem_forks == SEM_FAILED || input->sem_print == SEM_FAILED
		|| input->sem_meal_check == SEM_FAILED || input->sem_stop == SEM_FAILED
		|| input->sem_meal_done == SEM_FAILED
		|| input->sem_kill_detect == SEM_FAILED
		|| input->sem_dead == SEM_FAILED)
	{
		perror("sem_open failed");
		return (0);
	}
	return (1);
}
