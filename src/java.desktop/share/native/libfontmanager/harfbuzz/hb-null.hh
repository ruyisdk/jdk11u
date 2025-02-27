/*
 * Copyright © 2018  Google, Inc.
 *
 *  This is part of HarfBuzz, a text shaping library.
 *
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in
 * all copies of this software.
 *
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN
 * IF THE COPYRIGHT HOLDER HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 *
 * THE COPYRIGHT HOLDER SPECIFICALLY DISCLAIMS ANY WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE COPYRIGHT HOLDER HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *
 * Google Author(s): Behdad Esfahbod
 */

#ifndef HB_NULL_HH
#define HB_NULL_HH

#include "hb.hh"


/*
 * Static pools
 */

/* Global nul-content Null pool.  Enlarge as necessary. */

#define HB_NULL_POOL_SIZE 9880

/* Use SFINAE to sniff whether T has min_size; in which case return T::null_size,
 * otherwise return sizeof(T). */

/* The hard way...
 * https://stackoverflow.com/questions/7776448/sfinae-tried-with-bool-gives-compiler-error-template-argument-tvalue-invol
 */

template<bool> struct _hb_bool_type {};

template <typename T, typename B>
struct _hb_null_size
{ enum { value = sizeof (T) }; };
template <typename T>
struct _hb_null_size<T, _hb_bool_type<(bool) (1 + (unsigned int) T::min_size)> >
{ enum { value = T::null_size }; };

template <typename T>
struct hb_null_size
{ enum { value = _hb_null_size<T, _hb_bool_type<true> >::value }; };
#define hb_null_size(T) hb_null_size<T>::value

/* These doesn't belong here, but since is copy/paste from above, put it here. */

/* hb_static_size (T)
 * Returns T::static_size if T::min_size is defined, or sizeof (T) otherwise. */

template <typename T, typename B>
struct _hb_static_size
{ enum { value = sizeof (T) }; };
template <typename T>
struct _hb_static_size<T, _hb_bool_type<(bool) (1 + (unsigned int) T::min_size)> >
{ enum { value = T::static_size }; };

template <typename T>
struct hb_static_size
{ enum { value = _hb_static_size<T, _hb_bool_type<true> >::value }; };
#define hb_static_size(T) hb_static_size<T>::value


/* hb_assign (obj, value)
 * Calls obj.set (value) if obj.min_size is defined and value has different type
 * from obj, or obj = v otherwise. */

template <typename T, typename V, typename B>
struct _hb_assign
// add cast to please AIX xlc12.1
//{ static inline void value (T &o, const V v) { o = v; } };
{ static inline void value (T &o, const V v) { o = (T&) v; } };
template <typename T, typename V>
struct _hb_assign<T, V, _hb_bool_type<(bool) (1 + (unsigned int) T::min_size)> >
{ static inline void value (T &o, const V v) { o.set (v); } };
template <typename T>
struct _hb_assign<T, T, _hb_bool_type<(bool) (1 + (unsigned int) T::min_size)> >
{ static inline void value (T &o, const T v) { o = v; } };

template <typename T, typename V>
static inline void hb_assign (T &o, const V v)
{ _hb_assign<T, V, _hb_bool_type<true> >::value (o, v); }


/*
 * Null()
 */

extern HB_INTERNAL
hb_vector_size_impl_t const _hb_NullPool[(HB_NULL_POOL_SIZE + sizeof (hb_vector_size_impl_t) - 1) / sizeof (hb_vector_size_impl_t)];

/* Generic nul-content Null objects. */
template <typename Type>
static inline Type const & Null () {
  static_assert (hb_null_size (Type) <= HB_NULL_POOL_SIZE, "Increase HB_NULL_POOL_SIZE.");
  return *reinterpret_cast<Type const *> (_hb_NullPool);
}
template <typename QType>
struct NullHelper
{
  typedef typename hb_remove_const (typename hb_remove_reference (QType)) Type;
  static const Type & get_null () { return Null<Type> (); }
};
#define Null(Type) NullHelper<Type>::get_null ()

/* Specializations for arbitrary-content Null objects expressed in bytes. */
#define DECLARE_NULL_NAMESPACE_BYTES(Namespace, Type) \
        } /* Close namespace. */ \
        extern HB_INTERNAL const unsigned char _hb_Null_##Namespace##_##Type[Namespace::Type::null_size]; \
        template <> \
        /*static*/ inline const Namespace::Type& Null<Namespace::Type> () { \
          return *reinterpret_cast<const Namespace::Type *> (_hb_Null_##Namespace##_##Type); \
        } \
        namespace Namespace { \
        static_assert (true, "Just so we take semicolon after.")
#define DEFINE_NULL_NAMESPACE_BYTES(Namespace, Type) \
        const unsigned char _hb_Null_##Namespace##_##Type[Namespace::Type::null_size]

/* Specializations for arbitrary-content Null objects expressed as struct initializer. */
#define DECLARE_NULL_INSTANCE(Type) \
        extern HB_INTERNAL const Type _hb_Null_##Type; \
        template <> \
        /*static*/ inline const Type& Null<Type> () { \
          return _hb_Null_##Type; \
        } \
static_assert (true, "Just so we take semicolon after.")
#define DEFINE_NULL_INSTANCE(Type) \
        const Type _hb_Null_##Type

/* Global writable pool.  Enlarge as necessary. */

/* To be fully correct, CrapPool must be thread_local. However, we do not rely on CrapPool
 * for correct operation. It only exist to catch and divert program logic bugs instead of
 * causing bad memory access. So, races there are not actually introducing incorrectness
 * in the code. Has ~12kb binary size overhead to have it, also clang build fails with it. */
extern HB_INTERNAL
/*thread_local*/ hb_vector_size_impl_t _hb_CrapPool[(HB_NULL_POOL_SIZE + sizeof (hb_vector_size_impl_t) - 1) / sizeof (hb_vector_size_impl_t)];

/* CRAP pool: Common Region for Access Protection. */
template <typename Type>
static inline Type& Crap () {
  static_assert (hb_null_size (Type) <= HB_NULL_POOL_SIZE, "Increase HB_NULL_POOL_SIZE.");
  Type *obj = reinterpret_cast<Type *> (_hb_CrapPool);
  memcpy (static_cast < void *> (obj), &Null(Type), sizeof (*obj));
  return *obj;
}
template <typename QType>
struct CrapHelper
{
  typedef typename hb_remove_const (typename hb_remove_reference (QType)) Type;
  static Type & get_crap () { return Crap<Type> (); }
};
#define Crap(Type) CrapHelper<Type>::get_crap ()

template <typename Type>
struct CrapOrNullHelper {
  static Type & get () { return Crap(Type); }
};
template <typename Type>
struct CrapOrNullHelper<const Type> {
  static const Type & get () { return Null(Type); }
};
#define CrapOrNull(Type) CrapOrNullHelper<Type>::get ()


/*
 * hb_nonnull_ptr_t
 */

template <typename P>
struct hb_nonnull_ptr_t
{
  typedef typename hb_remove_pointer (P) T;

  hb_nonnull_ptr_t (T *v_ = nullptr) : v (v_) {}
  T * operator = (T *v_)   { return v = v_; }
  T * operator -> () const { return get (); }
  T & operator * () const  { return *get (); }
  T ** operator & () const { return &v; }
  /* Only auto-cast to const types. */
  template <typename C> operator const C * () const { return get (); }
  operator const char * () const { return (const char *) get (); }
  T * get () const { return v ? v : const_cast<T *> (&Null(T)); }
  T * get_raw () const { return v; }

  T *v;
};


#endif /* HB_NULL_HH */
