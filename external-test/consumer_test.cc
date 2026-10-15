#include "message.hpp"
#include "DataView.hpp"

#ifndef DJINNI_CONSUMER_TEST
#error "The generated library must propagate its native defines to consumers"
#endif

int main() {
    consumer::Message message(consumer::Shared(consumer::Status::READY, 42));
    const uint8_t bytes[] = {42};
    djinni::DataView view(bytes, sizeof(bytes));
    return message.payload.status == consumer::Status::READY &&
                   message.payload.value == 42 && view.len() == 1 && view.buf()[0] == 42
               ? 0
               : 1;
}
