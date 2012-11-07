/*
 * Copyright (c) 2012 Glen Joseph Fernandes 
 * glenfe at live dot com
 *
 * Distributed under the Boost Software License, 
 * Version 1.0. (See accompanying file LICENSE_1_0.txt 
 * or copy at http://boost.org/LICENSE_1_0.txt)
 */
#ifndef BOOST_SMART_PTR_DETAIL_ARRAY_DELETER_HPP
#define BOOST_SMART_PTR_DETAIL_ARRAY_DELETER_HPP

#include <boost/config.hpp>
#include <boost/smart_ptr/detail/array_helper.hpp>
#include <cstddef>

namespace boost {
    namespace detail {
        template<typename T>
        class array_deleter {
        public:
            array_deleter() 
                : size(0) {
            }
            ~array_deleter() {
                destroy();
            }
            void construct(T* memory, std::size_t count) {
                for (object = memory; size < count; size++) {
                    array_helper<T>::create(object[size]);
                }
            }
#if defined(BOOST_HAS_VARIADIC_TMPL) && defined(BOOST_HAS_RVALUE_REFS)
            template<typename... Args>
            void construct(T* memory, std::size_t count, Args&&... args) {
                for (object = memory; size < count; size++) {
                    array_helper<T>::create(object[size], args...);
                }
            }
#endif
            void construct_noinit(T* memory, std::size_t count) {
                for (object = memory; size < count; size++) {
                    array_helper<T>::create_noinit(object[size]);
                }
            }
            void operator()(T*) {
                destroy();
            }
        private:
            void destroy() {
                while (size > 0) {
                    array_helper<T>::destroy(object[--size]);
                }
            }
            std::size_t size;
            T* object;
        };
    }
}

#endif
