#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <netinet/in.h>


void print_socket_err(int err) {
    printf("ERROR: ");
    switch (err) {
        case EACCES:
            printf("Permission to create a socket of the specified type and/or protocol is denied.\n");
            break;
        case EAFNOSUPPORT:
            printf("The implementation does not support the specified address family.\n");
            break;
        case EINVAL:
            printf("Unknown protocol, or protocol family not available.\n");
            break;
        case EMFILE:
            printf("The per-process limit on the number of open files has been reached.\n");
            break;
        case ENFILE:
            printf("The system-wide limit on the total number of open files has been reached.\n");
            break;
        case ENOBUFS:
        case ENOMEM:
            printf("Insufficient memory is available. The socket cannot be created until sufficient resources are freed.\n");
            break;
        case EPROTONOSUPPORT:
            printf("The protocol type or the specified protocol is not supported within this domain.\n");
            break;
        default:
            break;
    }
}

void print_bind_err(int err) {
    switch (errno) {
        case EROFS:
            printf("EROFS: The socket inode would resied on a read-only filesystem.\n");
            break;
        case ENOMEM:
            printf("ENOMEM: Insufficient kernel memory was available.\n");
            break;
        case ENOENT:
            printf("ENOENT: A component in the directory prefix of the socket pathname does not exist.\n");
            break;
        case ENOTDIR:
            printf("ENOTDIR: A component of the path prefix is not a directory.\n");
            break;
        case ENAMETOOLONG:
            printf("ENAMETOOLONG: 'addr' is too long.\n");
            break;
        case ELOOP:
            printf("ELOOP: Too many symbolic links were encountered while resolving 'addr'.\n");
            break;
        case EFAULT:
            printf("EFAULT: 'addr' points outside the user's accessible address space.\n");
            break;
        case EADDRNOTAVAIL:
            printf("EADDRNOTAVAIL: A nonexistent interface was requested or the requested address was not local.\n");
            break;
        case EACCES:
            printf("EACCES: Search permission is denied on a component of the path prefix.\n");
            break;
        case ENOTSOCK:
            printf("ENOTSOCK: The file descriptor sockfd does not refer to a socket.\n");
            break;
        case EINVAL:
            printf("EINVAL: addrlen is wrong, or addr is not a valid address for this socket's domain.\n");
            break;
        case EBADF:
            printf("EBADF: sockfd is not a valid file descriptor.\n");
            break;
        case EADDRINUSE:
            printf("EADDRINUSE: The given address is already in use.\n");
            break;
        default:
            break;
    }
}

void print_listen_err(int err) {
    switch (errno) {
        case EADDRINUSE:
            printf("Another socket is already listening on the same port.\n");
            break;
        case EBADF:
            printf("The argument sockfd is not a valid file descriptor.\n");
            break;
        case ENOTSOCK:
            printf("The file descriptor sockfd does not refer to a socket.\n");
            break;
        case EOPNOTSUPP:
            printf("The socket is not of a type that supports the listen() operation.\n");
            break;
        default:
            break;
    }
}

void print_recv_err(int err) {
    switch (err) {
        case EAGAIN:
            printf("The socket is marked non-blocking, and the receive operation would block, or a receive timeout had been set, and the timeout expired before data were received.\n");
            break;
        case EBADF:
            printf("The argument socket is an invalid descriptor.\n");
            break;
        case ECONNRESET:
            printf("The connection is closed by the peer during the receive attempt on a socket.\n");
            break;
        case EFAULT:
            printf("The receive buffer pointer(s) point outside the process's address space.\n");
            break;
        case EINTR:
            printf("The receive was interrupted by delivery of a signal before any data were available.\n");
            break;
        case EINVAL:
            printf("MSG_OOB is set, but no out-of-band data is available.\n");
            break;
        case ENOBUFS:
            printf("An attempt to allocate memory buffer fails.\n");
            break;
        case ENOTCONN:
            printf("The socket is associated with a connection oriented protocol and has not been connected.\n");
            break;
        case ENOTSOCK:
            printf("The argument socket does not refer to a socket.\n");
            break;
        case EOPNOTSUPP:
            printf("The type and/or protocol of socket do not support option(s) specified in flags.\n");
            break;
        case ETIMEDOUT:
            printf("The connection timed out.\n");
            break;
        case EMSGSIZE:
            printf("The msg_iovlen member of the msghdr structure pointed to by messages is less than or equal to 0, or is greater than IOV_MAX.\n");
            break;
        case ENOMEM:
            printf("Insufficient memory is available.\n");
        default:
            break;
    }
}

/*
 * A simple web server in C.
 */
int main() {
    errno = 0;
    int32_t sfd = socket(AF_INET, SOCK_STREAM, 0);

    if (errno != 0) {
        print_socket_err(errno);
        return 1;
    }

    /* bind socket to 127.0.0.1:4080 */
    struct sockaddr_in address = {
        .sin_family = AF_INET,
        .sin_port = 5080,
        .sin_addr = INADDR_ANY,
    };

    errno = 0;
    uint32_t result = bind(sfd, (struct sockaddr *)&address, sizeof(address));

    if (errno != 0) {
        print_bind_err(errno);
        return 1;
    }

    errno = 0;
    if (listen(sfd, 50) == -1) {
        print_listen_err(errno);
    }

    struct sockaddr_in peer_address = (struct sockaddr_in){ 0 };
    socklen_t peer_address_size = sizeof(peer_address);

    printf("Waiting for connection...\n");
    int32_t cfd;
    uint8_t buffer[2048] = { 0 };
    while (1) {
        cfd = accept(sfd, (struct sockaddr*)&peer_address, &peer_address_size);
        printf("Connection received!\n");
        while (1) {
            errno = 0;
            if (recvfrom(cfd, buffer, sizeof(buffer), MSG_PEEK, NULL, NULL)) {
                print_recv_err(errno);
                break;
            }
            printf("%s", buffer);
            // wipe string buffer
            memset(buffer, '\0', sizeof(buffer));
        }
    }

    printf("Closing socket...\n");
    close(sfd);

    return 0;
}
