#include "Chat.h"

void ChatMessage::to_bin()
{
    alloc_data(MESSAGE_SIZE);

    //Serializar los campos type, nick y message en el buffer _data
    //int32_t total = MESSAGE_SIZE + 2 * sizeof(int16_t);
        
    //alloc_data(total);

    char * tmp = _data;

    memset(tmp, 0, MESSAGE_SIZE);

    memcpy(tmp, static_cast<void *>(type), sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    memcpy(tmp, nick.c_str(), nick.length());
    tmp += sizeof(char) * 8;

    memcpy(tmp, message.c_str(), message.length());
}

int ChatMessage::from_bin(char * bobj)
{
    /*
    alloc_data(MESSAGE_SIZE);

    memcpy(static_cast<void *>(_data), bobj, MESSAGE_SIZE);*/

    //Reconstruir la clase usando el buffer _data
    char * tmp = bobj;

    memcpy(static_cast<void *>(type), tmp, sizeof(uint8_t));
    tmp += sizeof(uint8_t);

    char[8] _nick;
    memcpy(_nick, tmp, 8);
    nick = convertToString(_nick, _nick.length()); 
    tmp += 8 * sizeof(char);
    
    char[80] _messages;
    memcpy(_messages, tmp, 80);
    nick = convertToString(_messages, _messages.length()); 
    tmp += 80 * sizeof(char);

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
        socket.recv(message_Client, client_Socket);
        switch (message_Client.type)
        {
        case MessageType::LOGIN:
            auto it = clients.begin();
            while(it != clients.end() && (*it) != client_Socket)
            {
                it++;
            }
            if(it != client.end())
            {
                std::cout << "LOGIN FAIL: Client " << message_Client.nick <<  "is already logged\n";
            }
            {
                clients.push_back(client_Socket);
                std::cout << "New client " << message_Client.nick <<  " logged succesfully\n";
            }
            break;
        case MessageType::LOGOUT:
            auto it = clients.begin();
            while(it != clients.end() && (*it) != client_Socket)
            {
                it++;
            }
            if(it != client.end())
            {
                clients.erase(it);
                std::cout << "Client " << message_Client.nick <<  " logged out succesfully\n";
            }
            else
            {
                std::cout << "LOGOUT FAIL: Client " << message_Client.nick <<  " is not logged \n";
            }
            
            break;
        case MessageType::MESSAGE:
            
            auto it = clients.begin();
            while(it != clients.end() && (*it) != client_Socket)
            {
                it++;
            }
            if(it != client.end())
            {
                for(auto it = clients.begin(); it != clients.end(); it++)
                {
                    if((*it) != client_Socket) socket.send(message_Client, *(*it));
                }
                std::cout << "Client " << message_Client.nick <<  " says "<< message_Client.message <<" \n"
            }
            else
            {
                std::cout << "MESSAGE FAIL: Client " << message_Client.nick <<  " is not logged \n"
            }
            
            break;
        default:
            break;
        }
    }
}

void ChatClient::login()
{
    std::string msg;

    ChatMessage em(nick, msg);
    em.type = ChatMessage::LOGIN;

    socket.send(em, socket);
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
    while (intChat)
    {
        // Leer stdin con std::getline
        // Enviar al servidor usando socket
        std::string msg;

        msg = std::getline();

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

