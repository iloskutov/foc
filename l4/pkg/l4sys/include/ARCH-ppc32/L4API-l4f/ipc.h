/*!
 * \file
 * \brief   L4 IPC System Calls, PPC
 * \ingroup api_calls
 */
/*
 * (c) 2009 Adam Lackorzynski <adam@os.inf.tu-dresden.de>
 *     economic rights: Technische Universität Dresden (Germany)
 *
 * This file is part of TUD:OS and distributed under the terms of the
 * GNU General Public License 2.
 * Please see the COPYING-GPL-2 file for details.
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */
#pragma once

#ifdef __GNUC__

#include <l4/sys/compiler.h>

#include_next <l4/sys/ipc.h>

#define L4_IPC_CAPMAPMSG_BASE 0xf0000100   ///< Capability mapmsg base

#ifndef L4_SYSCALL_MAGIC_OFFSET	
#  define L4_SYSCALL_MAGIC_OFFSET	8
#endif
#define L4_SYSCALL_INVOKE		(-0x00000004-L4_SYSCALL_MAGIC_OFFSET)

L4_INLINE l4_msgtag_t
l4_ipc_call(l4_cap_idx_t dest, l4_utcb_t *utcb,
            l4_msgtag_t tag,
            l4_timeout_t timeout) L4_NOTHROW
{
  register l4_umword_t _dest     __asm__("r4") = dest | L4_SYSF_CALL;
  register l4_umword_t _timeout  __asm__("r5") = timeout.raw;
  register l4_umword_t _flags    __asm__("r6") = 0;
  register l4_msgtag_t _tag      __asm__("r3") = tag;
  (void)utcb;

  __asm__ __volatile__
    (" bla %[addr] \n"
     :
     "=r" (_dest),
     "=r" (_timeout),
     "=r" (_flags),
     "=r" (_tag)
     :
     [addr]"i" (L4_SYSCALL_INVOKE),
     "0" (_dest),
     "1" (_timeout),
     "2" (_flags),
     "3" (_tag)
     :
     "lr", "memory");

  tag.raw = _tag.raw; // because gcc doesn't return out of registers variables
  return tag;
}

L4_INLINE l4_msgtag_t
l4_ipc_reply_and_wait(l4_utcb_t *utcb, l4_msgtag_t tag,
                      l4_umword_t *label,
                      l4_timeout_t timeout) L4_NOTHROW
{
  register l4_umword_t _dest     __asm__("r4") = L4_INVALID_CAP | L4_SYSF_REPLY_AND_WAIT;
  register l4_umword_t _timeout  __asm__("r5") = timeout.raw;
  register l4_msgtag_t _tag      __asm__("r3") = tag;
  register l4_umword_t _flags    __asm__("r6") = 0;
  (void)utcb;

  __asm__ __volatile__
    (" bla %[addr] \n"
     :
     "=r" (_dest),
     "=r" (_timeout),
     "=r" (_flags),
     "=r" (_tag)
     :
     [addr]"i" (L4_SYSCALL_INVOKE),
     "0" (_dest),
     "1" (_timeout),
     "2" (_flags),
     "3" (_tag)
     :
     "lr", "memory");
  *label = _flags;
  tag.raw = _tag.raw; // because gcc doesn't return out of registers variables
  return tag;
}

L4_INLINE l4_msgtag_t
l4_ipc_send_and_wait(l4_cap_idx_t dest, l4_utcb_t *utcb,
                     l4_msgtag_t tag,
                     l4_umword_t *src,
                     l4_timeout_t timeout) L4_NOTHROW
{
  register l4_umword_t _dest     __asm__("r4") = dest | L4_SYSF_SEND_AND_WAIT;
  register l4_umword_t _timeout  __asm__("r5") = timeout.raw;
  register l4_msgtag_t _tag      __asm__("r3") = tag;
  register l4_umword_t _flags    __asm__("r6") = 0;
  (void)utcb;

  __asm__ __volatile__
    (" bla %[addr] \n"
     :
     "=r" (_dest),
     "=r" (_timeout),
     "=r" (_flags),
     "=r" (_tag)
     :
     [addr] "i" (L4_SYSCALL_INVOKE),
     "0" (_dest),
     "1" (_timeout),
     "2" (_flags),
     "3" (_tag)
     :
     "memory", "lr");
  *src = _flags;
  tag.raw = _tag.raw; // because gcc doesn't return out of registers variables
  return tag;
}

L4_INLINE l4_msgtag_t
l4_ipc_send(l4_cap_idx_t dest, l4_utcb_t *utcb,
            l4_msgtag_t tag,
            l4_timeout_t timeout) L4_NOTHROW
{
  register l4_umword_t _dest     __asm__("r4") = dest | L4_SYSF_SEND;
  register l4_umword_t _timeout  __asm__("r5") = timeout.raw;
  register l4_umword_t _flags    __asm__("r6") = 0;
  register l4_msgtag_t _tag      __asm__("r3") = tag;
  (void)utcb;

  __asm__ __volatile__
    ("bla %[addr] \n"
     :
     "=r" (_dest),
     "=r" (_timeout),
     "=r" (_flags),
     "=r" (_tag)
     :
     [addr] "i" (L4_SYSCALL_INVOKE),
     "0" (_dest),
     "1" (_timeout),
     "2" (_flags),
     "3" (_tag)
     :
     "memory", "lr");
  tag.raw = _tag.raw; // because gcc doesn't return out of registers variables
  return tag;
}

L4_INLINE l4_msgtag_t
l4_ipc_wait(l4_utcb_t *utcb, l4_umword_t *src,
            l4_timeout_t timeout) L4_NOTHROW
{
  l4_msgtag_t rtag;
  register l4_umword_t _r        __asm__("r4") = L4_INVALID_CAP | L4_SYSF_WAIT;
  register l4_umword_t _timeout  __asm__("r5") = timeout.raw;
  register l4_msgtag_t _tag      __asm__("r3");
  register l4_umword_t _flags    __asm__("r6") = 0;
  (void)utcb;
  _tag.raw = 0;

  __asm__ __volatile__
    ("bla %[addr] \n"
     :
     "=r"(_r),
     "=r"(_timeout),
     "=r"(_flags),
     "=r"(_tag)
     :
     [addr] "i"(L4_SYSCALL_INVOKE),
     "0"(_r),
     "1"(_timeout),
     "2"(_flags),
     "3"(_tag)

     :
     "memory", "lr");
  *src     = _flags;
  rtag.raw = _tag.raw; // because gcc doesn't return out of registers variables
  return rtag;
}

L4_INLINE l4_msgtag_t
l4_ipc_receive(l4_cap_idx_t src, l4_utcb_t *utcb,
               l4_timeout_t timeout) L4_NOTHROW
{
  l4_msgtag_t rtag;
  register l4_umword_t _r        __asm__("r4") = src | L4_SYSF_RECV;
  register l4_umword_t _timeout  __asm__("r5") = timeout.raw;
  register l4_msgtag_t _tag      __asm__("r3");
  register l4_umword_t _flags    __asm__("r6") = 0;
  (void)utcb;

  _tag.raw = 0;

  __asm__ __volatile__
    ("bla %[addr] \n"
     :
     "=r"(_r),
     "=r"(_timeout),
     "=r"(_flags),
     "=r"(_tag)
     :
     [addr] "i"(L4_SYSCALL_INVOKE),
     "0"(_r),
     "1"(_timeout),
     "2"(_flags),
     "3"(_tag)
     :
     "memory", "lr");
  rtag.raw = _tag.raw; // because gcc doesn't return out of registers variables
  return rtag;
}

// todo: let all calls above use this single call
L4_INLINE l4_msgtag_t
l4_ipc(l4_cap_idx_t dest, l4_utcb_t *utcb,
       l4_umword_t flags,
       l4_umword_t slabel,
       l4_msgtag_t tag,
       l4_umword_t *rlabel,
       l4_timeout_t timeout) L4_NOTHROW
{
  register l4_umword_t _dest     __asm__("r4") = dest | flags;
  register l4_umword_t _timeout  __asm__("r5") = timeout.raw;
  register l4_msgtag_t _tag      __asm__("r3") = tag;
  register l4_umword_t _lab      __asm__("r6") = slabel;
  (void)utcb;

  __asm__ __volatile__
    ("bla %[addr] \n"
     :
     "=r" (_dest),
     "=r" (_timeout),
     "=r" (_lab),
     "=r" (_tag)
     :
     [addr] "i" (L4_SYSCALL_INVOKE),
     "0" (_dest),
     "1" (_timeout),
     "2" (_lab),
     "3" (_tag)
     :
     "memory", "lr");
  *rlabel = _lab;
  tag.raw = _tag.raw; // because gcc doesn't return out of registers variables
  return tag;
}

#include <l4/sys/ipc-impl.h>

#endif //__GNUC__
