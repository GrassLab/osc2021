#ifndef __ATOMIC_H_
#define __ATOMIC_H_

#ifdef RPI3
#define atomic_fetch_add(target, n) ({ typeof(target) tmp = target; target += n; tmp; })
#define atomic_fetch_sub(target, n) ({ typeof(target) tmp = target; target -= n; tmp; })
#define atomic_add_fetch(target, n) ({ target += n; target; })
#define atomic_sub_fetch(target, n) ({ target -= n; target; })
#else
#define atomic_fetch_add(target, n) __atomic_fetch_add(&(target), n, __ATOMIC_RELAXED)
#define atomic_fetch_sub(target, n) __atomic_fetch_sub(&(target), n, __ATOMIC_RELAXED)
#define atomic_add_fetch(target, n) __atomic_add_fetch(&(target), n, __ATOMIC_RELAXED)
#define atomic_sub_fetch(target, n) __atomic_sub_fetch(&(target), n, __ATOMIC_RELAXED)
#endif

#endif