/* Reads a sequence of data from standard input, smotths the data, then prints
 * the times when the smoothed data exceeds the critical value. Also prints the
 * peak acceleration and the time at which it occured.
 *
 * Josie Smith, 01/08/2019
 */

#include <stdio.h>
#define CRITICAL_HIGH  9.81
#define CRITICAL_LOW 5.0
#define MAX_DATA 100000

/* Reads a sequence of white-space-separated double-precision floating point
 * numbers from standard input into data array.
 */
int readDoubles(int n, double data[])
{
    int i = 0;
    while ( i<n && scanf("%lf", &data[i]) != EOF ) {
        i++;
    }
    return i;
}

/* Smooths the data by replacing each value with the weighted average of its
 * neighbours, ratio 1:2:1
 */
void smoothData(int n, double data[])
{
    int i=0;
    double temp = data[i];
    data[i] = (3*data[i] + data[i+1])/4;
    for (i=1; i<n-1; i++) {
        double newtemp = data[i];
        data[i] = (temp + 2*data[i] + data[i+1])/4;
        temp = newtemp;
    }
    data[i] = (temp + 3*data[i])/4;
}

/* Finds the maximum value in an array, and changes the time varaible to
 * the time this occured.
 */
double max(int n, double data[], int *time)
{
    int i = 0;
    double max = data[i];
    for (i = 1; i < n; i++) {
        if ( data[i] > max ) {
            max = data[i];
            *time = i;
        }
    }
    return max;
}

int main(void)
{
    double data[MAX_DATA] = {0};
    int n = readDoubles(MAX_DATA, data);
    smoothData(n, data);

    char flag = 0;
    for (int i = 0; i < n; i++) {
        if (data[i] > CRITICAL_HIGH && flag == 0) {
            printf("Acceleration of %.2lf m/sec^2 exceeded at t = %.2lf secs.\n", CRITICAL_HIGH, i/100.0);
            flag = 1;
        } else if (data[i] < CRITICAL_LOW && flag == 1) {
            flag = 0;
        }
    }
    printf("\n");
    int time = 0;
    double maxVal = max(n, data, &time);
    printf("Maximum acceleration: %.2lf m/sec^2 at t = %.2lf secs.\n", maxVal, time/100.0);
}
