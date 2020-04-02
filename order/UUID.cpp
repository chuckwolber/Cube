/**
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2020 Chuck Wolber
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include "UUID.hpp"

UUID::UUID() {
    clear();
    uuid_generate_random(uuid);
}

UUID::UUID(const uuid_t uuid) {
    setUUID(uuid);
}

UUID::UUID(const std::string uuid) {
    setUUID(uuid);
}

UUID::UUID(const UUID &uuid) {
    uuid_copy(this->uuid, uuid.uuid);
}

UUID& UUID::operator=(const UUID &rhs) {
    if (this != &rhs)
        uuid_copy(uuid, rhs.uuid);
    return *this;
}

bool UUID::operator()(const UUID &left, const UUID &right) const {
    return uuid_compare(left.uuid, right.uuid) == -1;
}

bool UUID::operator!=(const UUID &rhs) const {
    return !(*this == rhs);
}

bool UUID::operator==(const UUID &rhs) const {
    return uuid_compare(this->uuid, rhs.uuid) == 0;
}

bool UUID::operator>(const UUID &rhs) const {
    return uuid_compare(this->uuid, rhs.uuid) == 1;
}

bool UUID::operator<(const UUID &rhs) const {
    return uuid_compare(this->uuid, rhs.uuid) == -1;
}

bool UUID::operator>=(const UUID &rhs) const {
    return uuid_compare(this->uuid, rhs.uuid) >= 0;
}

bool UUID::operator<=(const UUID &rhs) const {
    return uuid_compare(this->uuid, rhs.uuid) <= 0;
}

void UUID::setUUID(const uuid_t uuid) {
    clear();
    uuid_copy(this->uuid, uuid);
}

void UUID::setUUID(const std::string &uuid) {
    clear();
    if (uuid.length() == UUID_STRLEN)
        uuid_parse(uuid.c_str(), this->uuid);
}

void UUID::setUUID(const UUID &uuid) {
    clear();
    uuid_copy(this->uuid, uuid.uuid);
}

std::string UUID::unparseLower() {
    if (lcUUID == "") {
        char lc[UUID_STRLEN + 1];
        uuid_unparse_lower(uuid, lc);
        lcUUID = std::string(lc);
    }
    return lcUUID;
}

std::string UUID::unparseUpper() {
    if (ucUUID == "") {
        char uc[UUID_STRLEN + 1];
        uuid_unparse_upper(uuid, uc);
        ucUUID = std::string(uc);
    }
    return ucUUID;
}

bool UUID::isNull() {
    return uuid_is_null(uuid);
}

void UUID::clear() {
    ucUUID = "";
    lcUUID = "";
    uuid_clear(uuid);
}

void UUID::copy(uuid_t dst) {
    uuid_copy(dst, uuid);
}
