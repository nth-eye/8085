org 2000h
db 49h, 0Ah,             9Fh, 1Fh, 00h, 26h, 0CDh, 37h, 9Eh, 0F1h, 49h, 36h

org 1000h

start: ; 00
  lxi h,                 2000h
  lxi d,          2018h
  mvi c, 0Ch ; counter
copy: ; 08
  mov a, m
  stax d
  inx h
  inx d
  dcr c
  jnz copy
  mvi e, 0Bh
o_loop: ; 12
  lxi h, 2023h
  mov c, e ; counter
  mvi d, 0 ; flag
i_loop: ; 18
  mov a, m
  dcx h
  cmp m
  jnc check
swap: ; 1e
  mvi d, 1
  mov b, m
  mov m, a
  inx h
  mov m, b
  dcx h
check: ; 25
  dcr c
  jnz i_loop
  dcr e
  xra a
  cmp d
  jnz o_loop
end: ; 2f
  hlt
