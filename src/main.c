#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_RESULT_STRING_LENGTH 32

#define error(...) (fprintf(stderr, __VA_ARGS__))

typedef struct {
    double left_border;
    double right_border;
} interval_t;

double rectangle_rule(interval_t interval);
double simpson_rule(interval_t interval);
double integrate(interval_t interval, int part_count, double (*rule)(interval_t));
char **calculate_integrals(interval_t interval, const int *nums, int n);

void read_interval(FILE *stream, interval_t *interval);
void free_memory(char **results, int n);

int main() {
    interval_t interval = {0, 0};
    read_interval(stdin, &interval);
    int nums[] = {5, 10,  20,  100,  500,  1000};
    int size = sizeof(nums) / sizeof(nums[0]);
    char **st = calculate_integrals(interval, nums, size);
    for (int i = 0; i < size; ++i) {
        if (fprintf(stdout, "%s", st[i]) < 0) {
            error("Cannot write %d result to stdout", i);
        }
    }
    free_memory(st, size);
    return 0;
}

double rectangle_rule(interval_t interval) {
    return (interval.right_border - interval.left_border) * sin((interval.left_border + interval.right_border) / 2);
}

double simpson_rule(interval_t interval) {
    double first_part = ((interval.right_border - interval.left_border) / 6.0);
    double second_part = (sin(interval.left_border) + (4 * sin((interval.left_border + interval.right_border) / 2)) + sin(interval.right_border));
    return first_part * second_part;
}

double integrate(interval_t interval, int part_count, double (*rule)(interval_t)) {
    double integrate_value = 0.0;
    double part_len = (interval.right_border - interval.left_border) / part_count;
    double part_left_border = interval.left_border;
    for (int i = 0; i < part_count; ++i) {
        double part_right_border = part_left_border + part_len;
        interval_t curr_interval = {part_left_border, part_right_border};
        integrate_value += rule(curr_interval);
        part_left_border = part_right_border;
    }
    return integrate_value;
}

char **calculate_integrals(interval_t interval, const int *nums, int n) {
    char **res = (char**)malloc(n * sizeof(char *));
    if (res == NULL) {
        return NULL;
    }
    for (int i = 0; i < n; ++i) {
        res[i] = (char*)malloc(sizeof(char) * MAX_RESULT_STRING_LENGTH);
        if (res[i] == NULL) {
            free_memory(res, i);
            error("Cannot write results to string in %d experiment\\n", i);
            return NULL;
        }
    }
    for (int i = 0; i < n; ++i) {
        double integral_rectangle = integrate(interval, nums[i], rectangle_rule);
        double integral_simpson = integrate(interval, nums[i], simpson_rule);
        sprintf(res[i], "%d\t%.5f\t%.5f\n", nums[i], integral_rectangle, integral_simpson);
    }
    return res;
}

void read_interval(FILE *stream, interval_t *interval) {
    if (fscanf(stream, "%lf %lf", &(interval->left_border), &(interval->right_border)) < 1) {
        error("Cannot read interval\n");
        read_interval(stream, interval);
    }
    if (interval->left_border < 0 || interval->right_border > M_PI) {
        error("Borders of the interval must be from 0 to PI\n");
        read_interval(stream, interval);
    }
    if (interval->left_border > interval->right_border) {
        error("Left border of the interval must be less than right\n");
        read_interval(stream, interval);
    }
}

void free_memory(char **results, int n) {
    for (int i = 0; i < n; ++i) {
        free(results[i]);
    }
    free(results);
}
