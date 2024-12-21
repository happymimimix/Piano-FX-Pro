#pragma once
#include <PFXSTUDIO_Global_Imports.h>

template<typename... Pointers>
bool AllNullPtr(Pointers... PTRs) {
    bool result = true;
    for (auto ptr : { PTRs... }) {
        if (ptr != nullptr) {
            result = false;
            break;
        }
    }
    return result;
}

template<typename... Pointers>
bool AnyNullPtr(Pointers... PTRs) {
    bool result = false;
    for (auto ptr : { PTRs... }) {
        if (ptr == nullptr) {
            result = true;
            break;
        }
    }
    return result;
}

template<typename... Pointers>
bool AllGoodPtr(Pointers... PTRs) {
    bool result = true;
    for (auto ptr : { PTRs... }) {
        if (ptr == nullptr) {
            result = false;
            break;
        }
    }
    return result;
}

template<typename... Pointers>
bool AnyGoodPtr(Pointers... PTRs) {
    bool result = false;
    for (auto ptr : { PTRs... }) {
        if (ptr != nullptr) {
            result = true;
            break;
        }
    }
    return result;
}
