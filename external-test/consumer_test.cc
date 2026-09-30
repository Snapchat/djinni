#include "message.hpp"

#ifndef DJINNI_CONSUMER_TEST
#error "The generated library must propagate its native defines to consumers"
#endif

int main() {
    consumer::Message message(consumer::Shared(consumer::Status::READY, 42));
    return message.payload.status == consumer::Status::READY &&
                   message.payload.value == 42
               ? 0
               : 1;
}
