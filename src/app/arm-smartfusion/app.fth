\ Load file for application-specific Forth extensions
fl ../../cpu/arm/cortex-m3/bitband.fth
fl ../../lib/misc.fth
fl ../../lib/dl.fth
$3000 $5000 npatch load-base

: bitset  ( mask adr -- )  tuck l@ or swap l!  ;
: bitclr  ( mask adr -- )  tuck l@ swap invert and swap l!  ;


\ add leading zeros  ( value n -- )
: Z.R >R (U.) R> OVER - 200 MIN 0 MAX 0 ?DO ." 0" LOOP TYPE ;


: showdate forthdate@ dup 10 >> decimal 4 Z.R ." /" 
     dup 8 >> ff and 2 Z.R ." /" 
     ff and 2 Z.R 
     hex space ." v" forthversion@ 4 Z.R ; 

only forth also hidden also definitions
decimal

: x.head  (s addr len -- addr' len' )
   swap   dup -16 and  swap  15 and   ( cr )  adr-width 2+ spaces
   16 0 do  6 spaces  i ?.n  4 +loop rot +  ;

: x-dln   (s addr --- )
   ??cr   dup adr-width u.r   2 spaces
   dup 16 bounds do  i l@ .8  4 +loop  space ;

forth definitions
: xdump  ( addr len -- )
   push-hex
   x.head  ( addr len )
   bounds ?do   i x-dln  exit? ?leave  16 +loop
   pop-base
;

only forth also definitions





\ Replace 'quit' to make CForth auto-run some application code
\ instead of just going interactive.
: app  ." CForth " showdate cr hex quit  ;

\ " ../objs/tester" $chdir drop

" app.dic" save
