/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kill_processes.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/27 17:13:53 by omizin            #+#    #+#             */
/*   Updated: 2025/06/27 18:49:37 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo_bonus.h"

void	destroy_all_semaphores(t_input *input)
{
	sem_close(input->sem_forks);
	sem_close(input->sem_print);
	sem_close(input->sem_meal_check);
	sem_close(input->sem_stop);
	sem_close(input->sem_meal_done);
	sem_close(input->sem_kill_detect);
	sem_close(input->sem_dead);
	sem_unlink("/sem_forks");
	sem_unlink("/sem_print");
	sem_unlink("/sem_meal_check");
	sem_unlink("/sem_stop");
	sem_unlink("/sem_meal_done");
	sem_unlink("/sem_kill_detect");
	sem_unlink("/sem_dead");
}

static void	start_killer_process(t_input *input)
{
	pid_t	killer;

	killer = fork();
	if (killer == 0)
	{
		sem_wait(input->sem_kill_detect);
		sem_post(input->sem_stop);
		exit(0);
	}
	input->killer_pid = killer;
}

void	wait_for_meals(t_input *input)
{
	pid_t	checker;
	int		i;

	checker = fork();
	if (checker == 0)
	{
		i = 0;
		while (i < input->philo_num)
		{
			sem_wait(input->sem_meal_done);
			i++;
		}
		sem_wait(input->sem_print);
		printf("All philosophers have eaten %d times.\n", input->meal_num);
		sem_post(input->sem_print);
		sem_post(input->sem_stop);
		exit(0);
	}
	input->checker_pid = checker;
	start_killer_process(input);
}

void	kill_process(t_philo *philos, t_input *input)
{
	int	i;

	i = 0;
	while (i < input->philo_num)
	{
		kill(philos[i].pid, SIGKILL);
		i++;
	}
	if (input->meal_num != -1)
	{
		kill(input->checker_pid, SIGKILL);
		kill(input->killer_pid, SIGKILL);
		waitpid(input->checker_pid, NULL, 0);
		waitpid(input->killer_pid, NULL, 0);
	}
}
