#include "internal/ref.hpp"
#include "internal/ref_atomic.hpp"
#include "internal/ref_future.hpp"
#include "internal/ref_iowatcher.hpp"
#include "internal/ref_process.hpp"
#include "internal/ref_stream_console.hpp"
#include "internal/ref_stream_file.hpp"
#include "internal/ref_stream_process.hpp"
#include "internal/ref_stream_tcp.hpp"
#include "internal/ref_string.hpp"
#include "internal/ref_string_link.hpp"
#include "internal/ref_struct.hpp"
#include "internal/ref_ulong.hpp"

namespace vm::internal {

auto Ref::destroy() noexcept -> void {

  /* Manually invoke the the destructor of the implementor class.
  Reason for not defining a virtual destructor is that this way we avoid the additional size that
  the vtable pointer takes. */

  switch (m_kind) {
  case RefKind::Atomic:
    downcastRef<AtomicRef>(this)->~AtomicRef();
    break;
  case RefKind::Struct:
    downcastRef<StructRef>(this)->~StructRef();
    break;
  case RefKind::Future:
    downcastRef<FutureRef>(this)->~FutureRef();
    break;
  case RefKind::String:
    downcastRef<StringRef>(this)->~StringRef();
    break;
  case RefKind::StringLink:
    downcastRef<StringLinkRef>(this)->~StringLinkRef();
    break;
  case RefKind::ULong:
    downcastRef<ULongRef>(this)->~ULongRef();
    break;
  case RefKind::StreamConsole:
    downcastRef<ConsoleStreamRef>(this)->~ConsoleStreamRef();
    break;
  case RefKind::StreamFile:
    downcastRef<FileStreamRef>(this)->~FileStreamRef();
    break;
  case RefKind::StreamTcp:
    downcastRef<TcpStreamRef>(this)->~TcpStreamRef();
    break;
  case RefKind::StreamProcess:
    downcastRef<ProcessStreamRef>(this)->~ProcessStreamRef();
    break;
  case RefKind::Process:
    downcastRef<ProcessRef>(this)->~ProcessRef();
    break;
  case RefKind::IOWatcher:
    downcastRef<IOWatcherRef>(this)->~IOWatcherRef();
    break;
  }
}

} // namespace vm::internal
