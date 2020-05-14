#include "Serializable.h"

#include <iostream>
#include <string>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

class Jugador: public Serializable
{
public:
    Jugador(const char * _n, int16_t _x, int16_t _y):x(_x),y(_y)
    {
        strncpy(name, _n, MAX_NAME);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t total = MAX_NAME * sizeof(char) + 2 * sizeof(int16_t);
        alloc_data(total);

        char * aux = _data;

        memcpy(aux, name, MAX_NAME);
        aux += MAX_NAME * sizeof(char);

        memcpy(aux, &x, sizeof(int16_t));
        aux += sizeof(int16_t);

        memcpy(aux, &y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        char * aux = data;

        memcpy(name, aux, 80);
        aux += 80 * sizeof(char);

        memcpy(&x, aux, sizeof(int16_t));
        aux += sizeof(int16_t);

        memcpy(&y, aux, sizeof(int16_t));
        return 0;
    }

private:

    static const size_t MAX_NAME = 80;
    char name[MAX_NAME];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_a("Player_ONE", 123, 987);
    Jugador one_b("Payer_TWO", 456, 654);

    int file = open("data", O_CREAT|O_TRUNC);
    if (file == -1) {
        std::cout << "Error while opening file. \n";
    }

    one_a.to_bin();
    
    
    write(file, one_a.data(), one_a.size());
    std::cout << "Escritura finalizada.. \n";

    //Serializar y escribir one_w en un fichero
    //Leer el fichero en un buffer y "deserializar" en one_r
    //Mostrar el contenido one_r
    return 0;
}
