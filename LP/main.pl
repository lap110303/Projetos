:- use_module(library(lists)).

max_digit(9).
max_digits(20).

perfect_squares(Squares) :-
    MaxSum is 9*9*20,
    findall(S, (between(1, MaxSum, S), R is floor(sqrt(S)), R*R =:= S), Squares).

:- dynamic sum_table/4.

init_sum_table :-
    retractall(sum_table(_, _, _, _)),
    assertz(sum_table(0, 0, 1, 0)).

update_sum_table(Len, SumSq, CountAdd, SumAdd) :-
    (   sum_table(Len, SumSq, CountOld, SumOld)
    ->  NewCount is (CountOld + CountAdd) mod 1000000000,
        NewSum   is (SumOld + SumAdd) mod 1000000000,
        retract(sum_table(Len, SumSq, CountOld, SumOld)),
        assertz(sum_table(Len, SumSq, NewCount, NewSum))
    ;   assertz(sum_table(Len, SumSq, CountAdd, SumAdd))
    ).

build_sum_table_no_leading_zeros :-
    init_sum_table,
    max_digits(MaxLen),
    max_digit(MaxD),
    % Primeiro dígito: 1..9
    forall(
        between(1, MaxD, D),
        (
            NewLen = 1,
            NewSumSq is D*D,
            NewCount = 1,
            NewSum = D,
            assertz(sum_table(NewLen, NewSumSq, NewCount, NewSum))
        )
    ),
    MaxLen1 is MaxLen - 1,
    forall(
        between(1, MaxLen1, Len),
        forall(
            sum_table(Len, SumSq, Count, Sum),
            forall(
                between(0, MaxD, D),
                (
                    NewLen is Len + 1,
                    NewSumSq is SumSq + D*D,
                    NewCount is Count,
                    NewSum is (Sum * 10 + D * Count) mod 1000000000,
                    update_sum_table(NewLen, NewSumSq, NewCount, NewSum)
                )
            )
        )
    ).

total_sum(ModSum) :-
    perfect_squares(Squares),
    findall(Sum, (
        member(Sq, Squares),
        sum_table(_, Sq, _, Sum)
    ), Sums),
    sumlist(Sums, Total),
    ModSum is Total mod 1000000000.

main :-
    build_sum_table_no_leading_zeros,
    total_sum(Sum),
    format('Resultado: ~d~n', [Sum]).
