/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/18 11:26:00 by omizin            #+#    #+#             */
/*   Updated: 2025/06/26 12:12:59 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_status(t_philo *philo, const char *msg)
{
	sem_wait(philo->input->sem_print);
		printf("time: %ld, id: %d %s\n",
			get_current_ms(philo->input), philo->id, msg);
	sem_post(philo->input->sem_print);
}

long	ft_long_atoi(const char *string)
{
	int		i;
	int		negative;
	long	result;

	i = 0;
	negative = 1;
	result = 0;
	while (string[i] == ' ' || (string[i] >= 9 && string[i] <= 13))
		i++;
	if (string[i] == '+' || string[i] == '-')
	{
		if (string[i] == '-')
			negative *= -1;
		i++;
	}
	while (string[i] >= '0' && string[i] <= '9')
	{
		result = (result * 10) + (string[i] - 48);
		i++;
	}
	return (result * negative);
}

int	ft_num_check(char *s)
{
	if (s[0] == '+' && s[1] == '0' && s[2] == '0')
		return (0);
	if (s[0] == '0' && (s[1] >= '0' && s[1] <= '9'))
		return (0);
	if (s[0] >= '0' && s[0] <= '9')
		return (1);
	else if ((s[0] == '+') && (s[1] >= '0' && s[1] <= '9'))
		return (1);
	else
		return (0);
}

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

static int	set_input_info(t_input *input, int argc, char **argv)
{
	input->philo_num = ft_long_atoi(argv[1]);
	if (input->philo_num == 0)
		return (0);
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

void	handle_one_philo(t_philo *philo)
{
	print_status(philo, "is thinking");
	sem_wait(philo->input->sem_forks);
	print_status(philo, "has taken a fork");
	smart_sleep(philo->input->die_time);
	print_status(philo, "died");
	sem_post(philo->input->sem_stop);
	exit(1);
}


int	philo_thinking_sleeping(t_philo *philo, int helper)
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

void	update_meals(t_philo *philo)
{
	sem_wait(philo->input->sem_meal_check);
	philo->last_meal_time = get_current_ms(philo->input);
	philo->meals_eaten++;
	sem_post(philo->input->sem_meal_check);
}

int	philo_eating(t_philo *philo)
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
		return (0);
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
		if (!philo_thinking_sleeping(philo, 0))
			break ;
		if (!philo_eating(philo))
			break ;
		if (!philo_thinking_sleeping(philo, 1))
			break ;
	}
	exit(0);
}

void	*check_if_died(void *arg)
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
			sem_wait(philo->input->sem_print);
			printf("time: %ld, id: %d died\n",
				get_current_ms(philo->input), philo->id);
			sem_post(philo->input->sem_stop);
			exit(1);
		}
	}
	return (NULL);
}

void	*check_all_full(void *arg)
{
	t_philo	*philos = (t_philo *)arg;
	t_input	*input = philos[0].input;
	int		i;

	while (1)
	{
		int	full_count = 0;
		i = 0;
		while (i < input->philo_num)
		{
			sem_wait(input->sem_meal_check);
			if (input->meal_num != -1 && philos[i].meals_eaten >= input->meal_num)
				full_count++;
			sem_post(input->sem_meal_check);
			i++;
		}
		if (full_count == input->philo_num)
		{
			sem_wait(input->sem_print);
			printf("All philosophers have eaten %d times.\n", input->meal_num);
			sem_post(input->sem_stop);
			exit(0);
		}
		usleep(1000);
	}
	return (NULL);
}

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
			sem_wait(philo->input->sem_print);
			printf("time: %ld, id: %d died\n", get_current_ms(philo->input), philo->id);
			sem_post(philo->input->sem_stop);
			exit(1);
		}
	}
	return (NULL);
}

void	init_philos(t_philo *philos, t_input *input)
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

int	create_processes(t_input *input, t_philo *philos)
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

void	wait_for_processes(t_input *input, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < input->philo_num)
	{
		waitpid(philos[i].pid, NULL, 0);
		i++;
	}
}

void	destroy_all_semaphores(t_input *input)
{
	sem_close(input->sem_forks);
	sem_close(input->sem_print);
	sem_close(input->sem_meal_check);
	sem_close(input->sem_stop);
	sem_unlink("/sem_forks");
	sem_unlink("/sem_print");
	sem_unlink("/sem_meal_check");
	sem_unlink("/sem_stop");
}

int	init_info(t_input *input, t_philo **philos_out)
{
	t_philo		*philos;
	pthread_t	meals_thread;

	philos = malloc(sizeof(t_philo) * input->philo_num);
	if (!philos)
		return (0);
	init_philos(philos, input);
	if (!create_processes(input, philos))
		return (0);
	if (input->meal_num != -1)
	{
		pthread_create(&meals_thread, NULL, check_all_full, philos);
		pthread_detach(meals_thread);
	}
	*philos_out = philos;
	return (1);
}


int	init_semaphores(t_input *input)
{
	sem_unlink("/sem_forks");
	sem_unlink("/sem_print");
	sem_unlink("/sem_meal_check");
	sem_unlink("/sem_stop");
	input->sem_forks = sem_open("/sem_forks", O_CREAT, 0644, input->philo_num);
	input->sem_print = sem_open("/sem_print", O_CREAT, 0644, 1);
	input->sem_meal_check = sem_open("/sem_meal_check", O_CREAT, 0644, 1);
	input->sem_stop = sem_open("/sem_stop", O_CREAT, 0644, 0);
	if (input->sem_forks == SEM_FAILED || input->sem_print == SEM_FAILED
		|| input->sem_meal_check == SEM_FAILED || input->sem_stop == SEM_FAILED)
	{
		perror("sem_open failed");
		return (0);
	}
	return (1);
}

int	main(int argc, char **argv)
{
	t_input	input;
	t_philo	*philos;
	int		i;

	if (!get_input(argc, argv, &input))
		return (1);
	printf("meals %d\n", input.meal_num);
	if (!init_semaphores(&input))
		return (1);
	if (!init_info(&input, &philos))
		return (1);
	sem_wait(input.sem_stop);
	i = 0;
	while (i < input.philo_num)
	{
		kill(philos[i].pid, SIGKILL);
		i++;
	}
	destroy_all_semaphores(&input);
	free(philos);
	return (0);
}
