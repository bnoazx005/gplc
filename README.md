**Title**:   gplc

**Author**:  Kasimov Ildar

**e-mail**:  ildar2571@yandex.ru

**Version**: 0.1a

**Description**:

Gplc is a compiler for the gpl (non final project name) programming language.

**BNF description of the language**

(not finished yet)

    <program-unit> ::= <statements>;

    <statements> ::=   <statement> 
                     | <statement> <statements>;

    <statement> ::= <operator> ; ;                                  // add here <directive> here later

    <operator> ::= <declaration>;

    <declaration> ::=   <attributes> <identifiers> : <type>
                      | <identifiers> : <type>;

    <identifiers> ::=   <identifier>
                      | <identifier> , <identifiers>;


    <attributes> ::= ;                                             // empty by now

    <type> ::=   <builtin_type>
               | <identifier>
               | <struct_declaration>
               | <enum_declaration>
               | <func_declaration>;