/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 09:58:25 by omizin            #+#    #+#             */
/*   Updated: 2025/06/17 12:13:19 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <sys/time.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <semaphore.h>

typedef struct s_input
{
	int		philo_num;
	int		die_time;
	int		eat_time;
	int		sleep_time;
	int		meal_num;
	long	start_time;
	int		stop_flag;
	sem_t	*sem_forks;
	sem_t	*sem_print;
	sem_t	*sem_meal_check;
	sem_t	*sem_stop;
}	t_input;

typedef struct s_philo
{
	int		id;
	int		meals_eaten;
	long	last_meal_time;
	pid_t	pid;
	t_input	*input;
}	t_philo;

typedef struct s_monitor
{
	t_philo			*philos;
	t_input			*input;
	pthread_mutex_t	*print_mutex;
}	t_monitoring;

//logic
void	*philo_loop(void *arg);
//monitoring
void	*death_monitoring(void *arg);
//input
int		get_input(int argc, char **argv, t_input *input);
//init information
int		init_info(t_input *input);
//heplers
void	print_status(t_philo *philo, const char *msg);
int		is_dead(t_philo *philo);
long	ft_long_atoi(const char *string);
int		ft_num_check(char *s);
int		allocate_memory(t_input *input, pthread_t **threads, t_philo **philos);
//time
long	get_time(void);
long	get_current_ms(t_input *input);
void	smart_sleep(long ms, t_philo *philo);

#endif
