# Copyright 2018-2021 Peter Dimov
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

include(BoostTestJamfile OPTIONAL RESULT_VARIABLE HAVE_BOOST_TEST)

if(HAVE_BOOST_TEST)

boost_test_jamfile(FILE Jamfile.v2 LINK_LIBRARIES Boost::core Boost::static_assert Boost::type_traits)

set(BOOST_TEST_LINK_LIBRARIES Boost::core Boost::throw_exception)

boost_test(TYPE run SOURCES no_exceptions_support_test.cpp)

set(BOOST_TEST_LINK_LIBRARIES Boost::core Boost::utility)

boost_test(TYPE run SOURCES sv_conversion_test2.cpp)

set(BOOST_TEST_LINK_LIBRARIES Boost::core Boost::container_hash)

boost_test(TYPE run SOURCES sv_hash_test.cpp)

set(BOOST_TEST_LINK_LIBRARIES Boost::core Boost::config Boost::move Boost::smart_ptr)

boost_test(TYPE run SOURCES fclose_deleter_test.cpp)

set(BOOST_TEST_LINK_LIBRARIES Boost::core Boost::serialization)

boost_test(TYPE run SOURCES serialization_nvp_test.cpp)
boost_test(TYPE run SOURCES serialization_split_free_test.cpp)
boost_test(TYPE run SOURCES serialization_split_member_test.cpp)
boost_test(TYPE run SOURCES serialization_construct_data_test.cpp)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(BOOST_TEST_LINK_LIBRARIES Boost::core Threads::Threads)

boost_test(TYPE run SOURCES yield_prim_pthread_cancel_test.cpp)

endif()

add_subdirectory(swap)
