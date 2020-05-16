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
        strncpy(name, _n, 80);
    };

    virtual ~Jugador(){};

    void to_bin()
    {
        int32_t total = 80 * sizeof(char) + 2 * sizeof(int16_t);
        
        alloc_data(total);

        char * tmp = _data;

        memcpy(tmp, name, 80);
        tmp += 80 * sizeof(char);

        memcpy(tmp, &x, sizeof(int16_t));
        tmp += sizeof(int16_t);

        memcpy(tmp, &y, sizeof(int16_t));
    }

    int from_bin(char * data)
    {
        char * tmp = data;

        memcpy(name, tmp, 80);
        tmp += 80 * sizeof(char);

        memcpy(&x, tmp, sizeof(int16_t));
        tmp += sizeof(int16_t);

        memcpy(&y, tmp, sizeof(int16_t));

        return sizeof(data);
    }


public:
    char name[80];

    int16_t x;
    int16_t y;
};

int main(int argc, char **argv)
{
    Jugador one_r("", 0, 0);
    Jugador one_w("Player_ONE", 123, 987);

    //Serializar y escribir one_w en un fichero
    int usedTxt = open("data.txt", O_CREAT|O_WRONLY, 0600);
    if(usedTxt == -1)
    {
        std::cerr << "Error en la apertura \n";
    }
    one_w.to_bin();
    int bytes = write(usedTxt, one_w.data(), one_w.size());
    close(usedTxt);
    if(bytes == -1)
    {
        std::cerr << "Error escritura: " << strerror(errno) << '\n';
    }
    else
    {
        //Leer el fichero en un buffer y "deserializar" en one_r
        char buffer [one_w.size()];
        memset(&buffer, 0 ,sizeof(buffer));
        int readed_file = open("data.txt", O_RDONLY);
        int bytes = read(readed_file, buffer, one_w.size());
        close(readed_file);
        if(bytes == -1)
        {
           std::cerr << "Error lectura: " << strerror(errno) << '\n'; 
        }
        else
        {
            one_r.from_bin(buffer);
            //Mostrar el contenido one_r
            std::cout << one_r.name << std::endl;
            std::cout << one_r.x << std::endl;
            std::cout << one_r.y << std::endl;
        }    
    }
    return 0;
}
