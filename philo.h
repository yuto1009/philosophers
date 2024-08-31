#ifndef PHILO_H
#define PHILO_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

typedef struct s_philo
{
    int id;
    long last_meal_time;
    int meals_eaten;
    pthread_t thread;
    pthread_mutex_t *left_fork;
    pthread_mutex_t *right_fork;
    struct s_data *data;
} t_philo;

typedef struct s_data
{
    int number_of_philosophers;
    int time_to_die;
    int time_to_eat;
    int time_to_sleep;
    int number_of_times_each_philosopher_must_eat;
    long start_time;
    pthread_mutex_t *forks;
    pthread_mutex_t print_lock;
    t_philo *philos;
    int stop;
} t_data;

long get_time_in_ms();
void init_data(t_data *data, int argc, char **argv);
void init_philos(t_data *data);
void *philosopher_routine(void *arg);
void start_simulation(t_data *data);
void print_state(t_philo *philo, char *state);

#endif