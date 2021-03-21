#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    while (1)
    {
        printf("1 - Mudar TR\n");
        printf("2 - Retornar a TR do potenciometro\n");
        int opcao;
        double tempRef;
        scanf("%d", &opcao);
        if (opcao == 1)
        {
            printf("Enviar uma nova TR: ");
            scanf("%lf", &tempRef);
        }
        else if (opcao == 2)
        {
            printf("ret\n");
        }
        usleep(1000000);
        system("clear");
    }
    return 0;
}