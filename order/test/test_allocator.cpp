/**
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2019 Chuck Wolber
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

#include <cassert>
#include <iostream>
#include <vector>

#include "../Allocator.hpp"
#include "../ServerConfig.hpp"
#include "../UUID.hpp"

#include "../../Algorithm.h"
#include "../../Cube.h"

const unsigned int session_size = 1000;

Allocator* test_init();
void test_disabled(Allocator* al);
void test_new_session(Allocator* al);
void test_active_session_retrieval(Allocator* al);
void test_end_session(Allocator* al);
void test_expire_active_session(Allocator* al);
void test_expire_all_active_sessions(Allocator* al);
void display_user_sessions(Allocator* al);

Session getSession(std::string session);
bool verify_session(Session st_new, Session st_old);
bool verify_user_sessions(Allocator* al, uint32_t v1, uint32_t v5, uint32_t v8);

int main() {
    ServerConfig *sc = new ServerConfig("build/cube_order_data/server_config.json");
    Allocator* al = new Allocator(sc);
    al->enable();

    verify_user_sessions(al, 4, 3, 2);
    test_disabled(al);
    test_new_session(al);

    verify_user_sessions(al, 10, 3, 2);
    test_active_session_retrieval(al);

    verify_user_sessions(al, 10, 3, 2);
    test_end_session(al);

    verify_user_sessions(al, 8, 3, 2);
    test_expire_active_session(al);

    verify_user_sessions(al, 8, 2, 1);
    test_expire_all_active_sessions(al);
    verify_user_sessions(al, 0, 0, 0);

    free(al);
    free(sc);
    return 0;
}

void test_disabled(Allocator* al) {
    std::cout << "Testing disabled... ";

    UUID u1("28255F79-ADFA-4D24-9091-6293ED050FB4");
    UUID sess("15A32617-C9FB-4966-95CD-EED45CA5C781");

    Session ses;
    verify_user_sessions(al, 4, 3, 2);
    al->disable();
    assert(!al->getNewSession(u1, ses));
    assert(!al->getActiveSession(sess, ses));
    al->enable();
    verify_user_sessions(al, 4, 3, 2);

    std::cout << "Passed" << std::endl;
}

void test_new_session(Allocator* al) {
    std::cout << "Testing new session... ";
    UUID u1("28255F79-ADFA-4D24-9091-6293ED050FB4");
    Session ses;

    verify_user_sessions(al, 4, 3, 2);

    /* The effect is the reactivate expired sessions. */
    assert(al->getNewSession(u1, ses));
    assert(ses.getUser() == u1.unparseUpper());
    verify_user_sessions(al, 5, 3, 2);

    assert(al->getNewSession(u1, ses));
    assert(ses.getUser() == u1.unparseUpper());
    verify_user_sessions(al, 6, 3, 2);

    assert(al->getNewSession(u1, ses));
    assert(ses.getUser() == u1.unparseUpper());
    verify_user_sessions(al, 7, 3, 2);
;
    /* These are the first actual new session. */
    assert(al->getNewSession(u1, ses));
    assert(ses.getAlgorithm() == "B F' U B");
    assert(ses.getUser() == u1.unparseUpper());
    verify_user_sessions(al, 8, 3, 2);

    assert(al->getNewSession(u1, ses));
    assert(ses.getAlgorithm() == "B L U U");
    assert(ses.getUser() == u1.unparseUpper());
    verify_user_sessions(al, 9, 3, 2);

    assert(al->getNewSession(u1, ses));
    assert(ses.getAlgorithm() == "B' U' F' D");
    assert(ses.getUser() == u1.unparseUpper());
    verify_user_sessions(al, 10, 3, 2);

    std::cout << "Passed" << std::endl;
}

void test_active_session_retrieval(Allocator* al) {
    std::cout << "Testing active session retrieval... ";

    std::string u1("28255F79-ADFA-4D24-9091-6293ED050FB4");
    std::string s1("15A32617-C9FB-4966-95CD-EED45CA5C781");
    std::string a1("F");
    Session x1 = getSession(u1, s1, a1);

    std::string u2(u1);
    std::string s2("BAAEE8F9-8ECD-4BB1-84B0-01F0716946E1");
    std::string a2("F' B' D'");
    Session x2 = getSession(u2, s2, a2);

    std::string u3(u1);
    std::string s3("83588A40-B0D4-4169-A1EB-4F350065F661");
    std::string a3("L B L'");
    Session x3 = getSession(u3, s3, a3);

    std::string u4(u1);
    std::string s4("7F182C55-D9AF-479B-921A-4C18C5FC5EC1");
    std::string a4("F U' L' D'");
    Session x4 = getSession(u4, s4, a4);

    std::string u5("65C8C46C-2E0E-420E-96AC-38AF503234DD");
    std::string s5("75ED3EE4-6182-494B-B2E8-F6D05A975CC3");
    std::string a5("F' R' D' B");
    Session x5 = getSession(u5, s5, a5);

    std::string u6(u5);
    std::string s6("3475B123-B9B7-4B47-A9AA-CE952C6B32BB");
    std::string a6("U F D B");
    Session x6 = getSession(u6, s6, a6);

    std::string u7(u5);
    std::string s7("3E7AFCE3-68A1-4EED-8737-AEFE40482AAD");
    std::string a7("U D' R' L'");
    Session x7 = getSession(u7, s7, a7);

    std::string u9(u8);
    std::string s9("3E2D673C-69D3-458C-84A8-A69FA5224653");
    std::string a9("R U' L' D'");
    Session x9 = getSession(u9, s9, a9);

    std::string uA(u1);
    std::string sA("5163577B-579A-47BC-8D23-E93DB27BAD2E");
    std::string aA("U' F' B' R'");
    Session xA = getSession(uA, sA, aA);

    std::string uB(u1);
    std::string sB("7B353974-C12C-4E7E-8FD2-32371B3C1C08");
    std::string aB("R B L L");
    Session xB = getSession(uB, sB, aB);

    std::string uC(u1);
    std::string sC("ECFAB1DE-5502-42DB-B6D8-194641332D0F");
    std::string aC("F B L B");
    Session xC = getSession(uC, sC, aC);

    std::vector<Session> sessions = {
        x1, x2, x3, x4, x5, x6, x7, x8, x9, xA, xB, xC
    };

    Session ses;
    for (Session x : sessions) {
        UUID s_uuid(x.getSession());
        assert(al->getActiveSession(s_uuid, ses));
        assert(verify_session(ses, x));
    }

    std::cout << "Passed" << std::endl;
}

void test_end_session(Allocator* al) {
    std::cout << "Testing end session... ";

    std::string u1("28255F79-ADFA-4D24-9091-6293ED050FB4");
    std::string s1("15A32617-C9FB-4966-95CD-EED45CA5C781");
    std::string a1("F");
    Session x1 = getSession(u1, s1, a1);

    std::string u2("28255F79-ADFA-4D24-9091-6293ED050FB4");
    std::string s2("BAAEE8F9-8ECD-4BB1-84B0-01F0716946E1");
    std::string a2("F' B' D'");
    Session x2 = getSession(u2, s2, a2);

    std::vector<Session> sessions = { x1, x2 };
    Session ses;
    for (Session x : sessions) {
        UUID s_uuid(x.getSession());
        assert(al->endSession(s_uuid));
        assert(!al->getActiveSession(s_uuid, ses));
    }
    verify_user_sessions(al, 8, 3, 2);

    std::cout << "Passed" << std::endl;
}

void test_expire_active_session(Allocator* al) {
    std::cout << "Testing expire active session... ";

    std::string u5("65C8C46C-2E0E-420E-96AC-38AF503234DD");
    std::string s5("75ED3EE4-6182-494B-B2E8-F6D05A975CC3");
    std::string a5("F' R' D' B");
    Session x5 = getSession(u5, s5, a5);

    std::string u8("3BBC67A4-DE48-4C60-A73D-DBB81CEFDE37");
    std::string s8("8159AD61-B7E3-454B-B029-C72B6ED5DAFF");
    std::string a8("U' L B R");
    Session x8 = getSession(u8, s8, a8);

    std::vector<Session> sessions = { x5, x8 };

    Session ses;
    for (Session x : sessions) {
        UUID s_uuid(x.getSession());
        assert(al->expireActiveSession(s_uuid));
        assert(!al->getActiveSession(s_uuid, ses));
    }
    verify_user_sessions(al, 8, 2, 1);

    std::cout << "Passed" << std::endl;
}

void test_expire_all_active_sessions(Allocator* al) {
    std::cout << "Testing expire all active sessions... ";

    Session x3 = getSession("83588A40-B0D4-4169-A1EB-4F350065F661");
    Session x4 = getSession("7F182C55-D9AF-479B-921A-4C18C5FC5EC1");
    Session x6 = getSession("3475B123-B9B7-4B47-A9AA-CE952C6B32BB");
    Session x7 = getSession("3E7AFCE3-68A1-4EED-8737-AEFE40482AAD");
    Session x9 = getSession("3E2D673C-69D3-458C-84A8-A69FA5224653");
    Session xA = getSession("5163577B-579A-47BC-8D23-E93DB27BAD2E");
    Session xB = getSession("7B353974-C12C-4E7E-8FD2-32371B3C1C08");
    Session xC = getSession("ECFAB1DE-5502-42DB-B6D8-194641332D0F");

    assert(al->expireAllActiveSessions());
    std::vector<Session> sessions = { x3, x4, x6, x7, x9, xA, xB, xC };

    Session ses;
    for (Session x : sessions) {
        UUID s_uuid(x.getSession());
        assert(!al->getActiveSession(s_uuid, ses));
    }
    verify_user_sessions(al, 0, 0, 0);

    std::cout << "Passed" << std::endl;
}

Session getSession(std::string session) {
    Session ses;
    ses.readSession(UUID(session));
    return ses;
}

bool verify_session(Session st_new, Session st_old) {
    assert(st_new == st_old);
    return true;
}

void display_user_sessions(Allocator* al) {
    UUID u1("28255F79-ADFA-4D24-9091-6293ED050FB4");
    UUID u5("65C8C46C-2E0E-420E-96AC-38AF503234DD");
    UUID u8("3BBC67A4-DE48-4C60-A73D-DBB81CEFDE37");
    std::vector<UUID> users = {u1, u5, u8};

    for (UUID user : users) 
        std::cout << user.unparseUpper() << ": " 
                  << al->getActiveSessionsForUser(user) << std::endl;
}

bool verify_user_sessions(Allocator* al, uint32_t v1, uint32_t v5, uint32_t v8) {
    UUID u1("28255F79-ADFA-4D24-9091-6293ED050FB4");
    UUID u5("65C8C46C-2E0E-420E-96AC-38AF503234DD");
    UUID u8("3BBC67A4-DE48-4C60-A73D-DBB81CEFDE37");

    assert(al->getActiveSessionsForUser(u1) == v1);
    assert(al->getActiveSessionsForUser(u5) == v5);
    assert(al->getActiveSessionsForUser(u8) == v8);

    return true;
}
