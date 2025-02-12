// Allocator traits -*- C++ -*-

// Copyright (C) 2011-2015 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file bits/alloc_traits.h
 *  This is an internal header file, included by other library headers.
 *  Do not attempt to use it directly. @headername{memory}
 */

#ifndef _ALLOC_TRAITS_H
#define _ALLOC_TRAITS_H 1

#if __cplusplus >= 201103L

#include <bits/memoryfwd.h>
#include <bits/ptr_traits.h>
#include <ext/numeric_traits.h>

namespace std _GLIBCXX_VISIBILITY(default)
{
_GLIBCXX_BEGIN_NAMESPACE_VERSION

  struct __allocator_traits_base
  {
    template<typename _Alloc, typename _Up>
      using __rebind = typename _Alloc::template rebind<_Up>::other;

  protected:
    template<typename _Tp>
      using __pointer = typename _Tp::pointer;
    template<typename _Tp>
      using __c_pointer = typename _Tp::const_pointer;
    template<typename _Tp>
      using __v_pointer = typename _Tp::void_pointer;
    template<typename _Tp>
      using __cv_pointer = typename _Tp::const_void_pointer;
    template<typename _Tp>
      using __diff_type = typename _Tp::difference_type;
    template<typename _Tp>
      using __size_type = typename _Tp::size_type;
    template<typename _Tp>
      using __pocca = typename _Tp::propagate_on_container_copy_assignment;
    template<typename _Tp>
      using __pocma = typename _Tp::propagate_on_container_move_assignment;
    template<typename _Tp>
      using __pocs = typename _Tp::propagate_on_container_swap;
    template<typename _Tp>
      using __equal = typename _Tp::is_always_equal;
  };

  template<typename _Alloc, typename _Up>
    using __alloc_rebind = __detected_or_t_<__replace_first_arg_t,
					    __allocator_traits_base::__rebind,
					    _Alloc, _Up>;

  /**
   * @brief  Uniform interface to all allocator types.
   * @ingroup allocators
  */
  template<typename _Alloc>
    struct allocator_traits : __allocator_traits_base
    {
      /// The allocator type
      typedef _Alloc allocator_type;
      /// The allocated type
      typedef typename _Alloc::value_type value_type;

      /**
       * @brief   The allocator's pointer type.
       *
       * @c Alloc::pointer if that type exists, otherwise @c value_type*
      */
      using pointer = __detected_or_t<value_type*, __pointer, _Alloc>;

      /**
       * @brief   The allocator's const pointer type.
       *
       * @c Alloc::const_pointer if that type exists, otherwise
       * <tt> pointer_traits<pointer>::rebind<const value_type> </tt>
      */
      using const_pointer
	= __detected_or_t<__ptr_rebind<pointer, const value_type>,
			  __c_pointer, _Alloc>;

      /**
       * @brief   The allocator's void pointer type.
       *
       * @c Alloc::void_pointer if that type exists, otherwise
       * <tt> pointer_traits<pointer>::rebind<void> </tt>
      */
      using void_pointer
	= __detected_or_t<__ptr_rebind<pointer, void>, __v_pointer, _Alloc>;

      /**
       * @brief   The allocator's const void pointer type.
       *
       * @c Alloc::const_void_pointer if that type exists, otherwise
       * <tt> pointer_traits<pointer>::rebind<const void> </tt>
      */
      using const_void_pointer
	= __detected_or_t<__ptr_rebind<pointer, const void>, __cv_pointer,
			  _Alloc>;

      /**
       * @brief   The allocator's difference type
       *
       * @c Alloc::difference_type if that type exists, otherwise
       * <tt> pointer_traits<pointer>::difference_type </tt>
      */
      using difference_type
	= __detected_or_t<typename pointer_traits<pointer>::difference_type,
			  __diff_type, _Alloc>;

      /**
       * @brief   The allocator's size type
       *
       * @c Alloc::size_type if that type exists, otherwise
       * <tt> make_unsigned<difference_type>::type </tt>
      */
      using size_type
	= __detected_or_t<typename make_unsigned<difference_type>::type,
			  __size_type, _Alloc>;

      /**
       * @brief   How the allocator is propagated on copy assignment
       *
       * @c Alloc::propagate_on_container_copy_assignment if that type exists,
       * otherwise @c false_type
      */
      using propagate_on_container_copy_assignment
	= __detected_or_t<false_type, __pocca, _Alloc>;

      /**
       * @brief   How the allocator is propagated on move assignment
       *
       * @c Alloc::propagate_on_container_move_assignment if that type exists,
       * otherwise @c false_type
      */
      using propagate_on_container_move_assignment
	= __detected_or_t<false_type, __pocma, _Alloc>;

      /**
       * @brief   How the allocator is propagated on swap
       *
       * @c Alloc::propagate_on_container_swap if that type exists,
       * otherwise @c false_type
      */
      using propagate_on_container_swap
	= __detected_or_t<false_type, __pocs, _Alloc>;

      /**
       * @brief   Whether all instances of the allocator type compare equal.
       *
       * @c Alloc::is_always_equal if that type exists,
       * otherwise @c is_empty<Alloc>::type
      */
      using is_always_equal
	= __detected_or_t<typename is_empty<_Alloc>::type, __equal, _Alloc>;

      template<typename _Tp>
	using rebind_alloc = __alloc_rebind<_Alloc, _Tp>;
      template<typename _Tp>
	using rebind_traits = allocator_traits<rebind_alloc<_Tp>>;

      static_assert(!is_same<rebind_alloc<value_type>, __undefined>::value,
	  "allocator defines rebind or is like Alloc<T, Args>");

    private:
      template<typename _Alloc2>
	static auto
	_S_allocate(_Alloc2& __a, size_type __n, const_void_pointer __hint, int)
	-> decltype(__a.allocate(__n, __hint))
	{ return __a.allocate(__n, __hint); }

      template<typename _Alloc2>
	static pointer
	_S_allocate(_Alloc2& __a, size_type __n, const_void_pointer, ...)
	{ return __a.allocate(__n); }

      template<typename _Tp, typename... _Args>
	struct __construct_helper
	{
	  template<typename _Alloc2,
	    typename = decltype(std::declval<_Alloc2*>()->construct(
		  std::declval<_Tp*>(), std::declval<_Args>()...))>
	    static true_type __test(int);

	  template<typename>
	    static false_type __test(...);

	  using type = decltype(__test<_Alloc>(0));
	};

      template<typename _Tp, typename... _Args>
	using __has_construct
	  = typename __construct_helper<_Tp, _Args...>::type;

      template<typename _Tp, typename... _Args>
	static _Require<__has_construct<_Tp, _Args...>>
	_S_construct(_Alloc& __a, _Tp* __p, _Args&&... __args)
	{ __a.construct(__p, std::forward<_Args>(__args)...); }

      template<typename _Tp, typename... _Args>
	static
	_Require<__and_<__not_<__has_construct<_Tp, _Args...>>,
			       is_constructible<_Tp, _Args...>>>
	_S_construct(_Alloc&, _Tp* __p, _Args&&... __args)
	{ ::new((void*)__p) _Tp(std::forward<_Args>(__args)...); }

      template<typename _Alloc2, typename _Tp>
	static auto
	_S_destroy(_Alloc2& __a, _Tp* __p, int)
	-> decltype(__a.destroy(__p))
	{ __a.destroy(__p); }

      template<typename _Alloc2, typename _Tp>
	static void
	_S_destroy(_Alloc2&, _Tp* __p, ...)
	{ __p->~_Tp(); }

      template<typename _Alloc2>
	static auto
	_S_max_size(_Alloc2& __a, int)
	-> decltype(__a.max_size())
	{ return __a.max_size(); }

      template<typename _Alloc2>
	static size_type
	_S_max_size(_Alloc2&, ...)
	{
	  // _GLIBCXX_RESOLVE_LIB_DEFECTS
	  // 2466. allocator_traits::max_size() default behavior is incorrect
	  return __gnu_cxx::__numeric_traits<size_type>::__max
	    / sizeof(value_type);
	}

      template<typename _Alloc2>
	static auto
	_S_select(_Alloc2& __a, int)
	-> decltype(__a.select_on_container_copy_construction())
	{ return __a.select_on_container_copy_construction(); }

      template<typename _Alloc2>
	static _Alloc2
	_S_select(_Alloc2& __a, ...)
	{ return __a; }

    public:

      /**
       *  @brief  Allocate memory.
       *  @param  __a  An allocator.
       *  @param  __n  The number of objects to allocate space for.
       *
       *  Calls @c a.allocate(n)
      */
      static pointer
      allocate(_Alloc& __a, size_type __n)
      { return __a.allocate(__n); }

      /**
       *  @brief  Allocate memory.
       *  @param  __a  An allocator.
       *  @param  __n  The number of objects to allocate space for.
       *  @param  __hint Aid to locality.
       *  @return Memory of suitable size and alignment for @a n objects
       *          of type @c value_type
       *
       *  Returns <tt> a.allocate(n, hint) </tt> if that expression is
       *  well-formed, otherwise returns @c a.allocate(n)
      */
      static pointer
      allocate(_Alloc& __a, size_type __n, const_void_pointer __hint)
      { return _S_allocate(__a, __n, __hint, 0); }

      /**
       *  @brief  Deallocate memory.
       *  @param  __a  An allocator.
       *  @param  __p  Pointer to the memory to deallocate.
       *  @param  __n  The number of objects space was allocated for.
       *
       *  Calls <tt> a.deallocate(p, n) </tt>
      */
      static void
      deallocate(_Alloc& __a, pointer __p, size_type __n)
      { __a.deallocate(__p, __n); }

      /**
       *  @brief  Construct an object of type @a _Tp
       *  @param  __a  An allocator.
       *  @param  __p  Pointer to memory of suitable size and alignment for Tp
       *  @param  __args Constructor arguments.
       *
       *  Calls <tt> __a.construct(__p, std::forward<Args>(__args)...) </tt>
       *  if that expression is well-formed, otherwise uses placement-new
       *  to construct an object of type @a _Tp at location @a __p from the
       *  arguments @a __args...
      */
      template<typename _Tp, typename... _Args>
	static auto construct(_Alloc& __a, _Tp* __p, _Args&&... __args)
	-> decltype(_S_construct(__a, __p, std::forward<_Args>(__args)...))
	{ _S_construct(__a, __p, std::forward<_Args>(__args)...); }

      /**
       *  @brief  Destroy an object of type @a _Tp
       *  @param  __a  An allocator.
       *  @param  __p  Pointer to the object to destroy
       *
       *  Calls @c __a.destroy(__p) if that expression is well-formed,
       *  otherwise calls @c __p->~_Tp()
      */
      template <class _Tp>
	static void destroy(_Alloc& __a, _Tp* __p)
	{ _S_destroy(__a, __p, 0); }

      /**
       *  @brief  The maximum supported allocation size
       *  @param  __a  An allocator.
       *  @return @c __a.max_size() or @c numeric_limits<size_type>::max()
       *
       *  Returns @c __a.max_size() if that expression is well-formed,
       *  otherwise returns @c numeric_limits<size_type>::max()
      */
      static size_type max_size(const _Alloc& __a) noexcept
      { return _S_max_size(__a, 0); }

      /**
       *  @brief  Obtain an allocator to use when copying a container.
       *  @param  __rhs  An allocator.
       *  @return @c __rhs.select_on_container_copy_construction() or @a __rhs
       *
       *  Returns @c __rhs.select_on_container_copy_construction() if that
       *  expression is well-formed, otherwise returns @a __rhs
      */
      static _Alloc
      select_on_container_copy_construction(const _Alloc& __rhs)
      { return _S_select(__rhs, 0); }
    };

  template<typename _Alloc>
    inline void
    __do_alloc_on_copy(_Alloc& __one, const _Alloc& __two, true_type)
    { __one = __two; }

  template<typename _Alloc>
    inline void
    __do_alloc_on_copy(_Alloc&, const _Alloc&, false_type)
    { }

  template<typename _Alloc>
    inline void __alloc_on_copy(_Alloc& __one, const _Alloc& __two)
    {
      typedef allocator_traits<_Alloc> __traits;
      typedef typename __traits::propagate_on_container_copy_assignment __pocca;
      __do_alloc_on_copy(__one, __two, __pocca());
    }

  template<typename _Alloc>
    inline _Alloc __alloc_on_copy(const _Alloc& __a)
    {
      typedef allocator_traits<_Alloc> __traits;
      return __traits::select_on_container_copy_construction(__a);
    }

  template<typename _Alloc>
    inline void __do_alloc_on_move(_Alloc& __one, _Alloc& __two, true_type)
    { __one = std::move(__two); }

  template<typename _Alloc>
    inline void __do_alloc_on_move(_Alloc&, _Alloc&, false_type)
    { }

  template<typename _Alloc>
    inline void __alloc_on_move(_Alloc& __one, _Alloc& __two)
    {
      typedef allocator_traits<_Alloc> __traits;
      typedef typename __traits::propagate_on_container_move_assignment __pocma;
      __do_alloc_on_move(__one, __two, __pocma());
    }

  template<typename _Alloc>
    inline void __do_alloc_on_swap(_Alloc& __one, _Alloc& __two, true_type)
    {
      using std::swap;
      swap(__one, __two);
    }

  template<typename _Alloc>
    inline void __do_alloc_on_swap(_Alloc&, _Alloc&, false_type)
    { }

  template<typename _Alloc>
    inline void __alloc_on_swap(_Alloc& __one, _Alloc& __two)
    {
      typedef allocator_traits<_Alloc> __traits;
      typedef typename __traits::propagate_on_container_swap __pocs;
      __do_alloc_on_swap(__one, __two, __pocs());
    }

  template<typename _Alloc>
    class __is_copy_insertable_impl
    {
      typedef allocator_traits<_Alloc> _Traits;

      template<typename _Up, typename
	       = decltype(_Traits::construct(std::declval<_Alloc&>(),
					     std::declval<_Up*>(),
					     std::declval<const _Up&>()))>
	static true_type
	_M_select(int);

      template<typename _Up>
	static false_type
	_M_select(...);

    public:
      typedef decltype(_M_select<typename _Alloc::value_type>(0)) type;
    };

  // true if _Alloc::value_type is CopyInsertable into containers using _Alloc
  template<typename _Alloc>
    struct __is_copy_insertable
    : __is_copy_insertable_impl<_Alloc>::type
    { };

  // std::allocator<_Tp> just requires CopyConstructible
  template<typename _Tp>
    struct __is_copy_insertable<allocator<_Tp>>
    : is_copy_constructible<_Tp>
    { };

_GLIBCXX_END_NAMESPACE_VERSION
} // namespace std

#endif
#endif
