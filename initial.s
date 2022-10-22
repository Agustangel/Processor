push 1
pop rbx
in
pop rcx
push rcx
out
push 0
pop rdx
call FACTORIAL:
FACTORIAL:
        push rdx
        push rcx
        jae END:
        push rdx
        push rbx
        mul
        pop rbx
        push 1
        push rdx
        add
        pop rdx
        call FACTORIAL:
END:
    push rbx
    out
    hlt
