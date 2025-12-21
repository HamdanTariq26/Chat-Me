#ifndef TYPES_H
#define TYPES_H
#include "olc_net.h"

enum class MessageType : uint32_t {
    login,
    loginSuccessful,
    loginUnSuccessful,
    registeration,
    usernameTaken,
    registerationSuccessful,
    validationSuccessful,
    validationUnsuccessful,
    message,
    messageAll,
    newClient,
    removeClient,
    beat,
    loginDuplicate,
    fileInfo,
    fileChunk,
    fileComplete
};

#endif // TYPES_H
