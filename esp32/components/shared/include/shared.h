#include <stdbool.h>

typedef struct
{
    char callsign[32];
    char operator[32];
    char aircraft_type[32];
    char origin[64];
    char destination[64];
    char area[8];
    bool widebody;
} flight_t;
