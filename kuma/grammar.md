program : sub_program

sub_program : begin declaration_table ; execution_table end

declaration_table : declaration
                | declaration_table ; declaration

declaration : variable_declaration
            | function_declaration

variable_declaration : integer variable

variable : identifier

identifier : alpha
            | identifier alpha
            | identifier digit
        
alpha : ...

digit : ...

function_declaration : integer function identifier(parameter) ; function_body

parameter : variable

function_body : begin declaration_table ; execution_table end

execution_table : execution
                | execution_table ; execution

execution : read_statement
            | write_statement
            | assignment
            | condition

read_statement : read(variable)

write_statement : write(variable)

assignment : variable:=expression

expression : expression-item
            | item

item : item*factor
        | factor

factor : variable
        | constant
        | function_call

constant : unsigned_int

unsigned_int : digit
                | unsigned_int digit

condition : if condition_expression then execution else execution

condition_expression : expression relational_operator expression

relational_operator : <
                    | <=
                    | >
                    | >=
                    | =
                    | <>

function_call : identifier(variable)
                | identifier()