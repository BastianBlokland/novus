#include "catch2/catch.hpp"
#include "helpers.hpp"
#include "novasm/pcall_code.hpp"
#include "vm/exec_state.hpp"

namespace vm {

TEST_CASE("Execute tcp platform-calls", "[vm]") {

  auto loopbackAddrIpV4 = std::string{127, 0, 0, 1};
  auto loopbackAddrIpV6 = std::string{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1};

  SECTION("Send and receive message IpV4") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");
          asmb->addStackAlloc(1);

          // Start server.
          asmb->addLoadLitInt(0);    // Address family: IpV4.
          asmb->addLoadLitInt(8080); // Port.
          asmb->addLoadLitInt(-1);   // Backlog (-1 uses the default backlog).
          asmb->addPCall(novasm::PCallCode::TcpStartServer);
          asmb->addStackStore(0); // Store the server stream.

          // Open connection to server and send message.
          asmb->addLoadLitString(loopbackAddrIpV4);
          asmb->addLoadLitInt(0);    // Address family: IpV4.
          asmb->addLoadLitInt(8080); // Port.
          asmb->addPCall(novasm::PCallCode::TcpOpenCon);
          asmb->addLoadLitString("Hello world");
          asmb->addPCall(novasm::PCallCode::StreamWriteString);
          asmb->addPop(); // Ignore the write result.

          // Accept the connection on the server and read the message.
          asmb->addStackLoad(0);
          asmb->addPCall(novasm::PCallCode::TcpAcceptCon);
          asmb->addLoadLitInt(11); // Length of 'Hello world'.
          asmb->addPCall(novasm::PCallCode::StreamReadString);

          // Print the received message.
          ADD_PRINT(asmb);
          asmb->addRet();
        },
        "input",
        "Hello world");
  }

  SECTION("Send and receive message IpV6") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");
          asmb->addStackAlloc(1);

          // Start server.
          asmb->addLoadLitInt(1);    // Address family: IpV6.
          asmb->addLoadLitInt(8080); // Port.
          asmb->addLoadLitInt(-1);   // Backlog (-1 uses the default backlog).
          asmb->addPCall(novasm::PCallCode::TcpStartServer);
          asmb->addStackStore(0); // Store the server stream.

          // Open connection to server and send message.
          asmb->addLoadLitString(loopbackAddrIpV6);
          asmb->addLoadLitInt(1);    // Address family: IpV6.
          asmb->addLoadLitInt(8080); // Port.
          asmb->addPCall(novasm::PCallCode::TcpOpenCon);
          asmb->addLoadLitString("Hello world");
          asmb->addPCall(novasm::PCallCode::StreamWriteString);
          asmb->addPop(); // Ignore the write result.

          // Accept the connection on the server and read the message.
          asmb->addStackLoad(0);
          asmb->addPCall(novasm::PCallCode::TcpAcceptCon);
          asmb->addLoadLitInt(11); // Length of 'Hello world'.
          asmb->addPCall(novasm::PCallCode::StreamReadString);

          // Print the received message.
          ADD_PRINT(asmb);
          asmb->addRet();
        },
        "input",
        "Hello world");
  }

  SECTION("Second accept-connection call fails") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->setEntrypoint("entry");

          // --- Main function start.
          asmb->label("entry");

          // Start server.
          asmb->addLoadLitInt(0);    // Address family: IpV4.
          asmb->addLoadLitInt(8080); // Port.
          asmb->addLoadLitInt(-1);   // Backlog (-1 uses the default backlog).
          asmb->addPCall(novasm::PCallCode::TcpStartServer);
          asmb->addDup(); // Place the server twice on the stack.

          // Start a background worker that accepts a client connection.
          asmb->addCall("worker", 1, novasm::CallMode::Forked);

          // Sleep for 100 milli-seconds to give the worker time to start.
          asmb->addLoadLitLong(100'000'000);
          asmb->addPCall(novasm::PCallCode::SleepNano);
          asmb->addPop(); // Ignore the return value of sleep.

          // Attemp to accept a new connection (while the background-worker is still accepting a
          // connection).
          asmb->addPCall(novasm::PCallCode::TcpAcceptCon);

          // And assert that it fails.
          asmb->addPCall(novasm::PCallCode::StreamCheckValid);
          asmb->addLogicInvInt();
          asmb->addLoadLitString("Expected second accept-connection to fail");
          asmb->addPCall(novasm::PCallCode::Assert);

          asmb->addRet();
          // --- Main function end.

          // --- Worker function start (takes one tcp-server-stream arg).
          asmb->label("worker");
          asmb->addStackLoad(0); // Load arg 0.
          asmb->addPCall(novasm::PCallCode::TcpAcceptCon);
          asmb->addRet();
          // --- Worker function end.
        },
        "input",
        "");
  }

  SECTION("Lookup address IpV4") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString("localhost"); // Hostname.
          asmb->addLoadLitInt(0);              // Address family: IpV4.
          asmb->addPCall(novasm::PCallCode::IpLookupAddress);

          asmb->addLoadLitString(loopbackAddrIpV4);
          asmb->addCheckEqString();

          asmb->addLoadLitString("Invalid loopback address received");
          asmb->addPCall(novasm::PCallCode::Assert);

          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Lookup address IpV6") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString("localhost"); // Hostname.
          asmb->addLoadLitInt(1);              // Address family: IpV6.
          asmb->addPCall(novasm::PCallCode::IpLookupAddress);

          asmb->addLoadLitString(loopbackAddrIpV6);
          asmb->addCheckEqString();

          asmb->addLoadLitString("Invalid loopback address received");
          asmb->addPCall(novasm::PCallCode::Assert);

          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Lookup invalid address IpV4") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString("doesnotexist"); // Hostname.
          asmb->addLoadLitInt(0);                 // Address family: IpV4.
          asmb->addPCall(novasm::PCallCode::IpLookupAddress);

          asmb->addLoadLitString("");
          asmb->addCheckEqString();

          asmb->addLoadLitString("Expected an empty string");
          asmb->addPCall(novasm::PCallCode::Assert);

          asmb->addRet();
        },
        "input",
        "");
  }

  SECTION("Lookup invalid address IpV6") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->label("entry");
          asmb->setEntrypoint("entry");

          asmb->addLoadLitString("doesnotexist"); // Hostname.
          asmb->addLoadLitInt(1);                 // Address family: IpV6.
          asmb->addPCall(novasm::PCallCode::IpLookupAddress);

          asmb->addLoadLitString("");
          asmb->addCheckEqString();

          asmb->addLoadLitString("Expected an empty string");
          asmb->addPCall(novasm::PCallCode::Assert);

          asmb->addRet();
        },
        "input",
        "");
  }
}

} // namespace vm
