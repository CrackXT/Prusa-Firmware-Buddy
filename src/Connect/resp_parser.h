#pragma once

#include <automata/core.h>

namespace con::parser {

class ResponseParser final : public automata::Execution {
private:
    virtual automata::ExecutionControl event(automata::Event event) override;

public:
    uint16_t status_code = 0;
    std::optional<size_t> content_length;
    bool done = false;
    ResponseParser();
};

}
