\ Load file for application-specific Forth extensions
fl ../../cpu/arm/cortex-m3/bitband.fth
fl ../../lib/misc.fth
fl ../../lib/dl.fth
$3000 $5000 npatch load-base

: bitset  ( mask adr -- )  tuck l@ or swap l!  ;
: bitclr  ( mask adr -- )  tuck l@ swap invert and swap l!  ;

: test fpgadate@ u. fpgaversion@ u. forthdate@ u. forthversion@ u. ;

\ Replace 'quit' to make CForth auto-run some application code
\ instead of just going interactive.
: app  ." CForth" cr hex quit  ;

\ " ../objs/tester" $chdir drop

" app.dic" save
