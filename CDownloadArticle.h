/****************************************************************************
*
*   Copyright (C) 2011 by the respective authors (see AUTHORS)
*
*   This file is part of InyokaEdit.
*
*   InyokaEdit is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   InyokaEdit is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with InyokaEdit.  If not, see <http://www.gnu.org/licenses/>.
*
****************************************************************************/

/***************************************************************************
* File Name:  CDownloadArticle.h
* Purpose:    Class definition
***************************************************************************/

#ifndef CDOWNLOADARTICLE_H
#define CDOWNLOADARTICLE_H

#include <string>

// Headers for windows
#ifdef _WIN32
#include <winsock.h>
#include <io.h>
// Headers for Unix/Linux
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#endif

#define HTTP_PORT 80

class CDownloadArticle
{
public:
    CDownloadArticle();  // Constructor
    int StartDownload(std::string url, std::string sitename);  // Start download
    std::string getRawText();  // Give out downloaded text

private:
    struct sockaddr_in server;
    struct hostent *host_info;
    unsigned long addr;
    int sock;
    char buffer[8192];
    int count;
    std::string sTmpOutput;
};

#endif // CDOWNLOADARTICLE_H
