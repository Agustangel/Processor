in
pop rbx
in
pop rcx
in
pop rdx
push 19
pop rax
call SolveSquareEquation:
hlt
SolveSquareEquation:
    push 0
    push rbx
    je SolveLinearEquation:
    push rcx
    dup
    mul
    push 4
    push rbx
    mul
    push rdx
    mul
    sub
    pop rdx
    push rdx
    push 0
    jb noEqSolutions:
    push rdx
    push 0
    je isNullDiscr:
    push -1
    push rcx
    mul
    push rdx
    sqrt
    add
    push 2
    push rbx
    mul
    div
    out
    push -1
    push rcx
    mul
    push rdx
    sub
    push 2
    push rbx
    mul
    div
    out
    ret
    isNullDiscr:
        push -1
        push rcx
        mul
        out
        hlt
        out
        hlt
        push 2
        push rbx
        mul
        div
        out
        ret
    SolveLinearEquation:
        push 0
        push rcx
        je  isNullB:
        jmp isNotNullB:
        isNullB:
            push 0
            push rdx
            je infEqSolutions:
            jmp noEqSolutions:
        isNotNullB:
            push -1
            push rdx
            mul
            push rcx
            div
            out
            ret             
    noEqSolutions:
        push -111
        out
        ret
    infEqSolutions:
        push -122
        out
        ret
