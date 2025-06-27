/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo_bonus.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: omizin <omizin@student.42heilbronn.de>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/16 09:58:25 by omizin            #+#    #+#             */
/*   Updated: 2025/06/27 20:16:37 by omizin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_BONUS_H
# define PHILO_BONUS_H

# include <sys/time.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <semaphore.h>
# include <signal.h>
# include <sys/wait.h>
# include <fcntl.h>
# include <sys/mman.h>

typedef struct s_input
{
	int		philo_num;
	int		die_time;
	int		eat_time;
	int		sleep_time;
	int		meal_num;
	long	start_time;
	pid_t	checker_pid;
	pid_t	killer_pid;
	sem_t	*sem_forks;
	sem_t	*sem_print;
	sem_t	*sem_meal_check;
	sem_t	*sem_stop;
	sem_t	*sem_meal_done;
	sem_t	*sem_kill_detect;
	sem_t	*sem_dead;
}	t_input;

typedef struct s_philo
{
	int		id;
	int		meals_eaten;
	long	last_meal_time;
	pid_t	pid;
	t_input	*input;
}	t_philo;

//input
int		get_input(int argc, char **argv, t_input *input);
//time
long	get_time(void);
long	get_current_ms(t_input *input);
void	smart_sleep(long ms);
//monitoring
void	*death_monitoring(void *arg);
//utils
int		ft_num_check(char *s);
long	ft_long_atoi(const char *string);
void	print_status(t_philo *philo, const char *msg);
//philo_loop
void	philo_loop(t_philo *philo);
//kill_process
void	destroy_all_semaphores(t_input *input);
void	wait_for_meals(t_input *input);
void	kill_process(t_philo *philos, t_input *input);
//init_info
int		init_semaphores(t_input *input);
int		init_info(t_input *input, t_philo **philos_out);
void	*death_monitoring(void *arg);

#endif
