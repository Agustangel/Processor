push 1
pop rbx
in
pop rcx
push 1
pop rdx
call FACTORIAL:
FACTORIAL:
        push rdx
        push rcx
        ja END:
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
