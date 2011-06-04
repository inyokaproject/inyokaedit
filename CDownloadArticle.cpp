/****************************************************************************
* Code from Martin Kompf: http://cplus.kompf.de/artikel/httpget.html
* Quotation from author: "Sie können den Code frei verwenden."
****************************************************************************/

/***************************************************************************
* File Name:  CDownloadArticle.cpp
* Purpose:    Downloading raw text of an existing inyoka wiki article
***************************************************************************/

#include "CDownloadArticle.h"
#include <stdio.h>  // sprintf()

// Constructor
CDownloadArticle::CDownloadArticle()
{
}

// -----------------------------------------------------------------------------------------------

int CDownloadArticle::StartDownload(std::string url, std::string sitename){

    sTmpOutput.clear();

    // Initialize TCP for Windows (winsock)
#ifdef _WIN32
    short wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD (1, 1);
    if (WSAStartup (wVersionRequested, &wsaData) != 0) {
        sTmpOutput = "Failed to init windows sockets.\n";
        return -1;
    }
#endif


    // Create socket
    sock = socket( PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        sTmpOutput = "Failed to create socket.";
        return -2;
    }

    // Create socket address of server (type, ip address and port number)
    memset( &server, 0, sizeof (server));
    // If url is a numeric ip address
    if ((addr = inet_addr( url.c_str())) != INADDR_NONE) {
        memcpy( (char *)&server.sin_addr, &addr, sizeof(addr));
    }
    // Convert server name to ip address
    else {
        host_info = gethostbyname( url.c_str());
        if (NULL == host_info) {
            sTmpOutput = "Unknown server: " + url + "\n";
            return -3;
        }
        memcpy( (char *)&server.sin_addr, host_info->h_addr, host_info->h_length);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons( HTTP_PORT);

    // Create connection to server
    if ( connect( sock, (struct sockaddr*)&server, sizeof( server)) < 0) {
        sTmpOutput = "Can't connect to server.";
        return -4;
    }

    // Create and send http GET request
    sprintf (buffer, "GET /%s HTTP/1.1\r\nHost: %s\r\n\r\n", sitename.c_str(), url.c_str());
    send( sock, buffer, strlen( buffer), 0);

    // Recieve server response and append it output string
    do {
        count = recv( sock, buffer, sizeof(buffer), 0);
        sTmpOutput.append(buffer, count);
    }
    while (count > 0);

    // Close socket connection
    close( sock);

    return count;
}

// -----------------------------------------------------------------------------------------------

std::string CDownloadArticle::getRawText(){
    return sTmpOutput;
}
