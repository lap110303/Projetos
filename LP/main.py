from functools import lru_cache

MOD = 10**9

max_digit_sum_squares = 9**2 * 20
perfect_squares = set(i*i for i in range(1, int(max_digit_sum_squares**0.5) + 1))

@lru_cache(None)
def count(pos, tight, sum_sq, non_zero_digits):
    if pos == 0:
        return (1, 0) if sum_sq in perfect_squares and non_zero_digits else (0, 0)
    
    total_count = 0
    total_sum = 0
    limit = 9 if not tight else int(digits[-pos])
    
    for d in range(limit + 1):
        new_tight = tight and (d == limit)
        cnt, sm = count(
            pos - 1,
            new_tight,
            sum_sq + d*d,
            non_zero_digits or d > 0
        )
        total_count += cnt
        total_sum = (total_sum + sm + cnt * d * 10**(pos-1)) % MOD

    return total_count, total_sum

digits = str(10**20 - 1)
length = len(digits)

_, total_sum = count(length, True, 0, False)
print(str(total_sum).zfill(9))
