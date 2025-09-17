#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char **argv)
{
    if (argc < 3) {
        printf("Usage: <folder> <csv file>");
        return 1;
    }




    int roll = -1;
    int pitch = -1;
    int num;

    for (int i = 3; i < argc; i++) {
        if (strcmp(argv[i], "-roll") == 0 || strcmp(argv[i], "-r") == 0) {
            if (!(num = atoi(argv[i + 1]))) {
                fprintf(stderr, "Missing value after %s\n", argv[i]);
                //return 1;
            }
            else {
                roll = atoi(argv[i + 1]);
                i++;
            }

        }
        else if (strcmp(argv[i], "-pitch") == 0 || strcmp(argv[i], "-p") == 0) {
            if (!(num = atoi(argv[i + 1]))) {
                fprintf(stderr, "Missing value after %s\n", argv[i]);
                //return 1;

            }
            else {
                pitch = atoi(argv[i + 1]);
                i++;
            }

        }


        else {
            printf("Unknown option: %s\n", argv[i]);
        }
    }

    printf("Roll = %d\n", roll);
    printf("Pitch = %d\n", pitch);


    return 0;
}
