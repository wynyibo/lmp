// copied from
// https://github.com/merbridge/merbridge/blob/0.7.0/bpf/headers/helpers.h
#pragma once
#include <linux/bpf.h>
#include <linux/bpf_common.h>
#include <linux/in.h>
#include <linux/swab.h>
#include <linux/types.h>

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define bpf_htons(x) __builtin_bswap16(x)
#define bpf_htonl(x) __builtin_bswap32(x)
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define bpf_htons(x) (x)
#define bpf_htonl(x) (x)
#else
#error "__BYTE_ORDER__ error"
#endif

#ifndef __section
#define __section(NAME) __attribute__((section(NAME), used))
#endif

#define PIN_GLOBAL_NS 2

struct bpf_elf_map {
  __u32 type;
  __u32 size_key;
  __u32 size_value;
  __u32 max_elem;
  __u32 flags;
  __u32 id;
  __u32 pinning;
};

struct record {
  __u32 count;
  __u32 fail_count;
  // any request count
  __u32 any_count;
  __u32 padding;
};

struct dns_hdr {
  __u16 id;
  __u16 flags;
  __u16 qdcount;
  __u16 ancount;
  __u16 nscount;
  __u16 arcount;
};

static __u64 (*bpf_get_current_pid_tgid)() = (void *)
    BPF_FUNC_get_current_pid_tgid;
static __u64 (*bpf_get_current_uid_gid)() = (void *)
    BPF_FUNC_get_current_uid_gid;
static void (*bpf_trace_printk)(const char *fmt, int fmt_size,
                                ...) = (void *)BPF_FUNC_trace_printk;
static void *(*bpf_map_lookup_elem)(struct bpf_elf_map *map, const void *key) =
    (void *)BPF_FUNC_map_lookup_elem;
static __u64 (*bpf_map_update_elem)(struct bpf_elf_map *map, const void *key,
                                    const void *value, __u64 flags) = (void *)
    BPF_FUNC_map_update_elem;
static __u64 (*bpf_map_delete_elem)(struct bpf_elf_map *map, const void *key) =
    (void *)BPF_FUNC_map_delete_elem;
static __u64 (*bpf_perf_event_output)(void *ctx, struct bpf_elf_map *map,
                                      __u64 flags, void *data, __u64 size) =
    (void *)BPF_FUNC_perf_event_output;

#ifdef PRINTNL
#define PRINT_SUFFIX "\n"
#else
#define PRINT_SUFFIX ""
#endif

#ifndef printk
#define printk(fmt, ...)                                                       \
  ({                                                                           \
    char ____fmt[] = fmt PRINT_SUFFIX;                                         \
    bpf_trace_printk(____fmt, sizeof(____fmt), ##__VA_ARGS__);                 \
  })
#endif