#include "novasm/assembler.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <set>

namespace backend::internal {

auto genLazyGet(novasm::Assembler* asmb) -> void {
  /* A 'lazy' object is represented by a struct with 2 fields:
      1: Atomic to indicate the state:
          0 = val not computed.
          1 = value is being computed.
          2 = value is computed.
      2: Closure to compute the value OR the computed value.

    When executing a 'get':
      - Try to change the atomic from 0 to 1.
      - If the old value was 0:
          * Execute a call to the closure at field 2.
          * Store the result to field 2.
          * Change the atomic from 1 to 2.
          * Get the resulting value from field 2.
      - If the old value was not 0:
          * Block until the atomic has a value of 2.
          * Get the resulting value from field 2.
  */

  auto notIdleLabel      = asmb->generateLabel("lazy-get-not-idle");
  auto returnResultLabel = asmb->generateLabel("lazy-get-return-result");

  // Note: we start with a single 'lazy' structure on the stack, to avoid having to store it in a
  // stack 'variable' we duplicate it twice (because we access it 3 times at least).
  asmb->addDup();
  asmb->addDup();

  asmb->addStructLoadField(0);      // Load the atomic.
  asmb->addAtomicCompareSwap(0, 1); // Attempt to set the atomic from 0 to 1.
  asmb->addJumpIf(notIdleLabel);    // If the old value was not 0 then jump.

  // Getting here means the value has not been computed yet.
  // Duplicate the lazy struct two times more as we need to access it two more times.
  asmb->addDup();
  asmb->addDup();

  // Execute the closure and store the result.
  asmb->addStructLoadField(1);                   // Load the closure.
  asmb->addCallDyn(0, novasm::CallMode::Normal); // Note: The closure takes no arguments.
  asmb->addStructStoreField(1);                  // Store the computed value at field 2.

  // Set the atomic to 2.
  asmb->addStructLoadField(0);      // Load the atomic
  asmb->addAtomicCompareSwap(1, 2); // Set the atomic from 0 to 1 (we know it will succeed).
  asmb->addPop();                   // Ignore the result of the compare and swap.
  asmb->addJump(returnResultLabel); // Skip over the waiting.

  // Wait for the value to be available.
  asmb->label(notIdleLabel);
  asmb->addStructLoadField(0); // Load the atomic.
  asmb->addAtomicBlock(2);     // Wait for the atomic to have the value 2.

  // Return.
  asmb->label(returnResultLabel);
  asmb->addStructLoadField(1); // Load the computed value.
}

} // namespace backend::internal
