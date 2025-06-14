#include <rabbitmq-c/amqp.h>
#include <rabbitmq-c/tcp_socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main() {
    printf("Programm begin\n");
    char const *hostname = "localhost";
    int port = 5672;
    int status;
    int rate_limit = 1;
    int message_count = 0;

    amqp_socket_t *socket = NULL;
    amqp_connection_state_t conn;

    conn = amqp_new_connection();

    socket = amqp_tcp_socket_new(conn);
    if (!socket) {
        printf("socket exception");
        exit(1);
    }

    status = amqp_socket_open(socket, hostname, port);
    if (status) {
        printf("socket open exception");
        exit(1);
    }

    amqp_rpc_reply_t reply = amqp_login(conn, "/", 0, 131072, 0, 
        AMQP_SASL_METHOD_PLAIN, "user", "pass");

    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        fprintf(stderr, "Login exception\n");
        exit(1);
    }

    amqp_channel_open(conn, 1);
    amqp_get_rpc_reply(conn);

    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
        fprintf(stderr, "Channel creation exception\n");
        exit(1);
    }

    amqp_bytes_t queue = amqp_cstring_bytes("test_queue");
    amqp_queue_declare(conn, 1, queue, 0, 0, 0, 0, amqp_empty_table);
    for (int i = 0; i < 10; i++) {
        amqp_basic_publish(
            conn, 
            1, 
            amqp_literal_bytes(""),
            queue,
            0, 
            0, 
            NULL, 
            amqp_cstring_bytes("test message")
        );
        printf("Sended %d message\n", i);
        // sleep(1);
    }
    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);
    return 0;
}