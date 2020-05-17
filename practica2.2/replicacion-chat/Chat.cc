#include "Chat.h"
#include <vector>

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    memset(_data, 0, MESSAGE_SIZE);

    memcpy(_data, static_cast<void *>(&type), sizeof(uint8_t));
    _data += sizeof(uint8_t);

    memcpy(_data, static_cast<void *>((char*)nick.c_str()), nick.length());
    _data += sizeof(char) * 8;

    memcpy(_data, static_cast<void *>((char*)message.c_str()), message.length());
}

int ChatMessage::from_bin(char * bobj)
{

    memcpy(static_cast<void *>(&type), bobj, sizeof(uint8_t));
    bobj += sizeof(uint8_t);

    char _nick [8];
    memcpy(static_cast<void *>(&_nick), bobj, 8);
    nick = _nick; 
    bobj += 8 * sizeof(char);
    
    char _message [80];
    memcpy(static_cast<void *>(&_message), bobj, 80);
    message = _message; 
    bobj += 80 * sizeof(char);

    return 0;
}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

void ChatServer::do_messages()
{
    while (true)
    {
        //Recibir Mensajes en y en función del tipo de mensaje
        // - LOGIN: Añadir al vector clients
        // - LOGOUT: Eliminar del vector clients
        // - MESSAGE: Reenviar el mensaje a todos los clientes (menos el emisor)
        Socket* client_Socket;
        ChatMessage message_Client;

        int err = socket.recv(message_Client, client_Socket);
        if(err != -1)
        {
            switch (message_Client.type)
            {
                case ChatMessage::LOGIN:
                {
                    std::vector<Socket*>::iterator it = clients.begin();
                    while(it != clients.end() && (*it) != client_Socket)
                    {
                        it++;
                    }
                    if(it != clients.end())
                    {
                        std::cout << "LOGIN FAIL: Client " << message_Client.nick <<  "is already logged\n";
                    }
                    {
                        clients.push_back(client_Socket);
                        std::cout << "New client " << message_Client.nick <<  " logged succesfully\n";
                    }
                    break;
                }   
                case ChatMessage::LOGOUT:
                {
                    std::vector<Socket*>::iterator it = clients.begin();
                    while(it != clients.end() && (*it) != client_Socket)
                    {
                        it++;
                    }
                    if(it != clients.end())
                    {
                        clients.erase(it);
                        std::cout << "Client " << message_Client.nick <<  " logged out succesfully\n";
                    }
                    else
                    {
                        std::cout << "LOGOUT FAIL: Client " << message_Client.nick <<  " is not logged \n";
                    }
                    break;
                }   

                case ChatMessage::MESSAGE:
                {
                    std::vector<Socket*>::iterator it = clients.begin();
                    while(it != clients.end() && (*it) != client_Socket)
                    {
                        it++;
                    }
                    if(it != clients.end())
                    {
                        for(std::vector<Socket*>::iterator it = clients.begin(); it != clients.end(); it++)
                        {
                            if((*it) != client_Socket) socket.send(message_Client, *(*it));
                        }
                        std::cout << "Client " << message_Client.nick <<  " says "<< message_Client.message <<" \n";
                    }
                    else
                    {
                        std::cout << "MESSAGE FAIL: Client " << message_Client.nick <<  " is not logged \n";
                    }
                    break;
                }  

                default:
                std::cout << "Default \n";
                    break;
            }   
        }       
    }
}

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;
    std:: cout << "Type: " << ChatMessage::LOGIN << '\n',

    std::cout << "Logging: sending: " << em.type << " to: " << socket << '\n';
    int err = socket.send(em, socket);
    if (err == -1) {
        std::cout << "login send error \n";
    }
    
}

void ChatClient::logout()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGOUT;

    socket.send(em, socket);
}

void ChatClient::input_thread()
{
    bool inChat = true;
    while (inChat)
    {
        std::cout << "inChat \n";
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
        std::string msg;
        std::getline(std::cin, msg);

        if(msg == "exit" || msg == "logout")
        {
            logout();
            inChat = false;
        } 
        else
        {
            ChatMessage em(nick, msg);
            em.type = ChatMessage::MESSAGE;

            socket.send(em, socket);
        }
    }
}

void ChatClient::net_thread()
{
    while(true)
    {
        //Recibir Mensajes de red
        //Mostrar en pantalla el mensaje de la forma "nick: mensaje"
        Socket* server_Socket;
        ChatMessage message_Server;
        int tmp = socket.recv(message_Server, server_Socket);
        if(tmp != -1)
        {
            std::cout << message_Server.nick << ": " << message_Server.message << "\n";
        }
    }
}

