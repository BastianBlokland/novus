#include "gen_type_eq.hpp"
#include "utilities.hpp"
#include <algorithm>
#include <set>

namespace backend::internal {

auto genLazyGet(novasm::Assembler* asmb) -> void {
  /* A 'lazy' object is represented by a struct with 3 fields:
      1: Flag to indicate if the value has been computed.
      2: Closure to call to compute the value.
      3: Computed value (or empty)

    When executing a 'get' we check if the flag in the first field is set:
      - If so: Return the value at field 3.
      - If not:
          * Execute a call to the closure at field 2.
          * Store the result of the call to field 3.
          * Set the flag at field 1.

    Note: We cannot simply alias field 2 and 3 because multiple threads could call 'get' on the same
    lazy at the same time. Thats why we need to leave the 'closure' in place even after our call
    completes.
  */

  auto alreadyComputedLabel = asmb->generateLabel("lazy-already-computed");

  // Note: we start with a single 'lazy' structure on the stack, to avoid having to store it in a
  // stack 'variable' we duplicate it (because we access it 2 times at least).
  asmb->addDup();

  // Load the 'flag' at field 1.
  asmb->addStructLoadField(0);
  asmb->addJumpIf(alreadyComputedLabel);

  // Getting here means the value has not been computed yet.
  // Duplicate the lazy struct three times more as we need to access it three more times.
  asmb->addDup();
  asmb->addDup();
  asmb->addDup();

  // Load the closure at field 2 and call it (Note: the closure takes no arguments).
  asmb->addStructLoadField(1);
  asmb->addCallDyn(0, novasm::CallMode::Normal);

  // Store computed value in field 3.
  asmb->addStructStoreField(2);

  // Set the flag in field 1 to 'true'.
  // Note: we still had a lazy struct on the stack due to the dup's we did before)
  asmb->addLoadLitInt(1);
  asmb->addStructStoreField(0);

  // Load the computed value at field 3.
  asmb->label(alreadyComputedLabel);
  asmb->addStructLoadField(2);
}

} // namespace backend::internal
