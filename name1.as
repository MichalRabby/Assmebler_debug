mcr m_mcr
inc r2
mov A,r1
endmcr
.define sz = 2
mov r3,LIST[sz]
add label[2] , label[ 2 ]
.define y = 720
.define z = 1
LIST: .string "4234234124123    sfsdf lr ;sdf, sd qr    "
.extern W
.entry K
LABEL: jmp r7
K: cmp K, #sz
prn #-5
    mov STR[5],STR[2]
    sub r1,r4
        cmp K, #sz
    bne W
    mov r3,LIST[sz]
    jmp W
dec r1