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
          asmb->addLoadLitInt(5000); // Port.
          asmb->addLoadLitInt(-1);   // Backlog (-1 uses the default backlog).
          asmb->addPCall(novasm::PCallCode::TcpStartServer);
          asmb->addStackStore(0); // Store the server stream.

          // Open connection to server and send message.
          asmb->addLoadLitString(loopbackAddrIpV4);
          asmb->addLoadLitInt(0);    // Address family: IpV4.
          asmb->addLoadLitInt(5000); // Port.
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
          asmb->addLoadLitInt(5001); // Port.
          asmb->addLoadLitInt(-1);   // Backlog (-1 uses the default backlog).
          asmb->addPCall(novasm::PCallCode::TcpStartServer);
          asmb->addStackStore(0); // Store the server stream.

          // Open connection to server and send message.
          asmb->addLoadLitString(loopbackAddrIpV6);
          asmb->addLoadLitInt(1);    // Address family: IpV6.
          asmb->addLoadLitInt(5001); // Port.
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

  SECTION("Server shutdown cancels open-connection request") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->setEntrypoint("entry");

          // --- Main function start.
          asmb->label("entry");
          asmb->addStackAlloc(2);

          // Start server.
          asmb->addLoadLitInt(0);    // Address family: IpV4.
          asmb->addLoadLitInt(5002); // Port.
          asmb->addLoadLitInt(-1);   // Backlog (-1 uses the default backlog).
          asmb->addPCall(novasm::PCallCode::TcpStartServer);
          asmb->addStackStore(0); // Place the server on the stack at slot 0.

          // Start a background worker that accepts a client connection.
          asmb->addStackLoad(0);
          asmb->addCall("worker", 1, novasm::CallMode::Forked);
          asmb->addStackStore(1); // Place the future on the stack at slot 1.

          // Sleep for 100 milli-seconds to give the worker time to start.
          asmb->addLoadLitLong(100'000'000);
          asmb->addPCall(novasm::PCallCode::SleepNano);
          asmb->addPop(); // Ignore the return value of sleep.

          // Shutdown the server.
          asmb->addStackLoad(0);
          asmb->addPCall(novasm::PCallCode::TcpShutdown);

          // Assert that the shutdown completed successfully.
          asmb->addLoadLitString("Shutdown failed");
          asmb->addPCall(novasm::PCallCode::Assert);

          // Wait on the worker to finnish.
          asmb->addStackLoad(1);
          asmb->addFutureBlock();

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

  SECTION("Server shutdown cancels read request") {
    CHECK_PROG(
        [&](novasm::Assembler* asmb) -> void {
          asmb->setEntrypoint("entry");

          // --- Main function start.
          asmb->label("entry");
          asmb->addStackAlloc(2);

          // Start server.
          asmb->addLoadLitInt(0);    // Address family: IpV4.
          asmb->addLoadLitInt(5003); // Port.
          asmb->addLoadLitInt(-1);   // Backlog (-1 uses the default backlog).
          asmb->addPCall(novasm::PCallCode::TcpStartServer);
          asmb->addStackStore(0); // Store the server stream.

          // Open connection to server and send message.
          asmb->addLoadLitString(loopbackAddrIpV4);
          asmb->addLoadLitInt(0);    // Address family: IpV4.
          asmb->addLoadLitInt(5003); // Port.
          asmb->addPCall(novasm::PCallCode::TcpOpenCon);
          asmb->addPop(); // Ignore the stream to the opened connection.

          // Accept the connection on the server.
          asmb->addStackLoad(0);
          asmb->addPCall(novasm::PCallCode::TcpAcceptCon);

          // Start a background worker that will read a message.
          asmb->addCall("worker", 1, novasm::CallMode::Forked);
          asmb->addStackStore(1); // Place the future on the stack at slot 1.

          // Sleep for 100 milli-seconds to give the worker time to start.
          asmb->addLoadLitLong(100'000'000);
          asmb->addPCall(novasm::PCallCode::SleepNano);
          asmb->addPop(); // Ignore the return value of sleep.

          // Shutdown the server.
          asmb->addStackLoad(0);
          asmb->addPCall(novasm::PCallCode::TcpShutdown);

          // Assert that the shutdown completed successfully.
          asmb->addLoadLitString("Shutdown failed");
          asmb->addPCall(novasm::PCallCode::Assert);

          // Wait on the worker to finnish.
          asmb->addStackLoad(1);
          asmb->addFutureBlock();

          // Print the received message (should be empty).
          ADD_PRINT(asmb);
          asmb->addRet();
          // --- Main function end.

          // --- Worker function start (takes one stream arg and read a message).
          asmb->label("worker");
          asmb->addStackLoad(0);    // Load arg 0.
          asmb->addLoadLitInt(512); // Max message size.
          asmb->addPCall(novasm::PCallCode::StreamReadString);
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
