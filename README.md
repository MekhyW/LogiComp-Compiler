# LogiComp-Compiler

## Syntactic diagram 
![Syntactic diagram](Diagram.jpg)

## Extended Backus-Naur Form (EBNF)

<li>BLOCK = { STATEMENT };
<li>STATEMENT = ( "λ" | ASSIGNMENT | PRINT | WHILE | IF ), "\n" ;
<li>ASSIGNMENT = IDENTIFIER, "=", EXPRESSION ;
<li>PRINT = "print", "(", EXPRESSION, ")" ;
<li>WHILE = "while", BOOL_EXP, "do", "\n", "λ", { ( STATEMENT ), "λ" }, "end";
<li>IF = "if", BOOL_EXP, "then", "\n", "λ", { ( STATEMENT ), "λ" }, ( "λ" | ( "else", "\n", "λ", { ( STATEMENT ), "λ" })), "end" ;
<li>BOOL_EXP = BOOL_TERM, { ("or"), BOOL_TERM } ;
<li>BOOL_TERM = REL_EXP, { ("and"), REL_EXP } ;
<li>REL_EXP = EXPRESSION, { ("==" | ">" | "<"), EXPRESSION } ;
<li>EXPRESSION = TERM, { ("+" | "-"), TERM } ;
<li>TERM = FACTOR, { ("*" | "/" | "%"), FACTOR } ;
<li>FACTOR = NUMBER | IDENTIFIER | (("+" | "-" | "not"), FACTOR ) | "(", EXPRESSION, ")" | "read", "(", ")" ;
<li>IDENTIFIER = LETTER, { LETTER | DIGIT | "_" } ;
<li>NUMBER = DIGIT, { DIGIT } ;
<li>LETTER = ( "a" | "..." | "z" | "A" | "..." | "Z" ) ;
<li>DIGIT = ( "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | "0" ) ;

## Test status
![git status](http://3.129.230.99/svg/MekhyW/LogiComp-Compiler/)
