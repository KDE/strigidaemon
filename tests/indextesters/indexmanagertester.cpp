/* This file is part of Strigi Desktop Search
 *
 * Copyright (C) 2007 Flavio Castelli <flavio.castelli@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "indexmanagertester.h"
#include "indexmanager.h"
#include "indexpluginloader.h"

using namespace std;
using namespace Strigi;
using namespace strigiunittest;

CPPUNIT_TEST_SUITE_REGISTRATION( CLuceneIndexManagerTest );

IndexManagerTest::IndexManagerTest(const std::string& backendname)
    :m_backendname(backendname), m_indexpath(":memory:"), m_manager(0) {
}

void
IndexManagerTest::setUp() {
    m_manager = Strigi::IndexPluginLoader::createIndexManager(
        m_backendname.c_str(), m_indexpath.c_str());
}

void
IndexManagerTest::tearDown() {
    Strigi::IndexPluginLoader::deleteIndexManager(m_manager);
    // clean up data (if any)
    string cmd("rm -rf '" + m_indexpath + "'");
    system(cmd.c_str());
}

void
IndexManagerTest::testIndexReader() {
    Strigi::IndexReader* reader = m_manager->indexReader();
    CPPUNIT_ASSERT_MESSAGE("reader creation failed", reader);
}

void
IndexManagerTest::testIndexWriter() {
    Strigi::IndexWriter* writer = m_manager->indexWriter();
    CPPUNIT_ASSERT_MESSAGE("writer creation failed", writer);
}