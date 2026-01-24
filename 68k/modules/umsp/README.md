umsp:  User-Mode Service Package
================================

Privileged Instruction Emulation
--------------------------------

  * on 68010+, populates an override vector table and points VBR to it
    * override exception handlers just call low memory handlers
      * except one, see below
      * (this is required by MacRelix's system-info, see umsp.cc comments)
  * installs Privilege Violation exception handler
    * emulates some privileged instructions
      * needed for 68000 compatibility
        * MOVE from SR (substitutes for absent MOVE from CCR on 68000)
          * used as if unprivileged, but privileged on 68010+
      * used to disable and reenable interrupts in some games
        * MOVE to SR (allows CCR and interrupt mask only)
        * ORI to SR (allows CCR and interrupt mask only)
      * used (incorrectly) to set the CCR
        * ANDI to SR (allows CCR only)
      * for anything else, forwards to next handler
        * on 68000, the handler previously in that vector slot
        * on 68010+, the handler in the low memory vector table
      * for MOVE to/from SR, all 68000 addressing modes except PC-relative
