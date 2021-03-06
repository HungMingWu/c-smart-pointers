/*
 * The MIT License (MIT)
 *
 * Copyright © 2015 Franklin "Snaipe" Mathieu <http://snai.pe/>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef CSPTR_SMART_PTR_H_
# define CSPTR_SMART_PTR_H_

# include <string.h>
# include "apply.h"
# include "smalloc.h"

__attribute__ ((always_inline))
inline void sfree_stack(void *ptr) {
    void **real_ptr = ptr;
    sfree(*real_ptr);
    *real_ptr = NULL;
}

# define NOPAREN_(...) __VA_ARGS__

# define smart __attribute__ ((cleanup(sfree_stack)))
# define smart_ptr(Type, Val, Kind, Args...)                                \
    ({                                                                      \
        const __typeof__(Type[1]) dummy;                                    \
        void *var =                                                         \
            __builtin_choose_expr(sizeof (dummy[0]) == sizeof (dummy),      \
                smalloc(sizeof (Type), 0, Kind, ## Args),                   \
                smalloc(sizeof (dummy[0]),                                  \
                    sizeof (dummy) / sizeof (dummy[0]), Kind, ## Args));    \
        if (var != NULL) {                                                  \
            const __typeof__(Type) val = NOPAREN_ Val;                      \
            memcpy(var, &val, sizeof (Type));                               \
        }                                                                   \
        var;                                                                \
    })

# define shared_ptr(Type, Val, Args...) smart_ptr(Type, Val, SHARED , ## Args)
# define unique_ptr(Type, Val, Args...) smart_ptr(Type, Val, UNIQUE , ## Args)

# define DESTRUCTOR(Name, Attr, Type, Args...)                      \
    static void Name##_impl(__attribute__((unused)) Type *ptr,      \
                            __attribute__((unused)) void *meta);    \
    Attr void Name(void *ptr, void *meta) {                         \
        APPLY(DESTROY_MEMBER_, Type, Args)                          \
        Name##_impl(ptr, meta);                                     \
    }                                                               \
    static void Name##_impl(__attribute__((unused)) Type *ptr,      \
                            __attribute__((unused)) void *meta)

# define DESTROY_MEMBER_(Type, Member, ...) \
    sfree(ptr + offsetof(Type, Member));    \
    *(ptr + offsetof(Type, Member)) = NULL;

#endif /* !CSPTR_SMART_PTR_H_ */
