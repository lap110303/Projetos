/********************
 * passa_tempo.c - v3
 ********************/

#include <stdio.h>
#include <time.h>

struct timespec inicio = {0,0};

void inicia_tempo(void)
{
    clock_gettime(CLOCK_REALTIME,&inicio);
}

void passa_tempo(int tid, int gid, int pos, int decimos)
{
    struct timespec zzz, agora;
    int tstamp;

    if ((inicio.tv_sec == 0)&&(inicio.tv_nsec == 0)) {
        clock_gettime(CLOCK_REALTIME,&inicio);
    }

    zzz.tv_sec  = decimos/10;
    zzz.tv_nsec = (decimos%10) * 100L * 1000000L;

    clock_gettime(CLOCK_REALTIME,&agora);
    tstamp = ( 10 * agora.tv_sec  +  agora.tv_nsec / 100000000L )
            -( 10 * inicio.tv_sec + inicio.tv_nsec / 100000000L );

    printf("%3d [ %2d.%.2d @%.2d z%3d\n",tstamp,tid,gid,pos,decimos);

    nanosleep(&zzz,NULL);

    clock_gettime(CLOCK_REALTIME,&agora);
    tstamp = ( 10 * agora.tv_sec  +  agora.tv_nsec / 100000000L )
            -( 10 * inicio.tv_sec + inicio.tv_nsec / 100000000L );

    printf("%3d ) %2d.%.2d @%.2d\n",tstamp,tid,gid,pos);
}


/************** Use o main a seguir para conferir o formato final.
main()
{
    passa_tempo( 1, 2, 3, 4 );
    passa_tempo(99,88,77,66 );
}
*/