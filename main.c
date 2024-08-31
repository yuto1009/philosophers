#include "philo.h"

long get_time_in_ms()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    return (time.tv_sec * 1000) + (time.tv_usec / 1000);
}

void init_data(t_data *data, int argc, char **argv)
{
    data->number_of_philosophers = atoi(argv[1]);
    data->time_to_die = atoi(argv[2]);
    data->time_to_eat = atoi(argv[3]);
    data->time_to_sleep = atoi(argv[4]);
    if (argc == 6)
        data->number_of_times_each_philosopher_must_eat = atoi(argv[5]);
    else
        data->number_of_times_each_philosopher_must_eat = -1;
    data->start_time = get_time_in_ms();
    data->stop = 0;
    pthread_mutex_init(&data->print_lock, NULL);
    data->forks = malloc(sizeof(pthread_mutex_t) * data->number_of_philosophers);
    for (int i = 0; i < data->number_of_philosophers; i++)
        pthread_mutex_init(&data->forks[i], NULL);
}

void init_philos(t_data *data)
{
    data->philos = malloc(sizeof(t_philo) * data->number_of_philosophers);
    for (int i = 0; i < data->number_of_philosophers; i++)
    {
        data->philos[i].id = i + 1;
        data->philos[i].last_meal_time = get_time_in_ms();
        data->philos[i].meals_eaten = 0;
        data->philos[i].left_fork = &data->forks[i];
        data->philos[i].right_fork = &data->forks[(i + 1) % data->number_of_philosophers];
        data->philos[i].data = data;
    }
}

void *philosopher_routine(void *arg)
{
    t_philo *philo = (t_philo *)arg;
    t_data *data = philo->data;

    while (!data->stop)
    {
        pthread_mutex_lock(philo->left_fork);
        print_state(philo, "has taken a fork");
        pthread_mutex_lock(philo->right_fork);
        print_state(philo, "is eating");

        philo->last_meal_time = get_time_in_ms();
        philo->meals_eaten++;
        usleep(data->time_to_eat * 1000);

        pthread_mutex_unlock(philo->right_fork);
        pthread_mutex_unlock(philo->left_fork);
        print_state(philo, "is sleeping");
        usleep(data->time_to_sleep * 1000);

        print_state(philo, "is thinking");
    }
    return NULL;
}

void start_simulation(t_data *data)
{
    for (int i = 0; i < data->number_of_philosophers; i++)
        pthread_create(&data->philos[i].thread, NULL, philosopher_routine, &data->philos[i]);

    while (!data->stop)
    {
        for (int i = 0; i < data->number_of_philosophers; i++)
        {
            if (get_time_in_ms() - data->philos[i].last_meal_time > data->time_to_die)
            {
                print_state(&data->philos[i], "died");
                data->stop = 1;
                break;
            }
        }
    }
    for (int i = 0; i < data->number_of_philosophers; i++)
        pthread_join(data->philos[i].thread, NULL);
}

void print_state(t_philo *philo, char *state)
{
    pthread_mutex_lock(&philo->data->print_lock);
    printf("%ld %d %s\n", get_time_in_ms() - philo->data->start_time, philo->id, state);
    pthread_mutex_unlock(&philo->data->print_lock);
}

int main(int argc, char **argv)
{
    if (argc != 5 && argc != 6)
    {
        printf("Usage: %s number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]\n", argv[0]);
        return 1;
    }

    t_data data;
    init_data(&data, argc, argv);
    init_philos(&data);
    start_simulation(&data);

    free(data.forks);
    free(data.philos);
    return 0;
}