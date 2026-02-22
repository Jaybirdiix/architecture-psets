//=========================================================================
// parcv2-loaduse-stall-byp.S
//=========================================================================

#include "parc-macros.h"

        TEST_PARC_BEGIN

        //-----------------------------------------------------------------
        // Load-use hazard: lw -> immediate consumer must stall
        // Then continued dependent ops ensure correct forwarding afterwards
        //-----------------------------------------------------------------

        // Use an existing data label so we don't depend on "random" memory.
        // The tests usually link with some data region; we define our own.

        .data
        .align 4
data_word:
        .word 42

        .text

        la    $2, data_word      // $2 = &data_word
        lw    $3, 0($2)          // $3 = 42

        // Immediate consumer: MUST get 43; this is the load-use case
        addiu $4, $3, 1          // $4 = 43

        // More dependences to exercise forwarding paths after the stall
        addiu $5, $4, 1          // $5 = 44
        addu  $6, $5, $4         // $6 = 44 + 43 = 87

        TEST_CHECK_EQ( $4, 43 )
        TEST_CHECK_EQ( $5, 44 )
        TEST_CHECK_EQ( $6, 87 )

        TEST_PARC_END
