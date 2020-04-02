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

#ifndef UUID_HPP
#define UUID_HPP

#include <string>
#include <uuid/uuid.h>

class UUID {
    public:
        UUID();
        UUID(const uuid_t uuid);
        UUID(const std::string uuid);
        UUID(const UUID &obj);

        UUID& operator= (const UUID &rhs);
        bool  operator()(const UUID &left, const UUID &right) const;
        bool  operator!=(const UUID &rhs) const;
        bool  operator==(const UUID &rhs) const;
        bool  operator> (const UUID &rhs) const;
        bool  operator< (const UUID &rhs) const;
        bool  operator>=(const UUID &rhs) const;
        bool  operator<=(const UUID &rhs) const;

        void setUUID(const uuid_t uuid);
        void setUUID(const std::string &uuid);
        void setUUID(const UUID &uuid);

        std::string unparseLower();
        std::string unparseUpper();

        bool isNull();
        void clear();
        void copy(uuid_t dst);

        static const size_t UUID_STRLEN = 36;
        static const size_t UUID_SIZE = sizeof(uuid_t);

    private:
        uuid_t uuid;
        std::string ucUUID;
        std::string lcUUID;
};

#endif // UUID_HPP
