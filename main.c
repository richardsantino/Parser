#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"

#define DEBUG

// reconhecedor inicial
void programm(No_token *Tokens);

// reconhecedores intermediarios
No_token *rturn(No_token *Tokens);
No_token *data(No_token *Tokens);
No_token *fun_call(No_token *Tokens);
No_token *parameter(No_token *Tokens);
No_token *type(No_token *Tokens);
No_token *func(No_token *Tokens);
No_token *atribuition(No_token *Tokens);
No_token *atribuition_exist(No_token *Tokens);
No_token *while_statement(No_token *Tokens);
No_token *boolean(No_token *Tokens);
No_token *comp(No_token *Tokens);
No_token *else_statement(No_token *Tokens);
No_token *if_statement(No_token *Tokens);
No_token *condition(No_token *Tokens);
No_token *block(No_token *Tokens);

// reconhecedores intermediarios - exp matematica
No_token *expr(No_token *Tokens);

// reconhecedores finais
No_token *read_START(No_token *Tokens);
No_token *read_END(No_token *Tokens);
No_token *read_NUMERIC(No_token *Tokens);
No_token *read_INT(No_token *Tokens);
No_token *read_FLOAT(No_token *Tokens);
No_token *read_STRING(No_token *Tokens);
No_token *read_OPEN_PARENTHESES(No_token *Tokens);
No_token *read_CLOSE_PARENTHESES(No_token *Tokens);
No_token *read_COMMA(No_token *Tokens);
No_token *read_RETURN(No_token *Tokens);
No_token *read_QMARKS(No_token *Tokens);
No_token *read_ATTRIBUITION(No_token *Tokens);
No_token *read_TRUE(No_token *Tokens);
No_token *read_FALSE(No_token *Tokens);
No_token *read_END_FUNC(No_token *Tokens);
No_token *read_EQUAL(No_token *Tokens);
No_token *read_NOT_EQUAL(No_token *Tokens);
No_token *read_NEGATE(No_token *Tokens);
No_token *read_EQUAL_GREATER(No_token *Tokens);
No_token *read_EQUAL_LOWER(No_token *Tokens);
No_token *read_GREATER(No_token *Tokens);
No_token *read_LOWER(No_token *Tokens);
No_token *read_IF(No_token *Tokens);
No_token *read_END_IF(No_token *Tokens);
No_token *read_ELSE(No_token *Tokens);
No_token *read_WHILE(No_token *Tokens);
No_token *read_END_WHILE(No_token *Tokens);
No_token *read_NAME(No_token *Tokens);
No_token *read_SUM(No_token *Tokens);
No_token *read_SUB(No_token *Tokens);
No_token *read_DIV(No_token *Tokens);
No_token *read_MUL(No_token *Tokens);

// No_token *<>(No_token *Tokens);

int main(void){
    No_token *Tokens = lexer();

    if(Tokens == NULL) return -1;

    printf("------- PARSER -------\n\n");

    programm(Tokens);
    printf("-> PROGRAMM OK\n\n"); 
    
    return 0;
}

void programm(No_token *Tokens){
    while(1){
        if(Tokens->value == _INT || Tokens->value == _FLOAT || Tokens->value == STRING) Tokens = func(Tokens);
        else break;
    }

    Tokens = read_START(Tokens);
    Tokens = block(Tokens);
    Tokens = read_END(Tokens);

    if(Tokens != NULL){ // se ainda existir token depois do END
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *block(No_token *Tokens){

    while(1){
        if(Tokens->value == IF) Tokens = if_statement(Tokens);                  // if
        else if(Tokens->value == WHILE) Tokens = while_statement(Tokens);       // while
        else if(Tokens->value == _INT || Tokens->value == _FLOAT || Tokens->value == STRING){ // atribuition || func
            Tokens = type(Tokens);
            Tokens = read_NAME(Tokens);

            if(Tokens->value == ATRIBUICAO){ // atribuition
                Tokens = read_ATTRIBUITION(Tokens);
                Tokens = data(Tokens);
            }
            else{ // func
                Tokens = read_OPEN_PARENTHESES(Tokens);
                Tokens = parameter(Tokens);
                Tokens = read_CLOSE_PARENTHESES(Tokens);
                Tokens = block(Tokens);
                Tokens = rturn(Tokens);
                Tokens = read_END_FUNC(Tokens);
            }
        }
        else if(Tokens->value == NOME){ //fun call ou atribuition_exist(ente)
            if(Tokens->prox->value == ABRE_P) Tokens = fun_call(Tokens);
            else Tokens = atribuition_exist(Tokens);
        } 
        else break;
    }
     
    return Tokens;
}

No_token *expr(No_token *Tokens){
    int aux = 0;

    if(Tokens->value == ABRE_P){
        aux = 1;
        Tokens = read_OPEN_PARENTHESES(Tokens);
    }

    if(Tokens->value == NUMERIC) Tokens = read_NUMERIC(Tokens);
    else Tokens = read_NAME(Tokens);

    while(1){
        if(Tokens->type == OPERADOR){
            if(Tokens->value == SOMA) Tokens = read_SUM(Tokens);
            if(Tokens->value == SUBT) Tokens = read_SUB(Tokens);
            if(Tokens->value == DIV) Tokens = read_DIV(Tokens);
            if(Tokens->value == MULT) Tokens = read_MUL(Tokens);

            Tokens = expr(Tokens);
        }
        else break;        
    }

    if(aux == 1) Tokens = read_CLOSE_PARENTHESES(Tokens);

    if(Tokens->type == OPERADOR){
        if(Tokens->value == SOMA) Tokens = read_SUM(Tokens);
        if(Tokens->value == SUBT) Tokens = read_SUB(Tokens);
        if(Tokens->value == DIV) Tokens = read_DIV(Tokens);
        if(Tokens->value == MULT) Tokens = read_MUL(Tokens);

        Tokens = expr(Tokens);
    }    

    return Tokens;
}

No_token *data(No_token *Tokens){
    if(Tokens->prox->type == OPERADOR || Tokens->value == ABRE_P) return expr(Tokens);
    else if(Tokens->value == NUMERIC){ // num
        return read_NUMERIC(Tokens); 
    }
    else if(Tokens->value == NOME){ // name or fun call
        if(Tokens->prox->value == ABRE_P) return fun_call(Tokens); // fun call
        return read_NAME(Tokens); // name
    }
    else if(Tokens->value == ASPAS){ // Q_MARKS name Q_MARKS
        Tokens = read_QMARKS(Tokens);
        Tokens = read_NAME(Tokens);
        return read_QMARKS(Tokens);
    }
    else return Tokens->prox;

    return Tokens;
}

No_token *if_statement(No_token *Tokens){
    Tokens = read_IF(Tokens);
    Tokens = read_OPEN_PARENTHESES(Tokens);
    Tokens = condition(Tokens);
    Tokens = read_CLOSE_PARENTHESES(Tokens);
    Tokens = block(Tokens);
    if(Tokens->value == ELSE) Tokens = else_statement(Tokens);
    return read_END_IF(Tokens);
}

No_token *else_statement(No_token *Tokens){
    Tokens = read_ELSE(Tokens);
    Tokens = block(Tokens);
    return Tokens;
}

No_token *comp(No_token *Tokens){
    if(Tokens->value == EQU) return read_EQUAL(Tokens);
    else if(Tokens->value == DIF) return read_NOT_EQUAL(Tokens);
    else if(Tokens->value == MAIOR_IGUAL) return read_EQUAL_GREATER(Tokens);
    else if(Tokens->value == MENOR_IGUAL) return read_EQUAL_LOWER(Tokens);
    else if(Tokens->value == MAIOR) return read_GREATER(Tokens);
    else if(Tokens->value == MENOR) return read_LOWER(Tokens);

    return Tokens;
}

No_token *boolean(No_token *Tokens){
    if(Tokens->value == TRUE) return read_TRUE(Tokens);
    else if(Tokens->value == FALSE) return read_FALSE(Tokens);

    return Tokens;
}

No_token *condition(No_token *Tokens){
    if(Tokens->value == NEGADO){
        Tokens = read_NEGATE(Tokens);
        Tokens = boolean(Tokens);
    }
    else if(Tokens->value == TRUE || Tokens->value == FALSE){
        Tokens = boolean(Tokens);
    }
    else{
        Tokens = data(Tokens);
        Tokens = comp(Tokens);
        Tokens = data(Tokens);
    }

    return Tokens;   
}

No_token *while_statement(No_token *Tokens){
    Tokens = read_WHILE(Tokens);
    Tokens = read_OPEN_PARENTHESES(Tokens);
    Tokens = condition(Tokens);
    Tokens = read_CLOSE_PARENTHESES(Tokens);
    Tokens = block(Tokens);
    return read_END_WHILE(Tokens);
}

No_token *atribuition(No_token *Tokens){
    Tokens = type(Tokens);
    Tokens = read_NAME(Tokens);
    Tokens = read_ATTRIBUITION(Tokens);
    return data(Tokens);
}

No_token *atribuition_exist(No_token *Tokens){
    Tokens = read_NAME(Tokens);
    Tokens = read_ATTRIBUITION(Tokens);
    return data(Tokens);
}

No_token *func(No_token *Tokens){
    Tokens = type(Tokens);
    Tokens = read_NAME(Tokens);
    Tokens = read_OPEN_PARENTHESES(Tokens);
    Tokens = parameter(Tokens);
    Tokens = read_CLOSE_PARENTHESES(Tokens);
    Tokens = block(Tokens);
    Tokens = rturn(Tokens);
    return read_END_FUNC(Tokens);
}

No_token *type(No_token *Tokens){
    if(Tokens->value == _INT) return read_INT(Tokens);
    else if(Tokens->value == _FLOAT) return read_FLOAT(Tokens);
    else if(Tokens->value == STRING) return read_STRING(Tokens);

    return Tokens;
}

No_token *rturn(No_token *Tokens){
    Tokens = read_RETURN(Tokens);
    return data(Tokens);
}

No_token *fun_call(No_token *Tokens){
    // name -> ( -> parameter -> )
    
    Tokens = read_NAME(Tokens); // name
    Tokens = read_OPEN_PARENTHESES(Tokens); // (
    Tokens = parameter(Tokens); // parameter
    return read_CLOSE_PARENTHESES(Tokens); // )
}

No_token *parameter(No_token *Tokens){
    if(Tokens->value == FECHA_P) return Tokens;

    do{
        Tokens = data(Tokens);
        if(Tokens->value == VIRGULA) Tokens = read_COMMA(Tokens);
    } while (Tokens->value != FECHA_P);
    
    return Tokens;
}

// Leituras dos tokens:

No_token *read_START(No_token *Tokens){
    // printf("* esperando por START *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == START){ 
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_END(No_token *Tokens){
    // printf("* esperando por END *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == END){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_NUMERIC(No_token *Tokens){
    // printf("* esperando por NUMERO *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if((Tokens->type == INT || Tokens->type == FLOAT) && Tokens->value == NUMERIC){
        // printf("OK - %d\n\n", Tokens->n);
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_INT(No_token *Tokens){
    // printf("* esperando por INT *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n); 

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == _INT){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }      
}

No_token *read_FLOAT(No_token *Tokens){
    // printf("* esperando por FLOAT *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n); 

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == _FLOAT){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }      
}

No_token *read_STRING(No_token *Tokens){
    // printf("* esperando por STRING *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == STRING){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_OPEN_PARENTHESES(No_token *Tokens){
    // printf("* esperando por ( *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PONTUACAO && Tokens->value == ABRE_P){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_CLOSE_PARENTHESES(No_token *Tokens){
    // printf("* esperando por ) *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PONTUACAO && Tokens->value == FECHA_P){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_COMMA(No_token *Tokens){
    // printf("* esperando por , *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PONTUACAO && Tokens->value == VIRGULA){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_RETURN(No_token *Tokens){
    // printf("* esperando por RETURN *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == RETURN){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_QMARKS(No_token *Tokens){
    // printf("* esperando por \" *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PONTUACAO && Tokens->value == ASPAS){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_ATTRIBUITION(No_token *Tokens){
    // printf("* esperando por = *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PONTUACAO && Tokens->value == ATRIBUICAO){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_TRUE(No_token *Tokens){
    // printf("* esperando por TRUE *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == TRUE){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_FALSE(No_token *Tokens){
    // printf("* esperando por FALSE *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == FALSE){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_END_FUNC(No_token *Tokens){
    // printf("* esperando por END FUNC *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == END_FUNC){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_EQUAL(No_token *Tokens){
    // printf("* esperando por == *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == EQU){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_NOT_EQUAL(No_token *Tokens){
    // printf("* esperando por != *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == DIF){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_NEGATE(No_token *Tokens){
    // printf("* esperando por ! *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == NEGADO){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_EQUAL_GREATER(No_token *Tokens){
    // printf("* esperando por >= *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == MAIOR_IGUAL){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_EQUAL_LOWER(No_token *Tokens){
    // printf("* esperando por <= *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == MENOR_IGUAL){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_GREATER(No_token *Tokens){
    // printf("* esperando por > *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == MAIOR){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_LOWER(No_token *Tokens){
    // printf("* esperando por < *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == MENOR){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_IF(No_token *Tokens){
    // printf("* esperando por IF *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == IF){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_END_IF(No_token *Tokens){
    // printf("* esperando por END IF *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == END_IF){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_ELSE(No_token *Tokens){
    // printf("* esperando por ELSE *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == ELSE){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_WHILE(No_token *Tokens){
    // printf("* esperando por WHILE *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == WHILE){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_END_WHILE(No_token *Tokens){
    // printf("* esperando por END WHILE *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == PALAVRA_RESERVADA && Tokens->value == END_WHILE){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_NAME(No_token *Tokens){
    // printf("* esperando por <NAME> *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == INDENTIFICADOR && Tokens->value == NOME){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_SUM(No_token *Tokens){
    // printf("* esperando por SOMA *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == OPERADOR && Tokens->value == SOMA){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_SUB(No_token *Tokens){
    // printf("* esperando por SUBTRACAO *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == OPERADOR && Tokens->value == SUBT){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_DIV(No_token *Tokens){
    // printf("* esperando por DIVISAO *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == OPERADOR && Tokens->value == DIV){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

No_token *read_MUL(No_token *Tokens){
    // printf("* esperando por MULTIPLICACAO *\n");
    // printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

    if(Tokens->type == OPERADOR && Tokens->value == MULT){
        // printf("OK\n\n");
        return go_to_next_token(Tokens);
    }
    else{
        printf("NOT OK - sintax error\n\n");
        exit(1);
    }
}

// No_token *<>(No_token *Tokens){
//     printf("* esperando por <> *\n");
//     printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

//     if(Tokens->type == <> && Tokens->value == <>>){
//         printf("OK\n\n");
//         return go_to_next_token(Tokens);
//     }
//     else{
//         printf("NOT OK\n\n");
//         exit(1);
//     }
// }

// No_token *<>(No_token *Tokens){
//     printf("* esperando por <> *\n");
//     printf("lendo -> (type)<%d> (value)<%d> (literal)<%d>\n", Tokens->type, Tokens->value, Tokens->n);

//     if(Tokens->type == <> && Tokens->value == <>>){
//         printf("OK\n\n");
//         return go_to_next_token(Tokens);
//     }
//     else{
//         printf("NOT OK\n\n");
//         exit(1);
//     }
// }