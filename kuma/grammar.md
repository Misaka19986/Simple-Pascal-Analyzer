
# 1 
program : sub_program

# 2
sub_program : begin declaration_table ; execution_table end

# 3
declaration_table : declaration
                | declaration_table ; declaration

# 3'
declaration_table : declaration declaration_table_prime
declaration_table_prime : ; declaration declaration_table_prime
                        | null

FOLLOW = {;}

# 4
declaration : variable_declaration
            | function_declaration
            | null

FOLLOW = {;}

# 5
variable_declaration : integer variable

# 6
variable : identifier

identifier : alpha
            | identifier alpha
            | identifier digit
        
alpha : ...

digit : ...

# 7
function_declaration : integer function identifier(parameter) ; function_body

# 8
parameter : variable

# 9
function_body : begin declaration_table ; execution_table end

# 10
execution_table : execution
                | execution_table ; execution

# 10'
execution_table : execution execution_table_prime
execution_table_prime : ; execution execution_table_prime
                        | null

FOLLOW = {;, end}

# 11
execution : read_statement
            | write_statement
            | assignment
            | condition
            | null

FOLLOW = {;}

# 12
read_statement : read(variable)

# 13
write_statement : write(variable)

# 14
assignment : variable:=expression

# 15
expression : expression-item
            | item

# 15'
expression : item expression_prime
expression_prime : -item expression_prime | null

FOLLOW = {;}

# 16
item : item*factor
        | factor

# 16'
item : factor item_prime
item_prime : *factor item_prime
                | null


# 17
factor : variable
        | constant
        | function_call

constant : unsigned_int

unsigned_int : digit
                | unsigned_int digit

# 18
condition : if condition_expression then execution else execution

# 19
condition_expression : expression relational_operator expression

# 20
relational_operator : <
                    | <=
                    | >
                    | >=
                    | =
                    | <>

# 21
function_call : identifier(expression)
                | identifier()