/*
   © 2025 Hamdan Tariq

   LAN Chat App with File Transfer

   Licensed under the Apache License, Version 2.0.
   You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0

   Distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND.
   
   File: types.h
   Description:  
   */
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

