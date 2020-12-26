#include "internal/memory_allocator.hpp"
#include "internal/os_include.hpp"
#include "intrinsics.hpp"
#include <cassert>
#include <mutex>
#include <vector>

namespace vm::internal {

/*
Naive allocator, allocations for 'chunkSize' or less are allocated from a 'pool' of
chunks. Bigger allocations are handled through the systems 'malloc' and 'free'.

When an allocation for a chunk comes in it tries to satisfy it from a thread-cache, if the
thread-cache is empty it refills the thread-cache from a global cache. If the global cache is empty
it will allocate new memory from the system.

When free-ing a chunk it is placed on the thread-cache, if the thread-cache reaches capacity
'maxChunksPerThread' it will move the set to the global cache.

Note: Current implementation of the garbage collector free's only on the gc-thread and no
allocations happen on the gc-thread. Implementation of the MemoryAllocator should NOT rely on that
being true but it can be taken into account when tuning performance.

Note: All allocations are aligned to multiples of 64 bytes from the systems virtual page start.

Note: Pooled memory is never returned back to the system at the moment.
*/

// On MinGW there is a bug in thread_local objects with destructors.
// More info: https://sourceforge.net/p/mingw-w64/bugs/527/
// as a workaround we disable our custom allocator and fall back to the systems 'malloc' / 'free'.
// TODO(bastian): Investigate which MinGw versions are affected.
#if defined(__MINGW32__)
#define CUSTOM_ALLOCATOR_ENABLED false
#else // !__MINGW32__
#define CUSTOM_ALLOCATOR_ENABLED true
#endif

#if CUSTOM_ALLOCATOR_ENABLED

#if defined(_WIN32)
static auto getpagesize() -> unsigned int {
  SYSTEM_INFO si;
  GetSystemInfo(&si);
  return si.dwPageSize;
}
#endif

const unsigned int pageSize                 = getpagesize(); // Systems virtual page-size.
const unsigned int pagesPerArea             = 50U;
const unsigned int areaSize                 = pageSize * pagesPerArea;
const unsigned int chunkSize                = 64U;
const unsigned int chunksPerArea            = areaSize / chunkSize;
const unsigned int maxChunksPerThread       = chunksPerArea;
const unsigned int preAllocateChunkSetCount = 10U;

enum class MemTag : uint8_t {
  MAlloc = 0,
  Chunk  = 1,
};

struct Chunk {
  Chunk* next;
};

struct ChunkSet {
  Chunk* head;
  unsigned int count;
};

static auto getChunkSet() noexcept -> ChunkSet;
static auto freeChunkSet(ChunkSet s) noexcept -> void;

struct ThreadCache {
  ChunkSet chunks;

  ~ThreadCache() noexcept {
    if (chunks.count > 0) {
      freeChunkSet(chunks);
    }
  }
};

thread_local static ThreadCache threadCache;

static std::mutex globalChunksMutex;
static std::vector<ChunkSet> globalChunkSets;
static bool chunkSetsPreAllocated;

#if defined(_WIN32)
inline auto mapArea() noexcept -> void* {
  return VirtualAlloc(nullptr, areaSize, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
}
#else // !_WIN32
inline auto mapArea() noexcept -> void* {
  auto* res = mmap(nullptr, areaSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  return unlikely(res == MAP_FAILED) ? nullptr : res;
}
#endif

// Allocate an area and fill it with chunks.
inline auto allocChunkSet() noexcept -> ChunkSet {
  auto* area = mapArea();
  if (unlikely(area == nullptr)) {
    return ChunkSet{nullptr, 0};
  }

  auto* begin = static_cast<Chunk*>(area);
  auto* cur   = begin;
  for (auto i = 0U; i != chunksPerArea - 1; ++i) {
    void* curMem  = static_cast<void*>(cur);
    void* nextMem = static_cast<void*>(static_cast<uint8_t*>(curMem) + chunkSize);

    cur->next = static_cast<Chunk*>(nextMem);
    cur       = cur->next;
  }

  cur->next = nullptr;
  return ChunkSet{begin, chunksPerArea};
}

inline auto getChunkSet() noexcept -> ChunkSet {
  {
    auto lk = std::lock_guard<std::mutex>{globalChunksMutex};
    if (!globalChunkSets.empty()) {
      auto set = globalChunkSets.back();
      globalChunkSets.pop_back();
      assert(set.head != nullptr && set.count > 0);
      return set;
    }
  }
  return allocChunkSet();
}

inline auto freeChunkSet(ChunkSet chunkSet) noexcept -> void {
  assert(chunkSet.head != nullptr && chunkSet.count > 0);

  auto lk = std::lock_guard<std::mutex>{globalChunksMutex};
  globalChunkSets.push_back(chunkSet);
}

inline auto preAllocateChunkSets() noexcept {
  globalChunkSets.reserve(preAllocateChunkSetCount);
  for (auto i = 0U; i != preAllocateChunkSetCount; ++i) {
    auto chunkSet = allocChunkSet();
    if (unlikely(chunkSet.head == nullptr)) {
      return;
    }
    freeChunkSet(chunkSet);
  }
}

inline auto getThreadChunk() noexcept -> Chunk* {
  if (threadCache.chunks.count == 0) {
    threadCache.chunks = getChunkSet();
    if (unlikely(threadCache.chunks.head == nullptr)) {
      return nullptr;
    }
  }

  auto* chunk             = threadCache.chunks.head;
  threadCache.chunks.head = chunk->next;
  --threadCache.chunks.count;
  return chunk;
}

inline auto freeChunk(Chunk* chunk) noexcept -> void {
  chunk->next             = threadCache.chunks.head;
  threadCache.chunks.head = chunk;
  ++threadCache.chunks.count;

  if (threadCache.chunks.count > maxChunksPerThread) {
    freeChunkSet(threadCache.chunks);
    threadCache.chunks = ChunkSet{};
  }
}

MemoryAllocator::MemoryAllocator() noexcept {
  if (!chunkSetsPreAllocated) {
    preAllocateChunkSets();
    chunkSetsPreAllocated = true;
  }
}

auto MemoryAllocator::alloc(unsigned int size) noexcept -> std::pair<void*, uint8_t> {
  if (size > chunkSize) {
    return {std::malloc(size), static_cast<uint8_t>(MemTag::MAlloc)};
  }
  auto* chunk = getThreadChunk();
  return {static_cast<void*>(chunk), static_cast<uint8_t>(MemTag::Chunk)};
}

auto MemoryAllocator::free(void* memoryPtr, uint8_t memTag) noexcept -> void {
  assert(memoryPtr != nullptr);

  switch (static_cast<MemTag>(memTag)) {
  case MemTag::MAlloc:
    std::free(memoryPtr);
    break;
  case MemTag::Chunk:
    freeChunk(static_cast<Chunk*>(memoryPtr));
    break;
  default:
    assert(false);
    break;
  }
}

#else // !CUSTOM_ALLOCATOR_ENABLED

MemoryAllocator::MemoryAllocator() noexcept {}

auto MemoryAllocator::alloc(unsigned int size) noexcept -> std::pair<void*, uint8_t> {
  return {std::malloc(size), 0};
}

auto MemoryAllocator::free(void* memoryPtr, uint8_t /*unused*/) noexcept -> void {
  assert(memoryPtr != nullptr);
  std::free(memoryPtr);
}

#endif

} // namespace vm::internal
