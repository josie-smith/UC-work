/* Herbert the Heffalump climbs scree slopes in a series of rushes.
 * Each rush gains him a certain amount of height, rushHeight, but then,
 * unless he is already at the top, he slips back down again by an
 * amount slideBack. This program helps Herbert determine how many
 * rushes it will take him to get to the top of a scree slope of a
 * given height.
 */
#include <stdio.h>

int main(void)
{
    float screeHeight = 0;
    float rushHeight = 0;
    float slideBack = 0;
    scanf("%f %f %f", &screeHeight, &rushHeight, &slideBack);

    float totalHeight = 0;
    int tries = 0;
    while ( totalHeight < screeHeight) {
        if (totalHeight > 0) {
            totalHeight -= slideBack;
        }
        totalHeight += rushHeight;
        tries++;
    }
    printf("%d", tries);
}
