#include "shared/shared.hpp"

int main() {
    using shared::defaults::Shared;
    using shared::defaults::Status;
    Shared first{static_cast<Status>(0), 7};
    Shared second{static_cast<Status>(0), 7};
    return first == second ? 0 : 1;
}
