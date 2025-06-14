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
    
    char const *queuename = "test_queue";

    amqp_basic_consume(conn, 1, amqp_cstring_bytes(queuename), amqp_empty_bytes,
                     0, 0, 0, amqp_empty_table);

    amqp_rpc_reply_t res;
    amqp_envelope_t envelope;

    
    amqp_maybe_release_buffers(conn);

    res = amqp_consume_message(conn, &envelope, NULL, 0);

    if (AMQP_RESPONSE_NORMAL != res.reply_type) {
      return 1;
    }

    printf("Delivery %u, exchange %.*s routingkey %.*s\n",
           (unsigned)envelope.delivery_tag, (int)envelope.exchange.len,
           (char *)envelope.exchange.bytes, (int)envelope.routing_key.len,
           (char *)envelope.routing_key.bytes);

    if (envelope.message.properties._flags & AMQP_BASIC_CONTENT_TYPE_FLAG) {
      printf("Content-type: %.*s\n",
             (int)envelope.message.properties.content_type.len,
             (char *)envelope.message.properties.content_type.bytes);
    }
    printf("----\n");

    printf("message: %.*s\n",
        (int)envelope.message.body.len,
        (char *)envelope.message.body.bytes);

    // обязательно подтверждать или указывать в amqp_basic_consume получение без подтверждения
    amqp_basic_ack(conn, envelope.channel, envelope.delivery_tag, 0); 

    amqp_destroy_envelope(&envelope);

    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);
    return 0;
}